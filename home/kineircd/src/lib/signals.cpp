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

#include "kineircd/kineircdconf.h"

#include <csignal>

#include "kineircd/signals.h"
#include "kineircd/debug.h"
#include "kineircd/exit.h"

using namespace Kine;


// This is here so we can remember where we are!
Signals *siggies;


#ifdef SYS_SIGLIST_DECLARED
# define sigNames sys_siglist
#else
/* Reference - man page signal(7) on Linux (Posix/SysV/BSD combine)
 * Note: This is a guessing table since we don't have a real signal
 *       list declared for us
 */
static char const *sigNames[] = {
   "SIGZERO",   //  0 - Apparently never used
   "SIGHUP",    //  1 - Hang up detected on controlling terminal
   "SIGINT",    //  2 - Interrupt from keyboard (^C)
   "SIGQUIT",   //  3 - Quit from keyboard (^D)
   "SIGILL",    //  4 - Illegal intruction called
   "SIGTRAP",   //  5 - Trace/Breakpoint trap
   "SIGABRT",   //  6 - Abort signal from abort()
   "SIGEMT",    //  7 -
   "SIGFPE",    //  8 - Floating point exception
   "SIGKILL",   //  9 - Kill signal (cannot be caught)
   "SIGBUS",    // 10 - BUS error (bad memory access, usually causes core dump)
   "SIGSEGV",   // 11 - Invalid memory reference (segment violation)
   "SIGSYS",    // 12 - Bad argument to routine
   "SIGPIPE",   // 13 - Broken pipe (written to pipe with nobody listening)
   "SIGALRM",   // 14 - Time signal from alarm()
   "SIGTERM",   // 15 - Termination signal
   "SIGURG",    // 16 - Urgent condition on socket (should induce race condition)
   "SIGSTOP",   // 17 - Stop process (pauses process, cannot be caught)
   "SIGTSTP",   // 18 - Stop typed at TTY
   "SIGCONT",   // 19 - Continue if stopped
   "SIGCHLD",   // 20 - Child process stopped or terminated
   "SIGTTIN",   // 21 - TTY input for background process
   "SIGTTOU",   // 22 - TTY output for background process
   "SIGIO",     // 23 - I/O now possible
   "SIGXCPU",   // 24 - CPU time limit exceeded (usually terminates with core dump)
   "SIGXFSZ",   // 25 - File size limit exceeded (usually terminates with core dump)
   "SIGVTALRM", // 26 - Virtual alarm clock
   "SIGPROF",   // 27 - Profiling timer expired
   "SIGWINCH",  // 28 - Window resize signal
# ifdef SIGLOST
   "SIGLOST",   // 29 - Signal Lost (apparently obsolete)
# else
   "SIGPWR",    // 29 - Power failure
# endif
   "SIGUSR1",   // 30 - User-defined signal 1
   "SIGUSR2"    // 31 - User-defined signal 2
};
#endif
   

/* sigHandler - Handle signals
 * Original 11/08/2001 simonb
 * Note: If this is to compile on other systems we may need the header
 * 	 RETSIGTYPE sigHandler(int sig, int code, struct sigcontext *context) 
 *       right?
 */
RETSIGTYPE sigHandler(int sig)
{
#ifdef DEBUG
   debug("sigHandler() -> [" + sig + String("] ") + sigNames[sig]);
#endif
   
   switch (sig) {
      // Rehash
    case SIGHUP:
//      Daemon::rehash(0, 0);
      abort();//temporary
      break;
      
      // Die violently
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
      exit(Exit::ERR_UGLY_SIGNAL); // This could be a little nicer..
      break;

      // Die gracefully
    case SIGINT:
    case SIGQUIT:
/*    case SIGPIPE:  */ // bsd?
    case SIGTERM:
    case SIGABRT:
#ifdef SIGXCPU
    case SIGXCPU: // should this be in the die violently section?
#endif
#ifdef SIGXFSZ
    case SIGXFSZ: // should this be in the die violently section?
#endif
//      Daemon::shutdown(sigNames[sig]);
      abort();// temporary
      break;
      
      // Everything else we ignore.
//    default:
      // ?!
   }
   
   // Reset the signal for future handling. Some os's do not need this??
   signal(sig, sigHandler);
}


/* Signals - Constructor for the signal handler class
 * Original 07/07/2002 simonb
 */
Signals::Signals(void)
{
#ifdef DEBUG_PSYCHO
   debug("Signals::Signals() - Setting up signal handlers...");
#endif
   
   // Remember who we are..
   siggies = this;
   
   // We are very multi-talented and try to handle every signal we can..
   for (register unsigned int i = NSIG; i--;) {
      signal(i, sigHandler);
   }
}


/* ~Signals - Destructor for the signal handler class
 * Original 07/07/2002 simonb
 */
Signals::~Signals(void)
{
#ifdef DEBUG_PSYCHO
   debug("Signals::~Signals() - Defaulting signal handlers...");
#endif
   
   // Be neat and restore the signals. At least we are friendly!
   for (register unsigned int i = NSIG; i--;) {
      signal(i, SIG_DFL);
   }
}
