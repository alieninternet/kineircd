[+ AutoGen5 template h +]
/* logger.h
 * Logging functions base class
 * 
 * DO NOT EDIT THIS FILE! It is automatically generated from logger.h.tpl.
 * Edit logger.def/logger.h.tpl instead.
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

#ifndef _INCLUDE_KINEIRCD_LOGGER_H_
# define _INCLUDE_KINEIRCD_LOGGER_H_

# include "kineircd/kineircdconf.h"

# ifdef HAVE_SYSLOG_H
#  include <syslog.h>
# endif

# include "kineircd/str.h"


// The top Logger class the actual loggers are derived from
class Logger {
 public:
   // Bitmask list for a mask (type of log message)
   enum mask_type {
      MASK_NONE = 0x00000000,[+ (define bitvalue 1) +][+ FOR logger_masks +]
      MASK_[+name+] = [+
	 (define newbitvalue (* bitvalue 2))
	 (define bitvalue newbitvalue)
         (sprintf "0x%08X" (/ newbitvalue 2))
       +], // +[+char+][+ ENDFOR logger_masks +]
      MASK_ALL = 0xFFFFFFFF
   };
   
   // Logger types (each logger class must set itself to one of these)
   enum type_type {
      TYPE_OTHER,				// Other; eg. module extension
# ifdef HAVE_SYSLOG_H
      TYPE_SYSLOG,				// Syslog interface
# endif
      TYPE_FILE,				// Generic file based log
      TYPE_SNOTICE				// Server Notice (via daemon)
   };

   static const unsigned char maskTableSize = [+ 
      (count "logger_masks")
    +];

   // The table holding our mask to name to character translations
   struct maskTable_type {
      const char *name;				// Name of the mask
      const char chr;				// Character for the mask
      const mask_type mask;			// The mask itself
   } static const maskTable[maskTableSize];

 private:
   const type_type logType;				// Type of log this is
   const mask_type logMask;				// OK log message types
 
   // Log a string of text
   virtual void logLine(const String &, const mask_type = MASK_HOUSEKEEPING)
     {};

 public:
   // Constructor
   Logger(type_type t, mask_type m)
     : logType(t),
       logMask(m)
     {};
   
   // Destructor
   virtual ~Logger(void) 
     {};
   
   // Grab logging type
   const type_type getType(void) const
     { return logType; };

   // Grab logging mask
   const mask_type getMask(void) const
     { return logMask; };

   // Is the log ok?
   virtual bool ok(void) const
     { return true; };

   // Pass a line of text to the logging function if it matches our mask
   void log(const String &line, const mask_type mask = MASK_HOUSEKEEPING)
     {
        if (logMask & mask) {
	   logLine(line, mask);
	}
     };
};

#endif

