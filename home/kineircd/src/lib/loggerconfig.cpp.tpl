[+ AutoGen5 template cpp +]
/* $Id$
 * Configuration helper stuff for logging modules which may need it
 * 
 * Copyright (c) 2002 Simon Butcher <pickle@alien.net.au>
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

#ifdef HAVE_CONFIG_H
# include "autoconf.h"
#endif
#include "kineircd/kineircdconf.h"

#include <kineircd/loggerconfig.h>

using namespace Kine;


struct {
   const char* const name;
   const Logger::Mask::lazy_type mask;
} static const logMasks[[+(+ 2 (count "logger_masks"))+]] = {
     { "EVERYTHING",			Logger::Mask::Everything },
     { "NOTHING",			Logger::Mask::Nothing },[+FOR logger_masks ","+]
     [+(sprintf "{ %-33sLogger::Mask::%s }"
	  (sprintf "\"%s\","
	     (string-upcase (get "name")))
          (get "name"))
      +][+ENDFOR logger_masks+]
};
 

LIBAISUTIL_CONFIG_VARIABLE_HANDLER(LoggerConfig::varHandleLogMask)
{
   // If the parameters list is empty, log everything..
   if (values.empty()) {
      dataClass.*((Logger::Mask::lazy_type ConfigData::*)dataVariable) = 
	Logger::Mask::Everything;
      return true;
   }

   Logger::Mask::lazy_type logMask = Logger::Mask::Nothing;
   std::string maskWord;
   bool toggleOn;
   
   // Run through the parameters..
   for (unsigned int i = 0; i < values.size(); i++) {
      // Check the prefix char, and work out how we need to copy the word over
      switch (values[i][0]) {
       case '-':
	 toggleOn = false;
	 maskWord = values[i].toUpper().substr(1);
	 break;
	 
       case '+':
	 toggleOn = true;	
	 maskWord = values[i].toUpper().substr(1);
	 break;
	 
       default:
	 toggleOn = true;
	 maskWord = values[i].toUpper();
      }
      
      // Look for the word in the list..
      for (unsigned int j = 0; j < [+(+ 2 (count "logger_masks"))+]; j++) {
	 // Does this match?
	 if (maskWord == logMasks[j].name) {
	    // Okay, do whatever was asked with this mask..
	    if (toggleOn) {
	       logMask |= logMasks[j].mask;
	    } else {
	       logMask &= ~logMasks[j].mask;
	    }
	    
	    // Break out of this loop, since we got a match..
	    break;
	 }
      }
   }

   // Assign the new value
   dataClass.*((Logger::Mask::lazy_type ConfigData::*)dataVariable) =
     logMask;
   
   // All done - smile :)
   return true;
}
