/* $Id$
 * 
 * Copyright (c) 2001,2002,2003 Simon Butcher <pickle@alien.net.au>
 * Copyright (c) 2001,2002,2003 KineIRCd Development Team
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

/* Undefine the pain in the bum define from the autoconf header so that
 * autogen's autoopts works happily -- without this, it becomes a case of
 * autogrrr...
 */
#undef VERSION

#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#ifdef HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif
#include <ltdl.h>
#include <kineircd/languages.h>
#include <kineircd/config.h>
#include <kineircd/daemon.h>
#include <kineircd/mynetwork.h>
#include <kineircd/myserver.h>

#include "ircd/bits.h"
#include "ircd/exit.h"
#include "ircd/options.h"

using namespace Kine;


/* main
 * Original 11/08/2001 simonb
 */
int main(int argc, char **argv)
{
   // Parse the command line options (we don't care about its return value)
   (void)optionProcess(&kineircdOptions, argc, argv);

   // Are we being quiet? If not, we can print our wonderful startup info
   if (!ISSEL_OPT(QUIET)) {
      ircd::printVersion(COUNT_OPT(VERSION));
   }
   
   // If the version level is not 0, we will also need to exit
   if (COUNT_OPT(VERSION) > 0) {
      exit(ircd::Exit::NO_ERROR);
   }

   // Initialise the libtool dlopen mechanism before we (might) load modules
   if (lt_dlinit() != 0) {
      // Complain if we can
      if (!ISSEL_OPT(QUIET)) {
	 std::cout << "Unable to initialise libltdl: " << lt_dlerror() <<
	   std::endl;
      }
      
      exit(ircd::Exit::ERR_DAEMON_INIT);
   }
   
   // Create the all important languages interface class prior to configuration
   Languages::initInstance();

   // Create the registry, where all the entities live
//   Registry::initInstance();
   
   // Attempt to load and parser the configuration file
   if (!config().configure(OPT_ARG(CONFIG_FILE))) {
      // Complain if we can
      if (!ISSEL_OPT(QUIET)) {
	 std::cout << "Could not load the configuration file (" << 
	   config().getConfigFile() << ')' << std::endl;
      }
      
      exit(ircd::Exit::ERR_CONFIG_LOAD);
   }

   // Now that the config file has been loaded happily, init the daemon
   Daemon::initInstance();

   // Check on the daemon, make sure it is happy
   if (daemon().getRunlevel() != Daemon::RUNLEVEL_NORMAL) {
      // Complain if we can
      if (!ISSEL_OPT(QUIET)) {
	 std::cout << "Sorry, the daemon was unable to initialise" <<
	   std::endl;
      }
      
      exit(ircd::Exit::ERR_DAEMON_INIT);
   }

   // Initialise two important entities - our network and server entities
   MyNetwork::initInstance();
   MyServer::initInstance();
   
   // Where to put out PID when we get it (if we get it)
   pid_t pid = 0;

#ifndef KINE_DEBUG
   // Are we allowed to fork?
   if (!ISSEL_OPT(FOREGROUND)) {
      // Fork, and check that the fork worked...
      if ((pid = fork()) < 0) {
	 // Complain if we can
	 if (!ISSEL_OPT(QUIET)) {
	    // uhh this should be logged..
	    std::cout << "Eek! Could not fork (" << strerror(errno) << ")" <<
	      std::endl;
	 }
	 
	 exit(ircd::Exit::ERR_NO_FORK);
      }
      
      // If we forked and we are the parent, this is where we exit
      if (pid > 0) {
	 exit(ircd::Exit::FORKED);
      }
      
      /* Okay, if we get here we must be the child process. We need to create
       * a new session for ourselves to take control so we have no controlling
       * TTY and can become a real daemon..
       */
      pid = setsid();
   } 
#endif
   
   // If we need to make the pid file, make it..
   if (!config().getOptionsPidFile().empty()) {
      // If we are not forking, we need to grab our PID.
      if (ISSEL_OPT(FOREGROUND)) {
	 pid = getpid();
      }
      
      // Open the PID file
      std::ofstream pidFile(config().getOptionsPidFile().c_str(),
			    std::ios::out | std::ios::trunc);
      
      // Is the file okay?
      if (pidFile.good()) {
	 // Dump the PID number into the file
	 pidFile << (int)pid << '\n';
      } else if (!ISSEL_OPT(QUIET)) {
	 // Complain - this should really be via the logger..
	 std::cout << "Could not write PID file" << std::endl;
      }
      
      pidFile.close();
   }

#ifndef KINE_DEBUG
   // Close some file descriptors we will not be using - no more console i/o
   close(0); // stdin
   close(1); // stdout
   close(2); // stderr
#endif
   
   // Fire up the main loop
   if (!daemon().run()) {
      // If this returned a false value, exit in a cranky mood
      exit(ircd::Exit::ERR_UGLY_DEATH);
   }
   
   // Shut down libltdl stuff
   (void)lt_dlexit();
   
   // Exit normally
   exit(ircd::Exit::NO_ERROR);
}
