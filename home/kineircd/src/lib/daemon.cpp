/* $Id$
 * The main class (IRC daemon) constructors and destructors
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

#include <unistd.h>
#include <cstdio>
#include <cerrno>
#include <cstring>
#include <ctime>

extern "C" {
#ifdef KINE_HAVE_OPENSSL
# include <openssl/ssl.h>
#endif
}

#include "kineircd/daemon.h"
#include "kineircd/debug.h"
#include "kineircd/exit.h"
#include "kineircd/version.h"

using namespace Kine;


/* Daemon - Init the server
 * Original 11/08/01 simonb
 */
Daemon::Daemon(Config &conf, Signals &sigs)
  : stage(STAGE_INIT),
    config(conf),
    signals(sigs)
{
   // We are ready to go, go into normal running stage
   stage = STAGE_NORMAL;
}


/* ~Daemon - Deinit the server
 * Original 11/08/01 simonb
 */
Daemon::~Daemon(void)
{
#ifdef DEBUG
   debug("Shutting down server");
#endif

}


/* run - The main loop
 * Original 11/08/01 simonb
 * Note: Unfortuantely not a very nice looking routine..
 */
int Kine::Daemon::run(void)
{
   /* Make sure the init was all happy, else there isn't much point us
    * going beyond this point really
    */
   if (stage != STAGE_NORMAL) {
      return Exit::ERR_DAEMON_INIT;
   }
   
#ifdef DEBUG
   debug("Entering main loop");
#endif

   for (;;) {
      // Do stuff here with the poller :)
      sleep(10); // temporary
      
      // Check if we are in shutdown mode
      if (stage == STAGE_SHUTDOWN) {
	 // check queues here..
	 
	 // If we get here the queues must be all dry
#ifdef DEBUG
	 debug("Output queues exhausted - Breaking main loop");
#endif
	 break;
      }
   }
   
   return Exit::NO_ERROR;
}

