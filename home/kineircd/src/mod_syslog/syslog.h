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
 */

#ifndef _SRC_LIB_LOGGER_SYSLOG_H_
# define _SRC_LIB_LOGGER_SYSLOG_H_ 1

# ifdef HAVE_CONFIG_H
#  include "autoconf.h"
# endif

// Only continue if we actually have syslog support on this system
# ifndef HAVE_SYSLOG_H
#  error "Cannot compile syslog logging module without syslog support"
# endif

extern "C" {
# include <syslog.h>
}
   
# include "kineircd/logger.h"


namespace Kine {
   namespace mod_syslog {
      // The syslog logging class
      class Syslog : public Kine::Logger {
       private:
	 // Log a string of text
	 void logLine(const std::string& str, const Kine::Logger::Mask::type);
	 
       public:
	 // Constructor
	 Syslog(Kine::Logger::Mask::type, const char *, const bool);
	 
	 // Destructor
	 ~Syslog(void)
	   { closelog(); };
	 
	 // Is the log ok? We presume so :)
	 bool ok(void) const
	   { return true; };
      };
   }; // namespace mod_syslog
}; // namespace Kine

#endif // _SRC_LIB_LOGGER_SYSLOG_H_
   
