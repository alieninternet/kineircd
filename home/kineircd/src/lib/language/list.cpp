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

      // Check the first letter of the tag...
      if (tag[0] == '.') {
	 // Strip away the first char
	 tag = tag.substr(1);
	 
#ifdef KINE_DEBUG_PSYCHO
	 debug("LanguageList::loadFile() - Line " + String::convert(lineNum) +
	       ": Got control tag '" + tag + '\'');
#endif
	 
	 // Do something with the control tag? Huh? Huh?? :(

      } else {
	 LanguageData::tagID_type tagID;
	 
#ifdef KINE_DEBUG_PSYCHO
	 debug("LanguageList::loadFile() - Line " + String::convert(lineNum) +
	       ": Got tag '" + tag + '\'');
#endif

	 // Look for this tag in the dictionary
	 tagDictionary_type::const_iterator it = tagDictionary.find(tag);
	 
	 // Did we find it?
	 if (it == tagDictionary.end()) {
	    // We did not find it - make a new tag..
	    tagID = ++highestTagID;
	    
#ifdef KINE_DEBUG_PSYCHO
	    debug("LanguageList::loadFile() - " + tag + ": TUID #" +
		  String::convert(tagID) + " (new)");
#endif

	    // .. and insert its TUID into the tag dictionary
	    tagDictionary.
	      insert(tagDictionary_type::value_type(tag, tagID));
	 } else {
#ifdef KINE_DEBUG_PSYCHO
	    debug("LanguageList::loadFile() - " + tag + ": TUID #" +
		  String::convert(tagID) + " (found)");
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
