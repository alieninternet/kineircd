/* $Id$
 * Handle the languages list which resides in the daemon's configuration
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
 */

#ifdef HAVE_CONFIG_H
# include "autoconf.h"
#endif
#include "kineircd/kineircdconf.h"

#include <iostream>
#include <fstream>
#include <map>
#include <cctype>
#include <cstdlib>
#include <aisutil/string/tokens.h>

#include "kineircd/languagelist.h"
#include "libkineircd/debug.h"

using namespace Kine;
using AISutil::String;
using AISutil::StringTokens;


/* loadFile - Load a language file
 * Original 21/08/2002 simonb
 */
bool LanguageList::loadFile(const std::string& filename, String& errString)
{
   // Open the file
#ifdef KINE_DEBUG_PSYCHO
   debug("LanguageList::loadFile() - Attemping to open '" + filename + '\'');
#endif
   std::ifstream file(filename.c_str());
   
   // Check that we opened the file properly
   if (!file) {
      errString = "Could not open the file";
      return false;
   }
   
#ifdef KINE_DEBUG_PSYCHO
   debug("LanguageList::loadFile() - Beginning read loop");
#endif
   
   // Stuff we need during the read
   String line;
   String tag;
   String data;
   unsigned long lineNum = 0;

   // Loop through the file..
   for (;;) {
      // Read the line
      std::getline(file, line);

      // Make sure the file is still happy..
      if (!file.good() || file.eof()) {
	 break;
      }
      
      // Increase the line counter
      lineNum++;
      
      // Trim the line
      line = line.trim();
      
      // Is the line empty, or perhaps even a comment?
      if (line.empty() || (line[0] == '#')) {
	 // Skip this line!
	 continue;
      }
      
      // Rip out the two sides to the token
      StringTokens st(line);
      tag = st.nextToken('=').trim().toUpper();
      data = st.rest().trim();

      // Clear the line variable.. We will reuse it soon..
      line.clear();
      
      // Process the data: substitute escaped stuff
      for (std::string::size_type i = 0; i < data.length(); i++) {
	 // Escaped char?
	 if (data[i] == '\\') {
	    // Next char
	    i++;
	    
	    // If this is a three-digit number, we can convert it flat..
	    if ((data.length() > (i + 2)) &&
		isdigit(data[i]) &&
		isdigit(data[i + 1]) &&
		isdigit(data[i + 2])) {
	       // Convert the three digits found from octal to a long
	       long escape = strtol(data.substr(i, 3).c_str(), NULL, 8);
	       
	       /* Skip over the next numbers.. (the last will be skipped
		* automatically, so we only need to skip two of them)
		*/
	       i += 2;
	       
	       // Is this a naughty char?
	       if ((escape > 0) &&
		   (escape < 256) &&
		   (escape != '\n') &&
		   (escape != '\r') &&
		   (escape != '\v') &&
		   (escape != '\b') &&
		   ((escape < '\021') || (escape > '\024'))) {
		  line += char(escape);
	       }
	       
	       // Next!!
	       continue;
	    } else {
	       /* Something else, a single character tells us what to do then!
		* If this gets larger, reconsider using 512 byte table..
		*/
	       switch (data[i]) {
		case '\\': // A slash..
		  break; // just skip to the bit where we copy the char flat

		case 'b': // Bold
		  line += '\002';
		  continue;
		  
		case 'c': // Colour start sequence char
		  line += '\003';
		  continue;
		  
		case 'g': // Beep
		  line += '\007';
		  continue;
		  
		case 'r': // Reverse
		  line += '\026';
		  continue;
		  
		case 's': // Space (often used at the end of a line as a hack)
		  line.push_back('\040');
		  continue;
		  
		case 't': // Horizontal tab
		  line += '\011'; // should we just use 8 spaces here??
		  continue;
		  
		case 'u': // Underline
		  line += '\037';
		  continue;
		  
		default:
		  // nfi, skip it..
		  continue;
	       }
	    }
	 }
	    
	 // If we got here, nothing special happened.. just copy the char over
	 line.push_back(data[i]);
      }

#ifdef KINE_DEBUG_PSYCHO
      debug("LanguageList::loadFile() - Line " + String::convert(lineNum) +
	    " data: '" + line + '\'');
#endif
      
      // Check the first letter of the tag...
      if (tag[0] == '.') {
	 // Strip away the first char
	 tag = tag.substr(1);
	 
#ifdef KINE_DEBUG_PSYCHO
	 debug("LanguageList::loadFile() - Line " + String::convert(lineNum) +
	       ": Tag '" + tag + "' - Control tag");
#endif
	 
	 // Do something with the control tag? Huh? Huh?? :(

      } else {
	 LanguageData::tagID_type tagID;
	 
	 // Look for this tag in the dictionary
	 tagDictionary_type::const_iterator it = tagDictionary.find(tag);
	 
	 // Did we find it?
	 if (it == tagDictionary.end()) {
	    // We did not find it - make a new tag..
	    tagID = ++highestTagID;
	    
#ifdef KINE_DEBUG_PSYCHO
	 debug("LanguageList::loadFile() - Line " + String::convert(lineNum) +
	       ": Tag '" + tag + "' - TID #" + String::convert(tagID) +
	       " (new)");
#endif

	    // .. and insert its TUID into the tag dictionary
	    tagDictionary.
	      insert(tagDictionary_type::value_type(tag, tagID));
	 } else {
#ifdef KINE_DEBUG_PSYCHO
	 debug("LanguageList::loadFile() - Line " + String::convert(lineNum) +
	       ": Tag '" + tag + "' - TID #" + String::convert(tagID) +
	       " (found)");
#endif

	    // Use the tag ID we found
	    tagID = (*it).second;
	 }

	 // Remember the tag's data and TUID.. here... :)
	 
      }
   }
   
   // Presume everything went okay!
   return true;
}
