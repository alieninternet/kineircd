/* logger.h
 * Logging functions base class
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

#ifndef __LOGGER_H__
# define __LOGGER_H__

# include "autoconf.h"

# ifdef HAVE_SYSLOG_H
#  include <syslog.h>
# endif

# include "str.h"


// The top Logger class the actual loggers are derived from
class Logger {
 public:
   enum priority_type {
# ifdef HAVE_SYSLOG_H
      // If we have syslog, match them (dodgey cheat I am :)
      PRI_EMERGANCY = LOG_EMERG,
      PRI_ALERT = LOG_ALERT,
      PRI_CRITICAL = LOG_CRIT,
      PRI_ERROR = LOG_ERR,
      PRI_WARNING = LOG_WARNING,
      PRI_NOTICE = LOG_NOTICE,
      PRI_INFO = LOG_INFO,
      PRI_DEBUG = LOG_DEBUG
# else
      // .. Otherwise we just list them and let the compiler enumerate them
      PRI_EMERGANCY,
      PRI_ALERT,
      PRI_CRITICAL,
      PRI_ERROR,
      PRI_WARNING,
      PRI_NOTICE,
      PRI_INFO,
      PRI_DEBUG
# endif
   };

   enum logtype_type {
      TYPE_OTHER,				// Other; eg. module extension
# ifdef HAVE_SYSLOG_H
      TYPE_SYSLOG,				// Syslog interface
# endif
      TYPE_FILE,				// Generic file based log
      TYPE_SNOTICE				// Server Notice (via daemon)
   } const type;

   
   // Constructor
   Logger(logtype_type t)
     : type(t)
     {};
   
   // Destructor
   virtual ~Logger(void) 
     {};
   
   // Grab logging type
   const logtype_type getType(void) const
     { return type; };
   
   // Is the log ok?
   virtual bool ok(void) const
     { return true; };

   // Log a string of text
   virtual void log(const String &, const priority_type = PRI_NOTICE)
     {};
};

#endif
