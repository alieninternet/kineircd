[+ AutoGen5 template cpp +]
/* $Id$
 * 
 * Copyright (c) 2000,2002,2003 Simon Butcher <pickle@alien.net.au>
 * Copyright (c) 2000,2002,2003 Alien Internet Services
 * Copyright (c) 2002,2003 KineIRCd Development Team
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
 *
 *
[+(dne " * ")+]
 */

#ifdef HAVE_CONFIG_H
# include "autoconf.h"
#endif

#ifdef HAVE_SYSLOG_H
# include <syslog.h>
#endif
 
#include "mod_syslog/syslog.h"

using namespace Kine::mod_syslog;


/* Syslog - Syslog logger component constructor
 * Original 16/02/2000 simonb
 * 04/04/2002 simonb - Added PID boolean option
 * 04/04/2002 simonb - Added debugging info
 */
Syslog::Syslog(mod_syslog::Config& c)
  : Kine::Logger(c.getLogMask()),
    config(c)
{
   /* The options we will use with syslog. We use NDELAY so that the socket
    * is opened up immediately (in case we run out of file descriptors and
    * cannot log it!), and we use NOWAIT so that logging to syslog doesn't
    * slow the daemon down.
    */
   int options = LOG_NDELAY | LOG_NOWAIT;

   // LOG_CONS option?
   if (config.getConsoleOutput()) {
      options |= LOG_CONS;
   }
   
   // LOG_PID option?
   if (config.getShowPID()) {
      options |= LOG_PID;
   }
 
   // Open a connection to the system logger
   openlog(config.getIdentity().c_str(), options, LOG_DAEMON);
}


/* ~Syslog - Destructor (close the syslog connection)
 * Original 16/02/2000 simonb
 */
Syslog::~Syslog(void)
{
   closelog();
}


/* logLine - Log a line to the syslog daemon
 * Original 16/02/2000 simonb
 * 04/04/2002 simonb - Modified for use with new String class
 * 04/04/2002 simonb - Added debugging info
 */
void Syslog::logLine(const std::string& str,
		     const Kine::Logger::Mask::type mask)
{
   // Determine what the syslog-relative priority should be
   int priority = 0;
   switch (mask) {[+FOR logger_masks+][+COMMENT 
                                                It would be nice if this
                                                went through the list a few
						times, so it can generate an
						output which has less jumps.
						i.e. clustering the syslog
						priorities together in a 
						smart way.. but I don't care
						for now.. 
				       +]
    case Logger::Mask::[+name+]:
      priority = [+syslogpri+];
      break;[+ENDFOR logger_masks+]
   }

   // Log the line
   syslog(priority, "%s", str.c_str());
}
