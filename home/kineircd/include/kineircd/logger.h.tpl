[+ AutoGen5 template h +]
/* $Id$
 * Logging functions base class
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

#ifndef _INCLUDE_KINEIRCD_LOGGER_H_
# define _INCLUDE_KINEIRCD_LOGGER_H_ 1

# include <string>

namespace Kine {
   // The top Logger class the actual loggers are derived from
   class Logger {
    public:
      // Bitmask list for a mask (type of log message)
      struct Mask { // <=- should be namespace!
         // The masks
         enum type {[+ (define bitvalue 1) +][+ FOR logger_masks "," +]
	    [+
	       (define newbitvalue (* bitvalue 2))
	       (define bitvalue newbitvalue)
               (sprintf "%-25s = 0x%08X" (get "name") (/ newbitvalue 2))
             +][+ ENDFOR logger_masks +]
         };
	 
	 // Some handy defines, relating to the masks, but are not masks
	 typedef unsigned int lazy_type;
	 static const lazy_type Nothing       = 0x00000000;
	 static const lazy_type Everything    = 0xFFFFFFFF;
      };
   
      static const unsigned char maskTableSize = [+ 
         (count "logger_masks")
       +];

      // The table holding our mask to name to character translations
      struct maskTable_type {
	 const char *name;			// Name of the mask
	 const char chr;			// Character for the mask
	 const Mask::type mask;			// The mask itself
      } static const maskTable[maskTableSize];
      
    private:
      const Mask::type logMask;				// OK log message types
      
      // Log a string of text
      virtual void logLine(const std::string& str,
      	                   const Mask::type mask) = 0;
      
    public:
      // Constructor
      Logger(const Mask::type m)
	: logMask(m)
	{};

      // Destructor
      virtual ~Logger(void) 
	{};
   
      // Grab logging mask
      const Mask::type getMask(void) const
	{ return logMask; };
      
      // Is the log ok?
      virtual bool ok(void) const = 0;

      // Pass a line of text to the logging function if it matches our mask
      void log(const std::string &str, const Mask::type mask)
	{
	   if (logMask & mask) {
	      logLine(str, mask);
	   }
	};
   }; // class Logger
}; // namespace Kine
   
#endif // _INCLUDE_KINEIRCD_LOGGER_H_

