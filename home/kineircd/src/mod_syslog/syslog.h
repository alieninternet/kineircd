/* loggersyslog.h
 * Logging functions via the "syslog" interface
 * 
 * Copyright (c) 2000,2002 Alien Internet Services
 *
 * This file is a part of AustHex.
 * 
 * AustHex is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * AustHex is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with AustHex; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __LOGGERSYSLOG_H__
# define __LOGGERSYSLOG_H__

# include "autoconf.h"

// Only continue if we actually have syslog support on this system
# ifdef HAVE_SYSLOG_H

#  include <syslog.h>

#  include "logger.h"


// The syslog logging class
class LoggerSyslog : public Logger {
 public:
   LoggerSyslog(const char *, const bool);		// Constructor
   ~LoggerSyslog(void);					// Destructor
   
   // Is the log ok?
   bool ok(void) const
     { return true; };
   
   void log(const String &, 
	    const mask_type = MASK_HOUSEKEEPING);	// Log a string of text
};

# endif
#endif
