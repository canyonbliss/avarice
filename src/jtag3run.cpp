/*
 *	avarice - The "avarice" program.
 *	Copyright (C) 2012 Joerg Wunsch
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License Version 2
 *      as published by the Free Software Foundation.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111, USA.
 *
 * This file implements target execution handling for the JTAGICE3 protocol.
 */

#include <cstdio>

#include "jtag3.h"

unsigned long Jtag3::getProgramCounter() {
    if (cached_pc_is_valid)
        return cached_pc;

    uchar *resp;
    int respsize;
    const uchar cmd[] = {SCOPE_AVR, CMD3_READ_PC, 0};
    int cnt = 0;

again:
    try {
        doJtagCommand(cmd, sizeof(cmd), "read PC", resp, respsize);
    } catch (jtag3_io_exception &e) {
        cnt++;
        if (e.get_response() == RSP3_FAIL_WRONG_MODE && cnt < 2) {
            interruptProgram();
            goto again;
        }
        BOOST_LOG_TRIVIAL(error) << "cannot read program counter: " << e.what();
        throw;
    } catch (jtag_exception &e) {
        BOOST_LOG_TRIVIAL(error) << "cannot read program counter: " << e.what();
        throw;
    }

    unsigned long result = b4_to_u32(resp + 3);
    delete[] resp;

    // The JTAG box sees program memory as 16-bit wide locations. GDB
    // sees bytes. As such, double the PC value.
    result *= 2;

    cached_pc_is_valid = true;
    return cached_pc = result;
}

void Jtag3::setProgramCounter(unsigned long pc) {
    uchar *resp;
    int respsize;
    uchar cmd[7] = {SCOPE_AVR, CMD3_WRITE_PC};

    u32_to_b4(cmd + 3, pc / 2);

    try {
        doJtagCommand(cmd, sizeof(cmd), "write PC", resp, respsize);
    } catch (jtag_exception &e) {
        BOOST_LOG_TRIVIAL(error) << "cannot write program counter: " << e.what();
        throw;
    }

    delete[] resp;

    cached_pc_is_valid = false;
}

void Jtag3::resetProgram(bool) {
    uchar cmd[] = {SCOPE_AVR, CMD3_RESET, 0, 0x01};
    uchar *resp;
    int respsize;

    doJtagCommand(cmd, sizeof(cmd), "reset", resp, respsize);
    delete[] resp;

    /* Await the BREAK event that is posted by the ICE. */
    bool bp, gdb;
    expectEvent(bp, gdb);

    /* The PC value in the event returned after a RESET is the
     * PC where the reset actually hit, so ignore it. */
    cached_pc_is_valid = false;
}

void Jtag3::interruptProgram() {
    uchar cmd[] = {SCOPE_AVR, CMD3_STOP, 0, 0x01};
    uchar *resp;
    int respsize;

    doJtagCommand(cmd, sizeof(cmd), "stop", resp, respsize);
    delete[] resp;

    bool bp, gdb;
    expectEvent(bp, gdb);
}

void Jtag3::resumeProgram() {
    xmegaSendBPs();

    doSimpleJtagCommand(CMD3_CLEANUP, "cleanup");

    doSimpleJtagCommand(CMD3_GO, "go");

    cached_pc_is_valid = false;
}

void Jtag3::expectEvent(bool &breakpoint, bool &gdbInterrupt) {
    uchar *evtbuf;
    int evtsize;
    unsigned short seqno;

    if (cached_event != nullptr) {
        evtbuf = cached_event;
        cached_event = nullptr;
    } else {
        evtsize = recvFrame(evtbuf, seqno);
        if (evtsize > 0) {
            // XXX if not event, should push frame back into queue...
            // We really need a queue of received frames.
            if (seqno != 0xffff) {
                BOOST_LOG_TRIVIAL(debug) << "Expected event packet, got other response";
                return;
            }
        } else {
            BOOST_LOG_TRIVIAL(debug) << "Timed out waiting for an event";
            return;
        }
    }

    breakpoint = gdbInterrupt = false;

    switch ((evtbuf[0] << 8) | evtbuf[1]) {
    // Program stopped at some kind of breakpoint.
    // On Xmega, byte 7 denotes the reason:
    //   0x01 soft BP
    //   0x10 hard BP (byte 8 contains BP #, or 3 for data BP)
    //   0x20, 0x21 "run to address" or single-step
    //   0x40 reset, leave progmode etc.
    // On megaAVR, byte 6 , byte 7 are likely the "break status
    // register" (MSB, LSB), see here:
    // http://people.ece.cornell.edu/land/courses/ece4760/FinalProjects/s2009/jgs33_rrw32/Final%20Paper/index.html
    // The bits do not fully match that description but to
    // a good degree.
    case (SCOPE_AVR << 8) | EVT3_BREAK:
        if ((!is_xmega && evtbuf[7] != 0) || (is_xmega && evtbuf[7] != 0x40)) {
            // program breakpoint
            cached_pc = 2 * b4_to_u32(evtbuf + 2);
            cached_pc_is_valid = true;
            breakpoint = true;
            BOOST_LOG_TRIVIAL(debug) << format{"caching PC: 0x%04x"} % cached_pc;
        } else {
            BOOST_LOG_TRIVIAL(debug) << "ignoring break event";
        }
        break;

    case (SCOPE_AVR << 8) | EVT3_IDR:
        BOOST_LOG_TRIVIAL(debug) << format{"IDR dirty: 0x%02x"} % evtbuf[3];
        break;

    case (SCOPE_GENERAL << 8) | EVT3_POWER:
        if (evtbuf[3] == 0) {
            gdbInterrupt = true;
            BOOST_LOG_TRIVIAL(debug) << "Target power turned off";
        } else {
            BOOST_LOG_TRIVIAL(debug) << "Target power returned";
        }
        break;

    case (SCOPE_GENERAL << 8) | EVT3_SLEEP:
        if (evtbuf[3] == 0) {
            // gdbInterrupt = true;
            BOOST_LOG_TRIVIAL(debug) << "Target went to sleep";
        } else {
            // gdbInterrupt = true;
            BOOST_LOG_TRIVIAL(debug) << "Target went out of sleep";
        }
        break;

    default:
        gdbInterrupt = true;
        BOOST_LOG_TRIVIAL(warning) << format{"Unhandled JTAGICE3 event: 0x%02x, 0x%02x"} % evtbuf[0] % evtbuf[1];
    }

    delete[] evtbuf;
}

bool Jtag3::eventLoop(Server &server) {
    int maxfd;
    fd_set readfds;
    bool breakpoint = false, gdbInterrupt = false;

    // Now that we are "going", wait for either a response from the JTAG
    // box or a nudge from GDB.

    for (;;) {
        BOOST_LOG_TRIVIAL(debug) << "Waiting for input.";

        // Check for input from JTAG ICE (breakpoint, sleep, info, power)
        // or gdb (user break)
        FD_ZERO(&readfds);
        if (server.GetHandle() != -1)
            FD_SET(server.GetHandle(), &readfds);
        FD_SET(jtagBox, &readfds);
        if (server.GetHandle() != -1)
            maxfd = jtagBox > server.GetHandle() ? jtagBox : server.GetHandle();
        else
            maxfd = jtagBox;

        int numfds = select(maxfd + 1, &readfds, nullptr, nullptr, nullptr);
        if (numfds < 0)
            throw jtag_exception("GDB/JTAG ICE communications failure");

        if (server.GetHandle() != -1 && FD_ISSET(server.GetHandle(), &readfds)) {
            const auto c = server.getDebugChar();
            if (c == 3) // interrupt
            {
                BOOST_LOG_TRIVIAL(debug) << "interrupted by GDB";
                gdbInterrupt = true;
            } else
                BOOST_LOG_TRIVIAL(warning) << format{"Unexpected GDB input `%02x'"} % c;
        }

        if (FD_ISSET(jtagBox, &readfds)) {
            expectEvent(breakpoint, gdbInterrupt);
        }

        // We give priority to user interrupts
        if (gdbInterrupt)
            return false;
        if (breakpoint)
            return true;
    }
}

void Jtag3::jtagSingleStep() {
    uchar cmd[] = {SCOPE_AVR, CMD3_STEP, 0, 0x01, 0x01};
    uchar *resp;
    int respsize;

    xmegaSendBPs();

    cached_pc_is_valid = false;

    try {
        doJtagCommand(cmd, sizeof(cmd), "single-step", resp, respsize);
    } catch (jtag3_io_exception &e) {
        if (e.get_response() != RSP3_FAIL_WRONG_MODE)
            throw;
    }
    delete[] resp;

    bool bp, gdb;
    expectEvent(bp, gdb);
}

bool Jtag3::jtagContinue(Server &server) {
    updateBreakpoints(); // download new bp configuration

    xmegaSendBPs();

    if (cached_event != nullptr) {
        delete[] cached_event;
        cached_event = nullptr;
    }

    doSimpleJtagCommand(CMD3_GO, "go");

    return eventLoop(server);
}
