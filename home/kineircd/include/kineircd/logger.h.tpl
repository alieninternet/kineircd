[+ AutoGen5 template h +]
/* $Id$
 * Logging functions base class
 *
 * Copyright (c) 2000,2002 Alien Internet Services
 * Copyright (c) 2000,2002 Simon Butcher <pickle@alien.net.au>
 * Copyright (c) 2002 KineIRCd Development Team 
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
         enum type {[+ FOR logger_masks +]
	    [+ (sprintf "%-25s = 0x%08X%s //!< %s"
	          (get "name")
		  (expt 2 (for-index))
		  (if (last-for?) " " ",")
		  (get "comment"))
             +][+ ENDFOR logger_masks +]
         };
	 
	 // Some handy defines, relating to the masks, but are not masks
	 typedef unsigned int lazy_type;
	 static const lazy_type Nothing       = 0x00000000;
	 static const lazy_type Everything    = 0xFFFFFFFF;
      };
   
      static const unsigned char maskTableSize = [+(count "logger_masks")+];

      // The table holding our mask to name to character translations
      struct MaskMapper {
	 const Mask::type mask;			//!< The mask
	 const char character;			//!< Character for the mask
	 const char* const name;		//!< Name of the mask
	 const char* const nameUpper;		//!< Name as above, uppercased
      } static const maskTable[maskTableSize];

      // Handy functions for grabbing data in the table above
      static const MaskMapper& getMaskMapper(const Mask::type m);
      static const MaskMapper& getMaskMapper(const char c);
      static const MaskMapper& getMaskMapper(const char* const n);

    protected:
      const Mask::lazy_type logMask;			//!< OK log message types
      
    private:
      // Log a string of text
      virtual void logLine(const std::string& str,
      	                   const Mask::type mask) = 0;
      
    public:
      //! Constructor
      Logger(const Mask::lazy_type mask)
	: logMask(mask)
	{};

      //! Destructor
      virtual ~Logger(void) 
	{};
   
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

