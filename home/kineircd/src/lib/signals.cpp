/* $Id$
 * Signal handlers
 * 
 * Copyright (c) 2001,2002 KineIRCd Development Team
 * (See DEV-TEAM file for details)
 *
 * This file is a part of KineIRCd.
 * 
 * KineIRCd is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * KineIRCd is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with KineIRCd; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifdef HAVE_CONFIG_H
# include "autoconf.h"
#endif
#include "kineircd/kineircdconf.h"

#include <cstdlib>
#include <csignal>
#include <algorithm>
#include <functional>
#include <aisutil/string/string.h>

#include "kineircd/signals.h"
#include "kineircd/exit.h"
#include "debug.h"

using namespace Kine;
using AISutil::String;


// Our instance...
Signals* Signals::instance = 0;


/* checkMask - Check if a handler should be called based on the mask
 * Original 09/07/2002 simonb
 */
struct checkMask {
   const int signum;
   const Signals::mask_type maskBit;
   
   // Constructor
   checkMask(const int s, const Signals::mask_type m)
     : signum(s), maskBit(m)
     {};
   
   // Broadcaster, to make sure the signal is distributed to those who want it
   void operator()(const Signals::handlerInfo& handlerInfo)
     {
	if (handlerInfo.mask & maskBit) {
	   (*handlerInfo.handler)(signum, maskBit, handlerInfo.foo);
	}
     };
};


/* sigHandler - Handle signals
 * Original 11/08/2001 simonb
 * Note: If this is to compile on other systems we may need the header
 * 	 RETSIGTYPE sigHandler(int sig, int code, struct sigcontext *context) 
 *       right?
 */
static RETSIGTYPE signalHandler(int signum)
{
#ifdef KINE_DEBUG
   debug("sigHandler() -> [" + String::convert(signum) +
# ifdef SYS_SIGLIST_DECLARED
	 "] " + String(sys_siglist[signum])
# else
	 "]"
# endif
	 );
#endif

   // If the handlers list is not empty, use it
   if (!signals().handlers.empty()) {
      Signals::mask_type maskBit;
      
      // Work out what bit in the mask we are looking for
      switch (signum) {
	 // Rehash
       case SIGHUP:
	 maskBit = Signals::REHASH;
	 break;
	 
	 // Violent Death
       case SIGILL:
       case SIGTRAP:
#ifdef SIGEMT
       case SIGEMT:
#endif
#ifdef SIGBUS
       case SIGBUS:
#endif
#ifdef SIGSEGV
       case SIGSEGV:
#endif
#ifdef SIGSYS
       case SIGSYS:
#endif
#ifdef SIGURG
       case SIGURG:
#endif
       case SIGFPE:
#ifdef SIGXCPU
       case SIGXCPU:
#endif
#ifdef SIGXFSZ
       case SIGXFSZ:
#endif
	 maskBit = Signals::VIOLENT_DEATH;
	 break;
	 
	 // Peaceful death
       case SIGINT:
       case SIGQUIT:
       case SIGTERM:
	 maskBit = Signals::PEACEFUL_DEATH;
	 break;
	 
	 // The user-defined signals
       case SIGUSR1:
	 maskBit = Signals::USER_1;
	 break;
       case SIGUSR2:
	 maskBit = Signals::USER_2;
	 break;
	 
	 // Everything else we should have already handled by now!
       default:
#ifdef KINE_DEBUG
	 // Complain!!
	 debug("We did not know how to handle this signal!!!");
#endif
	 
	 // Return (and not reset this signal)
	 return;
      }
      
      // Run through our handler list and look for handlers to call
      (void)std::for_each(signals().handlers.begin(), signals().handlers.end(),
			  checkMask(signum, maskBit));
   } else {
      // This could be nicer..
#ifdef KINE_DEBUG
      debug("No signal handlers known yet - eek!");
#endif
      abort();
   }
	
   // Reset the signal for future handling. Some os's do not need this??
   signal(signum, signalHandler);
}


// A list of what to do with signals to iterate over
struct {
   const int signal;	// We specify the signal because the defines change
   const bool handle;	// If false, we will set SIG_IGN on it
} static const signalHandlerInstructions[] = {
     { SIGHUP,		true },	// Hangup detected on controlling terminal
     { SIGINT,		true },	// Interrupt from keyboard (^C)
     { SIGQUIT,		true },	// Quit from keyboard (^D)
     { SIGILL,		true },	// Illegal Instruction
     { SIGFPE,		true },	// Floating point exception
     { SIGSEGV,		true },	// Invalid memory reference (segment violation)
     { SIGPIPE,		false },// Broken pipe: write to pipe with no readers
     { SIGALRM,		true },	// Timer signal from alarm() (or lame sleep())
#ifdef SIGVTALRM
     { SIGVTALRM,	true },	// Virtual alarm clock
#endif
     { SIGTERM,		true },	// Termination signal
     { SIGUSR1,		true },	// User-defined signal 1
     { SIGUSR2,		true },	// User-defined signal 2
     { SIGCHLD,		true },	// Child stopped or terminated
#ifdef SIGCLD
     { SIGCLD,		true },	// Child stopped or terminated (SIGCHLD)
#endif
#ifndef KINE_DEBUG // Only compile this bit if we are not debugging
     { SIGCONT,		true },	// Continue if stopped
     { SIGTSTP,		true },	// Stop typed at tty
     { SIGTTIN,		true },	// tty input for background process
     { SIGTTOU,		true },	// tty output for background process
#endif
#ifdef SIGBUS
     { SIGBUS,		true },	// Bus error (bad memory access)
#endif
#ifdef SIGPOLL
     { SIGPOLL,		true },	// Pollable event (SIGIO)
#endif
#ifdef SIGSYS
     { SIGSYS,		true },	// Bad argument to routine (system call)
#endif
#ifdef SIGURG
     { SIGURG,		false },// Urgent condition on socket
#endif
#ifdef SIGXCPU
     { SIGXCPU,		true },	// CPU time limit exceeded (process quota)
#endif
#ifdef SIGXFSZ
     { SIGXFSZ,		true },	// File size limit exceeded (file quota)
#endif
#ifdef SIGEMT
     { SIGEMT,		false },// What is this?
#endif
#ifdef SIGSTKFLT
     { SIGSTKFLT,	true },	// Stack fault on coprocessor
#endif
#ifdef SIGIO
     { SIGIO,		true },	// I/O now possible
#endif
#ifdef SIGPWR
     { SIGPWR,		true },	// Power failure
#endif
#ifdef SIGINFO
     { SIGINFO,		true }, // SIGPWR
#endif
#ifdef SIGLOST
     { SIGLOST,		false },// File lock lost (obsolete?)
#endif
#ifdef SIGWINCH
     { SIGWINCH,	false },// Window resize signal
#endif
     { 0, false }
};


/* Signals - Constructor for the signal handler class
 * Original 07/07/2002 simonb
 */
Signals::Signals(void)
{
#ifdef KINE_DEBUG_PSYCHO
   debug("Signals::Signals() - Setting up signal handlers...");
#endif

#ifdef KINE_DEBUG_ASSERT
   // Make sure the list is really empty
   assert(handlers.empty());
#endif
   
   // Run through our signal instruction list...
   for (unsigned int i = 0; signalHandlerInstructions[i].signal != 0; i++) {
      if (signalHandlerInstructions[i].handle) {
	 (void)signal(signalHandlerInstructions[i].signal, signalHandler);
      } else {
	 (void)signal(signalHandlerInstructions[i].signal, SIG_IGN);
      }
   }
}


/* ~Signals - Destructor for the signal handler class
 * Original 07/07/2002 simonb
 */
Signals::~Signals(void)
{
#ifdef KINE_DEBUG_PSYCHO
   debug("Signals::~Signals() - Defaulting signal handlers...");
#endif
   
   // Be neat and restore the signals. At least we are friendly!
   for (register unsigned int i = NSIG; i--;) {
      (void)signal(i, SIG_DFL);
   }
}


/* addHandler - Add a handler to the handlers list
 * Original 09/07/2002 simonb
 */
bool Signals::addHandler(const handlerPtr_type handler,
			 const mask_type mask, void* foo)
{
#ifdef KINE_DEBUG_EXTENDED
   debug("Signals::addHandler() - Adding a handler @ ptr " + 
	 String::convert((int)handler));
#endif
   
   // Make sure the given data is okay
   if ((handler == 0) || (mask == 0)) {
      return false;
   }

   // Okay, add it then
   handlers.push_front(handlerInfo(handler, mask, foo));

#ifdef KINE_DEBUG_PSYCHO
   debug("Signals::addHandler() - Handlers list now has " +
	 String::convert(handlers.size()) + " entries");
#endif
   
   return true;
}
