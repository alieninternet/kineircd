/* language.cpp
 * Stuff for manipulating language data on the server
 * 
 * Copyright (c) 2001,2002 AustHex Development Team
 * (See DEV-TEAM file for details)
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

#include "austhex/austhexconf.h"

#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <cstring>
#include <iostream>
#include <fstream>
#include <cerrno>

#include "austhex/lang.h"
#include "austhex/debug.h"

namespace Lang {
   // The map with the languages in it..
   Lang::languages_map_t languages;
   
   // Default language, start off with it being none...
   LangData *defaultLanguage = 0;
   
   // Language tag list for ISUPPORT
   String ISUPPORTcodes;
}

/* Do not change these lines unless you know how they are used/referenced or
 * integrate with the IRC protocol etc. Often white-space before/after a
 * string of text is there for a very specific reason.
 */

namespace Lang {
   // Greeting lines sent when a user connects
   char const *L_PINGPONG_NOTICE =
     ":*** If you are having problems connecting due to ping timeouts, "
     "type /raw PONG %s or /quote PONG %s now. If you are still having "
     "trouble connecting, please e-mail help@austnet.org with the name "
     "and version of the client you are using, and the server you tried "
     "to connect to (%s).";


   // Misc/Disorganised
   char const *L_REQUESTED_SHUTDOWN =
     " requested shutdown";
   char const *L_BYE_BYE_USER =
     "Exit: %s";
   char const *L_ERROR_CLOSING_LINK =
     "ERROR :Closing Link: %s\r\n";
   char const *L_ERROR_CLOSING_LINK_DEFAULT_REASON =
     "Unknown error";

   char const *L_DEFAULT_QUIT_MESSAGE =
     "Exit: Austnet Chat Network";
};


/* loadLanguages - Seek and load language files in the given directory
 * Original 28/10/01 simonb
 */
bool Lang::loadLanguages(String const &directory,
			 String const &defaultCode)
{
#ifdef DEBUG
   debug("Loading language files...");
#endif
   
   struct dirent *dirEntry;
   DIR *dir = opendir(directory.c_str());

   // Make sure we opened that directory
   if (!dir) {
#ifdef DEBUG
      debug("Error opening language file directory " + directory + ": " +
	    strerror(errno));
#endif
      return false;
   }
   
   // Stuff we need
#ifdef STL_HAS_HASH
   std::hash_map <String, String *> langData;
#else
   std::map <String, String *> langData;
#endif
   String line, tag, data;
   unsigned int lineNum = 0;
   
   // Run through the directory list and look for files we want
   while ((dirEntry = readdir(dir)) != NULL) {
      // Check the name...
      if (!strncasecmp(dirEntry->d_name, LANG_FILE_PREFIX,
		       LANG_FILE_PREFIX_LEN)) {
	 // Open the file
	 String fileName = directory + dirEntry->d_name;
	 ifstream file(fileName.c_str());

	 // Check that we opened the file properly
	 if (!file) {
#ifdef DEBUG
	    debug("Could not open language file: " + fileName);
#endif
	    continue;
	 }
	 
#ifdef DEBUG_EXTENDED
	 debug("Loading language file " + fileName);
#endif
	 
	 // Reset some variables
	 LangData *lang = new LangData();
	 langData.clear();
	 line = tag = data = "";
	 lineNum = 0;
	 
	 while (!file.eof()) {
	    // Clean-up
	    tag = data = "";
	    
	    // Read the line
//	    file >> line;
	    std::getline(file, line);
	    lineNum++;
	    
	    // Trim the line
	    line = line.trim();
	    
	    // Is the line a comment or a blank line?
	    if (line.empty() || (line[0] == '#')) {
	       continue;
	    }
	    
	    // Grab the two tokens
	    StringTokens st(line);
	    tag = st.nextToken('=').trim().toUpper();
	    
	    // Look up the tag in the taglist (this is really inefficient)
	    bool found = false;
	    unsigned int tn = LangTags::_END_;
	    for (; tn--;) {
	       // Check for a match
	       if (tag == languageTags[tn].name) {
		  // Mark it as found and break out of the loop
		  found = true;
		  break;
	       }
	    }
	    
	    // If we did not find the tag, skip it
	    if (!found) {
	       continue;
	    }
	    
	    // Look up what we should do with this - one word or the whole lot?
	    if (languageTags[tn].oneword) {
	       data = st.nextToken().trim();
	    } else {
	       data = st.rest().trim();
	    }

	    // Check the data for this line
	    if (!tag.length() || !data.length()) {
#ifdef DEBUG
	       debug("Bad language line " + String::convert(lineNum) + 
		     ": " + line);
#endif
	       continue;
	    }
	    
#ifdef DEBUG_PSYCHO
	    debug(" +-> " + String(tag) + ": " + data);
#endif

	    // Is this data already in the map?
	    if (langData[tag]) {
	       // Delete this and add a fresh one
	       delete langData[tag];
	    } 
	    
	    // Add it..
	    langData[tag] = new String(data);
	 }
	 
	 // Close the file
	 file.close();
	 
	 /* Run through our language tags list and grab data from the map and
	  * shove it into the LanguageData:: vector in proper order
	  */
	 for (int i = 0; i < LangTags::_END_; i++) {
	    // Check if this has been set
	    if (langData[languageTags[i].name]) {
	       lang->dialogue.push_back(*langData[languageTags[i].name]);
	    } else {
	       // Check if this is required
//	       if (languageTags[i].required) {
	          // fail this language...
//	       } else {
		  lang->dialogue.push_back("");
//	       }
	       
	       // Erase that entry so the map can delete properly
	       langData.erase(languageTags[i].name);
	    }
	 }

	 // Add this language to the map thingy
	 languages[lang->get(LangTags::LANGCODE)] = lang;

	 // Clean out the map
	 String *foo;
	 while (!langData.empty()) {
	    foo = (*langData.begin()).second;
	    langData.erase(langData.begin());
	    delete foo;
	 }
      }
   }

   // Determine the default language
   defaultLanguage = NULL;
   if (defaultCode.length()) {
      defaultLanguage = get(defaultCode);
   }
   
   // Check the default language has been set, else make a dodgey presumption
   if (!defaultLanguage) {
      defaultLanguage = (*languages.begin()).second;
   }
   
   // Generate the ISUPPORT info
   ISUPPORTcodes = "";
   for (languages_map_t::iterator it = languages.begin(); 
	it != languages.end(); it++) {
      // If there is already something in the line, add a comma before the code
      if (ISUPPORTcodes.length()) {
	 ISUPPORTcodes += ',';
      }
      
      // If this is the default, add it with braces, otherwise add it normally
      if ((*it).second == defaultLanguage) {
	 ISUPPORTcodes += '(' + (*it).first + ')';
      } else {
	 ISUPPORTcodes += (*it).first;
      }
   }

   // OK!
   return true;
};


/* get - Find language data on related to the given language code, if possible
 * Original 30/10/01 simonb
 * Note: The given code will be presumed to be in the appropriate case (lower)
 *       for searching, eg. 'en' not 'En' etc.
 */
LangData *Lang::get(String const &code)
{
   // Find the language
   LangData *ld = languages[code];
   
   // Check that we got it
   if (ld) {
      return ld;
   };
   
   // Delete the empty record we just added and return null
   languages.erase(code);

   return NULL;
}


/* lang - Grab language dialogue from the default language if selected
 * Original 03/11/01 simonb
 * Notes: This should be inline...
 */
String Lang::lang(LangTags::tag_t const &t)
{
   return (defaultLanguage ? defaultLanguage->get(t) : '*');
};
