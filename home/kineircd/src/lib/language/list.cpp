/* $Id$
 * Handle the languages list which resides in the daemon's configuration
 *
 * Copyright (c) 2002,2003 Simon Butcher <pickle@alien.net.au>
 * Copyright (c) 2002,2003 KineIRCd Development Team
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
      
      // Process the data: substitute anything we know now
      for (std::string::size_type i = 0; i < data.length(); i++) {
	 if (data[i] == '%') {
	    /* Check the next character. If it's another percentage sign, let
	     * it get copied, otherwise convert it over to the super secret
	     * hush hush magical mystical char... which is a null.. ;)
	     */
	    if (data[i + 1] == '%') {
	       i++;
	    } else {
	       line += '\0';
	       continue;
	    }
	 }
	 
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
	       long value = strtol(data.substr(i, 3).c_str(), NULL, 8);
	       
	       /* Skip over the next numbers.. (the last will be skipped
		* automatically, so we only need to skip two of them)
		*/
	       i += 2;
	       
	       // Is this a naughty char?
	       if ((value > 0) && (value < 256)) {
		  // Replace our 'current' character for further analysis..
		  data[i] = char(value);
	       } else {
		  // Force this character to be replaced
		  data[i] = 0;
	       }
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
		  line += '\040';
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
	    
	 // Make sure this character isn't going to cause problems..
	 if ((data[i] == 0) ||
	     (data[i] == '\b') ||
	     (data[i] == '\n') ||
	     (data[i] == '\v') ||
	     (data[i] == '\f') ||
	     (data[i] == '\r') ||
	     ((data[i] >= '\021') && (data[i] <= '\024'))) {
	    /* Okay, it's a naughty char, we must replace it. Following
	     * Unicode specifications, here we replace the bad character
	     * with the "replacement" character.
	     * 
	     * Since all these bad chars are in the control char region,
	     * I have considered up-converting them to their symbols, so
	     * they'd appear like a dumb terminal in debug mode. It would
	     * be fairly easy, since all you need to do is add 0x2400 and
	     * convert it to UTF-8. I reconsidered this, since the
	     * characters aren't supposed to be there anyway, so a single,
	     * boring, replacement character does the job.
	     * 
	     * Then again, in the time it's taken me to write out my
	     * reasoning, I probably could have written, and tested an
	     * upconverter.. Hmm.. :)
	     */
	    line += Languages::replacementCharacterGlyph;
	    continue;
	 } 
	    
	 // If we got here, just copy the char over
	 line += data[i];
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
	       " tag: '" + tag + "' - Control tag");
#endif
	 
	 // Do something with the control tag? Huh? Huh?? :(

      } else {
	 Languages::tagID_type tagID;
	 
	 // Look for this tag in the dictionary
	 tagDictionary_type::const_iterator it = tagDictionary.find(tag);
	 
	 // Did we find it?
	 if (it == tagDictionary.end()) {
	    // We did not find it - make a new tag..
	    tagID = ++highestTagID;
	    
#ifdef KINE_DEBUG_PSYCHO
	 debug("LanguageList::loadFile() - Line " + String::convert(lineNum) +
	       " tag: '" + tag + "' - TID #" + String::convert(tagID) +
	       " (new)");
#endif

	    // .. and insert its TUID into the tag dictionary
	    tagDictionary.
	      insert(tagDictionary_type::value_type(tag, tagID));
	 } else {
	    // Use the tag ID we found
	    tagID = (*it).second;
	    
#ifdef KINE_DEBUG_PSYCHO
	 debug("LanguageList::loadFile() - Line " + String::convert(lineNum) +
	       " tag: '" + tag + "' - TID #" + String::convert(tagID) +
	       " (found)");
#endif
	 }

	 // Remember the tag's data and TUID.. here... :)
	 
      }
   }
   
   // Presume everything went okay!
   return true;
}
