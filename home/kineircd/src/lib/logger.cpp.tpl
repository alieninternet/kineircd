[+ AutoGen5 template cpp +]
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

#ifdef HAVE_CONFIG_H
# include "autoconf.h"
#endif
#include "kineircd/kineircdconf.h"

#include <cstring>
 
#include "kineircd/logger.h"

using namespace Kine;


// Our list of masks, and how to translate them around nicely
const Logger::MaskMapper Logger::maskTable[Logger::maskTableSize] = {[+FOR logger_masks ","+]
     {
        Logger::Mask::[+name+],
	  '[+char+]',
	  "[+name+]",
	  "[+(string-upcase (get "name"))+]"
     }[+ENDFOR logger_masks+]
};


/* getMaskMapper - Return a mask entry from the above table, using various means
 * Original 01/11/2002 simonb
 */
const Logger::MaskMapper& Logger::getMaskMapper(const Logger::Mask::type m)
{
   // Run through the table to find what we are looking for
   for (unsigned int i = maskTableSize; i--;) {
      // Is this the one?
      if (maskTable[i].mask == m) {
	 return maskTable[i];
      }
   }

#ifdef DEBUG_ASSERT
   assert(false);
#endif
   
   // Argh!!
   return maskTable[0];
}

const Logger::MaskMapper& Logger::getMaskMapper(const char c)
{
   // Run through the table to find what we are looking for
   for (unsigned int i = Logger::maskTableSize; i--;) {
      // Is this the one?
      if (maskTable[i].character == c) {
	 return maskTable[i];
      }
   }

#ifdef DEBUG_ASSERT
   assert(false);
#endif

   // Argh!!
   return maskTable[0];
}

const Logger::MaskMapper& Logger::getMaskMapper(const char* const n)
{
   // Run through the table to find what we are looking for
   for (unsigned int i = maskTableSize; i--;) {
      // Is this the one?
      if (strcasecmp(maskTable[i].name, n) == 0) {
	 return maskTable[i];
      }
   }

#ifdef DEBUG_ASSERT
   assert(false);
#endif
   
   // Argh!!
   return maskTable[0];
}


/* varHandleLogMask - Convert logging names into a mask value
 * Original 01/11/2002 simonb
 */
LIBAISUTIL_CONFIG_VARIABLE_HANDLER(Logger::Config::varHandleLogMask)
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
      
      // Is this 'nothing' ?
      if ((maskWord == "NOTHING") ||
          (maskWord == "NONE")) {
      	 if (toggleOn) {
	    logMask |= Mask::Nothing;
	 } else {
	    logMask &= ~Mask::Nothing;
	 }
         continue;
      }
      
      // Is this 'everything' ?
      if ((maskWord == "EVERYTHING") ||
          (maskWord == "ALL")) {
      	 if (toggleOn) {
	    logMask |= Mask::Everything;
	 } else {
	    logMask &= ~Mask::Everything;
	 }
         continue;
      }
      
      // Look for the word in the list..
      for (unsigned int j = 0; j < [+(count "logger_masks")+]; j++) {
	 // Does this match?
	 if (maskWord == maskTable[j].nameUpper) {
	    // Okay, do whatever was asked with this mask..
	    if (toggleOn) {
	       logMask |= maskTable[j].mask;
	    } else {
	       logMask &= ~maskTable[j].mask;
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
