[+ AutoGen5 template cpp +]
/* $Id$
 * Logging functions via the "syslog" interface
 * 
 * Copyright (c) 2000,2002 Alien Internet Services
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

#include "mod_syslog/syslog.h"

using namespace Kine::mod_syslog;


/* Syslog - Syslog logger component constructor
 * Original 16/02/2000 simonb
 * 04/04/2002 simonb - Added PID boolean option
 * 04/04/2002 simonb - Added debugging info
 */
Syslog::Syslog(Kine::Logger::Mask::type mask, const char *processName, 
	       const bool showPid)
 : Logger(mask)
{
   int option = LOG_NDELAY;

   openlog(processName, (showPid ? (option | LOG_PID) : option), LOG_DAEMON);
}


/* logLine - Log a line to the syslog daemon
 * Original 16/02/2000 simonb
 * 04/04/2002 simonb - Modified for use with new String class
 * 04/04/2002 simonb - Added debugging info
 */
void Syslog::logLine(const std::string& str,
		     const Kine::Logger::Mask::type mask)
{
   // Determine what the syslog-relative priority should be (this is shit)
   int priority;
   switch (mask) {[+ FOR logger_masks +]
    case Logger::Mask::[+name+]:
      priority = [+syslogpri+];
      break;[+ ENDFOR logger_masks +]
   }

   syslog(priority, "%s", str.c_str());
}
