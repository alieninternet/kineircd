/* $Id$
 * 
 * Copyright (c) 2003 Simon Butcher <pickle@alien.net.au>
 * Copyright (c) 2003 KineIRCd Development Team
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

#include <algorithm>
#include <iostream>
#include <fstream>
#include <cctype>
#include <cstdlib>
#include <aisutil/string/tokens.h>
#include <aisutil/utils.h>

#include "kineircd/languages.h"
#include "libkineircd/debug.h"

using namespace Kine;
using AISutil::String;
using AISutil::StringTokens;


// Our instance...
Languages* Languages::instance = 0;


// Replacement object glyph (will appear as nothing to clients)
const char* const Languages::replacementObjectGlyph = 
  "\357\277\274"; // <=- Unicode U+0FFFC; UTF-8 0xEF 0xBF 0xBC

// Replacement character glyph (shown to the client)
const char* const Languages::replacementCharacterGlyph =
  "\357\277\275"; // <=- Unicode U+0FFFD; UTF-8 0xEF 0xBF 0xBD


// Marker chars
const char Languages::parameterMarkerChar =
  '\000'; // NUL
const char Languages::newLineMarkerChar =
  '\001'; // SOH


/* initInstance - Create the single instance of this class
 * Original 13/03/2003 simonb
 * Note: This is separated from the getInstance() since doing the if for every
 *       reference call is damned wasteful, don't you think? :)
 */
void Languages::initInstance(void)
{
#ifdef KINE_DEBUG_ASSERT
   // We should only ever be called to init the first instance..
   assert(instance == 0);
#else
   // Well, since we are not debugging I suppose we should quietly ignore this
   if (instance != 0) {
      return;
   }
#endif
   
   // Create the new instance, then!
   instance = new Languages();
   
#ifdef KINE_DEBUG
   std::ostringstream debugOut;
   debugOut << "Languages::initInstance() - Created new instance @ " <<
     (void *)instance;
   debug(debugOut.str());
#endif
}


/* processTagMap - Process a given tag name -> tag id map
 * Original 18/03/2003 simonb
 */
void Languages::processTagMap(tagMap_type map) const
{
#ifdef KINE_DEBUG
   std::ostringstream out;
   out << "Languages::processTagMap() - Processing map @ " << (void *)map;
   debug(out.str());
#endif

   int i = -1;
   
   // Iterate through the given map, until we find a null tag name
   while (map[++i].tagName != 0) {
      // Try to find this tag in the tag dictionary
      tagDictionary_type::const_iterator it =
	tagDictionary.find(String(map[i].tagName).toUpper());
      
      // Did we find it?
      if (it == tagDictionary.end()) {
#ifdef KINE_DEBUG
	 debug(String("Languages::processTagMap() - Did not find ") +
	       map[i].tagName);
#endif
	 
	 // We didn't find it
	 map[i].tagID = unknownTagID;
	 continue;
      } 
      
#ifdef KINE_DEBUG
      debug("Languages::processTagMap() - Found tag " + (*it).first + 
	    " with TID of " + String::convert((*it).second));
#endif

      // We must have found it - put the tag ID we found in the map
      map[i].tagID = (*it).second;
   }
}


/* loadFile - Load a given language file
 * Original 21/08/2002 simonb
 */
bool Languages::loadFile(const std::string& fileName, std::string& errString,
			 const bool makeDefault)
{
#ifdef KINE_DEBUG
   debug("Languages::loadFile() - Attemping to open '" + fileName +
	 '\'');
#endif
   
   // Open the file
   std::ifstream file(fileName.c_str());
   
   // Check that we opened the file properly
   if (!file) {
      errString = "Could not open the file";
      return false;
   }
   
#ifdef KINE_DEBUG
   debug("Languages::loadFile() - Beginning read loop");
#endif
   
   // Stuff we need during the read
   String line;
   String tag;
   String data;
   unsigned long lineNum = 0;

   // Create what may be either our permanent or temporary home for this data
   LanguageData* languageData = new LanguageData();
   
#ifdef KINE_DEBUG
   std::ostringstream out;
   out << "Languages::loadFile() - Current language data @ " <<
     (void *)languageData;
   debug(out.str());
#endif

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
	       line += parameterMarkerChar;
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

		case 'n': // New line (actually, it's a secret char, SOH)
		  line += newLineMarkerChar;
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
	 if ((data[i] <= 1) ||
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
	    line += replacementCharacterGlyph;
	    continue;
	 } 
	    
	 // If we got here, just copy the char over
	 line += data[i];
      }

#ifdef KINE_DEBUG
      debug("Languages::loadFile() - Line " + String::convert(lineNum) +
	    " data: '" + line + '\'');
#endif

#ifdef KINE_SECURITY_CONSCIOUS
      // Make sure the data contains valid UTF-8 sequences..
      if (!AISutil::Utils::validateUTF8(line)) {
	 // Wuss out - this is not a nice file to have loaded
	 errString = 
	   "File contains invalid UTF-8 sequence(s) and poses a potential "
	   "security problem";
	 return false;
      }
#endif
      
      // Check the first letter of the tag...
      if (tag[0] == '.') {
	 // Strip away the first char and convert it to upper-case
	 tag = tag.substr(1);
	 
#ifdef KINE_DEBUG
	 debug("Languages::LoadFile() - Line " + String::convert(lineNum) +
	       " tag: '" + tag + "' - Control tag");
#endif
	 
	 // Do something with the control tag (this is ugly)
	 if (tag == "LANGCODE") {
	    languageData->languageCode =
	      data.toLower().substr(0, data.find(' '));
	 } else if (tag == "LANGNAME") {
	    languageData->languageName = data;
	 } else if (tag == "LANGNOTE") {
	    languageData->languageNote = data;
	 } else if (tag == "MAINTAINER") {
	    languageData->maintainer = data;
	 } else if (tag == "REVISION") {
	    languageData->fileRevision = atol(data.c_str());
	 }
      } else {
	 tagID_type tagID;
	 
	 // Look for this tag in the dictionary
	 tagDictionary_type::const_iterator it = tagDictionary.find(tag);
	 
	 // Did we find it?
	 if (it == tagDictionary.end()) {
	    // We did not find it - make a new tag..
	    tagID = ++highestTagID;
	    
#ifdef KINE_DEBUG
	 debug("Languages::loadFile() - Line " + String::convert(lineNum) +
	       " tag: '" + tag + "' - TID #" + String::convert(tagID) +
	       " (new)");
#endif

	    // .. and insert its TUID into the tag dictionary
	    tagDictionary.insert(tagDictionary_type::value_type(tag, tagID));
	 } else {
	    // Use the tag ID we found
	    tagID = (*it).second;
	    
#ifdef KINE_DEBUG
	 debug("Languages::loadFile() - Line " + String::convert(lineNum) +
	       " tag: '" + tag + "' - TID #" + String::convert(tagID) +
	       " (found)");
#endif
	 }

	 // If the language data vector is not big enough yet, resize it
	 if (languageData->tagData.size() < tagID) {
	    languageData->tagData.resize(tagID);
	 }
	 
	 // Copy the tag's data into this language's tag data vector
	 languageData->tagData[tagID - 1] = new std::string(line);

	 // Increase the tag counter..
	 languageData->tagCount++;
      }
   }

   // Try to find this language..
   LanguageData* const langData = findByCode(languageData->languageCode);
   
   if (langData == 0) {
#ifdef KINE_DEBUG
      debug("Languages::loadFile() - Inserting as new data");
#endif
      
      // This is a new language, add this language to the list..
      languageDataMap.insert(languageDataMap_type::value_type
			     (languageData->languageCode, languageData));
   } else {
#ifdef KINE_DEBUG
      debug("Languages::loadFile() - Merging with existing data");
#endif
      
      /* This language already exists, so we must merge this data "over" the
       * existing data in memory..
       */
      langData->mergeWith(*languageData);
      
      // Destroy the language data..
      delete languageData;
      
      // Point the language data pointer at where the real stuff is
      languageData = langData;
   }
   
   /* If there is no default language, or we are told to make this the
    * default language, then make this language the default! :)
    */
   if ((defaultLanguage == 0) || makeDefault) {
      defaultLanguage = languageData;
   }
   
   // Close the file!
   file.close();

   /* If the daemon is not in the middle of configuration, we need to trigger
    * the tag maps to be updated, otherwise any new tags that were needed and
    * now actually exist won't be discovered automatically..
    */
//   if (!config().isConfiguring()) {
      processMaps();
//   }
   
   // Presume everything went okay!
   return true;
}


/* registerMap - Register a tag name to tag ID correlation array
 * Original 19/03/2003 simonb
 */
bool Languages::registerMap(tagMap_type map)
{
#ifdef KINE_DEBUG
   std::ostringstream debugOut;
   debugOut << "Languages::registerMap() - Attempting to register map @ " <<
     (void *)map;
   debug(debugOut.str());
#endif

#ifdef KINE_DEBUG_ASSERT
   // Make sure the map is not invalid..
   assert((void*)map != 0);
#endif
   
//   // Firstly, we need to see if this is already registered..
//   if (tagMaps.find((tagMap_type*)&map) != tagMaps.end()) {
//#ifdef KINE_DEBUG
//      debug("Languages::registerMap() - Map already exists");
//#endif
//
//      // Don't actually add it, but act as if we did :)
//      return true;
//   }

   // Okay, good enough, add it!
   (void)tagMaps.insert((tagMap_type*)&map);
   
#ifdef KINE_DEBUG
   debug("Languages::registerMap() - Map array added to the set (" +
	 String::convert(tagMaps.size()) + " maps in the set)");
#endif

   // Process this map now..
   processTagMap(map);
   
   return true;
}


/* deregisterMap - Remove a tag map array from our table (ie. module unloading)
 * Original 19/03/2003 simonb
 */
void Languages::deregisterMap(const tagMap_type map)
{
#ifdef KINE_DEBUG
   std::ostringstream debugOut;
   debugOut << "Languages::registerMap() - Attempting to deregister map @ " <<
     (void *)map;
   debug(debugOut.str());
#endif
   
   /* Just try to erase it.. (We really don't care if it is not actually 
    * registered)
    */
   (void)tagMaps.erase((tagMap_type*)&map);
}


/* processMaps - Process all known language maps
 * Original 22/03/2003 simonb
 * Note: No, I'm not using std::for_each - it's too much effort for something
 *       this simple. std::for_each requires the processTagMap() function to
 *       have a wrapper, and other rubbish.
 */
void Languages::processMaps(void) const
{
   for (tagMaps_type::const_iterator it = tagMaps.begin();
	it != tagMaps.end();
	it++) {
      processTagMap(*(*it));
   }
}


/* findByCode - Find a language by the given language code
 * Original 22/03/2003 simonb
 */
Languages::LanguageData* const
  Languages::findByCode(const std::string& code) const
{
   // Find the given language..
   const languageDataMap_type::const_iterator it =
     languageDataMap.find(String(code).toLower());
   
   // Did we find the language?
   if (it != languageDataMap.end()) {
      return (*it).second;
   }
   
   // Could not find it..
   return 0;
}


/* get - Return the given language data, from the given language
 * Original 15/03/2003 simonb
 */
const std::string
  Languages::get(const LanguageData* const languageData,
		 const tagID_type tagID,
		 const parameterList_type* const parameters) const
{
   // Is the given language data valid?
   if (languageData != 0) {
#ifdef KINE_DEBUG
      debug("Languages::get() - Attempting to get data from " + 
	    languageData->getLanguageCode());
#endif

      // Okay, try to grab it from the language data..
      std::string tagData = languageData->get(tagID, parameters);
      
      // If we got some data, return it
      if (!tagData.empty()) {
	 return tagData;
      }

      // Can we try the default language?
      if (defaultLanguage != 0) {
#ifdef KINE_DEBUG
	 debug("Languages::get() - Attempting to get data from default "
	       "language");
#endif
	 
	 // Urgh, try to find it in the default language then
	 std::string tagData = defaultLanguage->get(tagID, parameters);
	 
	 // If we got some data, return it
	 if (!tagData.empty()) {
	    return tagData;
	 }
      }
   }

   // Give up! Return a replacement object character..
   return replacementObjectGlyph;
}


/* get - Handle a multi-call tag with the given language data
 * Original 06/04/2003 simonb
 */
void Languages::get(const LanguageData* const languageData,
		    const tagID_type tagID,
		    callFunction_type& callFunction,
		    const parameterList_type* const parameters) const
{
   // Is the given language data valid?
   if (languageData != 0) {
#ifdef KINE_DEBUG
      debug("Languages::get() - Attempting to get data from " + 
	    languageData->getLanguageCode());
#endif

      // Okay, try to grab it from the language data..
      if (languageData->get(tagID, callFunction, parameters)) {
	 return;
      }
      
      // Can we try the default language instead?
      if (defaultLanguage != 0) {
#ifdef KINE_DEBUG
	 debug("Languages::get() - Attempting to get data from default "
	       "language");
#endif
	 
	 if (defaultLanguage->get(tagID, callFunction, parameters)) {
	    return;
	 }
      }
   }

   // Give up! Use the call function to send a replacement object glyph
   (void)(callFunction)(replacementObjectGlyph);
}


/* get - Return the given language data, from a language in the given list
 * Original 04/04/2003 simonb
 */
const std::string
  Languages::get(const languageDataList_type& languageDataList,
		 const tagID_type tagID,
		 const parameterList_type* const parameters) const
{
   /* If there is no language info, or the first item on the list is null,
    * return a replacement object character..
    */
   if (languageDataList.empty()) {
      return replacementObjectGlyph;
   }
   
   std::string tagData;
   
   // Okay.. run through the list of languages and see what happens..
   for (languageDataList_type::const_iterator it = languageDataList.begin();
	it != languageDataList.end(); it++) {
      // If it's null, break
      if ((*it) == 0) {
	 return replacementObjectGlyph;
      }
      
      // Try to grab the data from this language
      tagData = (*it)->get(tagID, parameters);
      
      // If we got some data, return it
      if (!tagData.empty()) {
	 break;
      }
   }
   
   // Return whatever we got..
   return tagData;
}
