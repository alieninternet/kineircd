/* $Id$
 * Languages interface (langtags)
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

#include "kineircd/languages.h"
#include "kineircd/config.h"
#include "libkineircd/debug.h"

using namespace Kine;
using AISutil::String;
using AISutil::StringTokens;


// Our instance...
Languages* Languages::instance = 0;


// Two replacement characters..
const char* const Languages::replacementObjectGlyph = 
  "\357\277\274"; // <=- Unicode U+0FFFC; UTF-8 0xEF 0xBF 0xBC
const char* const Languages::replacementCharacterGlyph =
  "\357\277\275"; // <=- Unicode U+0FFFD; UTF-8 0xEF 0xBF 0xBD


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
   
#ifdef KINE_DEBUG_PSYCHO
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
   int i = -1;
   
   // Iterate through the given map, until we find a null tag name
   while (map[++i].tagName != 0) {
      // Try to find this tag in the tag dictionary
      tagDictionary_type::const_iterator it =
	tagDictionary.find(String(map[i].tagName).toUpper());
      
      // Did we find it?
      if (it == tagDictionary.end()) {
#ifdef KINE_DEBUG_PSYCHO
	 debug(String("Languages::processTagMap() - Did not find ") +
	       map[i].tagName);
#endif
	 
	 // We didn't find it
	 map[i].tagID = unknownTagID;
	 continue;
      } 
      
#ifdef KINE_DEBUG_PSYCHO
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
bool Languages::loadFile(const std::string& fileName, std::string& errString)
{
#ifdef KINE_DEBUG_PSYCHO
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
   
#ifdef KINE_DEBUG_PSYCHO
   debug("Languages::loadFile() - Beginning read loop");
#endif
   
   // Stuff we need during the read
   String line;
   String tag;
   String data;
   unsigned long lineNum = 0;

   // Create what may be either our permanent or temporary home for this data
   LanguageData* languageData = new LanguageData();
   
#ifdef KINE_DEBUG_PSYCHO
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
	    line += replacementCharacterGlyph;
	    continue;
	 } 
	    
	 // If we got here, just copy the char over
	 line += data[i];
      }

#ifdef KINE_DEBUG_PSYCHO
      debug("Languages::loadFile() - Line " + String::convert(lineNum) +
	    " data: '" + line + '\'');
#endif
      
      // Check the first letter of the tag...
      if (tag[0] == '.') {
	 // Strip away the first char and convert it to upper-case
	 tag = tag.substr(1);
	 
#ifdef KINE_DEBUG_PSYCHO
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
	    
#ifdef KINE_DEBUG_PSYCHO
	 debug("Languages::loadFile() - Line " + String::convert(lineNum) +
	       " tag: '" + tag + "' - TID #" + String::convert(tagID) +
	       " (new)");
#endif

	    // .. and insert its TUID into the tag dictionary
	    tagDictionary.insert(tagDictionary_type::value_type(tag, tagID));
	 } else {
	    // Use the tag ID we found
	    tagID = (*it).second;
	    
#ifdef KINE_DEBUG_PSYCHO
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

   // Add this language to the list..
   languageDataList.insert(languageDataList_type::value_type
			   (languageData->languageCode, languageData));
   
   // If there is no default language, make this language the default..
   if (defaultLanguage == 0) {
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
#ifdef KINE_DEBUG_PSYCHO
   std::ostringstream debugOut;
   debugOut << "Languages::registerMap() - Attempting to register map @ " <<
     (void *)map;
   debug(debugOut.str());
#endif

#ifdef KINE_DEBUG_ASSERT
   // Make sure the map is not invalid..
   assert((void*)map != 0);
#endif
   
   // Firstly, we need to see if this is already registered..
   if (tagMaps.find((tagMap_type*)&map) != tagMaps.end()) {
#ifdef KINE_DEBUG_PSYCHO
      debug("Languages::registerMap() - Map already exists");
#endif

      // Don't actually add it, but act as if we did :)
      return true;
   }

   // Okay, good enough, add it!
   (void)tagMaps.insert((tagMap_type*)&map);
   
#ifdef KINE_DEBUG_PSYCHO
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
#ifdef KINE_DEBUG_PSYCHO
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
   const languageDataList_type::const_iterator it =
     languageDataList.find(String(code).toLower());
   
   // Did we find the language?
   if (it != languageDataList.end()) {
      return (*it).second;
   }
   
   // Could not find it..
   return 0;
}


/* get - Return the given language data, from the given language
 * Original 15/03/2003 simonb
 */
const std::string
  Languages::get(const std::string& languageCode,
		 const tagID_type tagID,
		 const parameterList_type* const parameters) const
{
#ifdef KINE_DEBUG_PSYCHO
   debug("Languages::get() - Requested TID #" + String::convert(tagID) + 
	 " from language " + languageCode + " (" + 
	 String::convert(languageDataList.size()) + " langs available)");
#endif

   // Check if the tag ID is 0, or larger than the highest known TID..
   if ((tagID == 0) || (tagID > highestTagID)) {
      /* Return 'unknown' - we have no idea what to put here.. This is most
       * likely a missing tag from the language file(s), or a programmer has
       * done something nasty..
       */
      return replacementObjectGlyph;
   }

   const std::string* tagData = 0;
   
   // Find the given language..
   LanguageData* const langData = findByCode(languageCode);
   
   // Did we find the language?
   if (langData != 0) {
#ifdef KINE_DEBUG_PSYCHO
      debug("Languages::get() - Attempting to get data from " + languageCode);
#endif

      // Okay, try to grab it from the language data..
      tagData = langData->findTag(tagID);
   }
   
   // Did we get tag data? Can we try the default language?
   if ((tagData == 0) && (defaultLanguage != 0)) {
#ifdef KINE_DEBUG_PSYCHO
      debug("Languages::get() - Attempting to get data from default language");
#endif

      // Urgh, try to find it in the default language then
      tagData = defaultLanguage->findTag(tagID);
   }
   
   // Okay, last try.. Did we get tag data?
   if (tagData == 0) {
#ifdef KINE_DEBUG_PSYCHO
      debug("Languages::get() - No language data available");
#endif

      // Give up! Return a replacement object character..
      return replacementObjectGlyph;
   }
   
   /* If the null character (used to mark necessary substitutions) is missing
    * from the tag data, then we do not need to do any processing. Is this
    * wise to run over the string potentially twice here? In doing this, I'm
    * presuming the majority of tags would not require substitutions..
    */
   if (tagData->find('\0') == (std::string::size_type)-1) {
      // Simply return the string, no further work is necessary..
      return *tagData;
   }
   
   // Our output string; this is what we will return after processing
   std::string output;
   
   // Run through the tag data string and substitute anything we can
   for (std::string::size_type i = 0; i < tagData->length(); i++) {
      // Is this a substitution?
      if ((*tagData)[i] == '\0') {
	 // Make sure there is more on the line..
	 if ((i + 1) >= tagData->length()) {
	    // Just skip it - eek!
	    break;
	 }
	 
	 // What do we do with this substitution?
	 switch ((*tagData)[++i]) {
	  case 'a': // The name of the server's administrator
	    output += config().getAdministratorName();
	    continue;
	    
	  case 'A': // The contact point of the server's administrator (e-mail)
	    output += config().getAdministratorContact();
	    continue;
	    
	  case 'd': // The description of the server
	    output += config().getOptionsDescription();
	    continue;
	    
	  case 'L': // The location of the server
	    output += config().getAdministratorLocation();
	    continue;
	    
	  case 'n': // The name of the server (its hostname)
	    output += config().getOptionsServerName();
	    continue;
	    
	  case 'N': // The name of the network (if there is one)
	    output += config().getNetworkName();
	    continue;
	    
	  case 'p':
	  case 'P':
	  case 'q':
	  case 'Q': // A parameter substitution, from the given parameters list
	    // Next char..
	    i++;
	    
	    /* The next char must be a numeric digit, and we must have been
	     * given parameters to work with! Otherwise, there's no point
	     * working out WHICH parameter we're supposed to look at, is
	     * there?
	     */
	    if ((parameters != 0) &&
		(i <= tagData->length()) &&
		(isdigit((*tagData)[i]))) {
	       unsigned char paramNumber;
	       
	       // Work out the base parameter number..
	       switch ((*tagData)[i - 1]) {
		case 'p':
		  paramNumber = 0;
		  break;
		case 'P':
		  paramNumber = 10;
		  break;
		case 'q':
		  paramNumber = 20;
		  break;
		case 'Q':
		  paramNumber = 30;
	       }
	       
	       // Add on the supplementary parameter number
	       paramNumber += (unsigned char)((*tagData)[i] - 0x30);
	       
	       // Do we have this parameter in our list??
	       if ((parameterList_type::size_type)(paramNumber + 1) <=
		   parameters->size()) {
		  /* Fantastic, we must have been given this parameter.. Now
		   * all we need to do is append it to out output string
		   * and we're done with it - finally!
		   * 
		   * This look uglier than it really is - honest :(
		   */
		  output += *((*parameters)[paramNumber]);
		  continue;
	       }
	    }
	    
	    // Throw a replacement char in place..
	    output += replacementCharacterGlyph;
	    continue;
	      
	  default: // NFI what this is supposed to be, use a replacement char
	    output += replacementCharacterGlyph;
	    continue;
	 }
      }
      
      // Copy this char flat
      output += (*tagData)[i];
   }
   
   // Return the processed output
   return output;
}

  
/* get - Functions for lazy people (for one to five parameters)
 * Original 20/03/2003 simonb
 */
const std::string Languages::get(const std::string& languageCode,
				 const tagID_type tagID,
				 const parameter_type& p0) const
{ 
   parameterList_type parameters;
   parameters.push_back(&p0);
   std::string output(get(languageCode, tagID, &parameters));
   return output;
}

const std::string Languages::get(const std::string& languageCode,
				 const tagID_type tagID,
				 const parameter_type& p0,
				 const parameter_type& p1) const
{
   parameterList_type parameters;
   parameters.push_back(&p0);
   parameters.push_back(&p1);
   std::string output(get(languageCode, tagID, &parameters));
   return output;
}

const std::string Languages::get(const std::string& languageCode,
				 const tagID_type tagID,
				 const parameter_type& p0,
				 const parameter_type& p1,
				 const parameter_type& p2) const
{
   parameterList_type parameters;
   parameters.push_back(&p0);
   parameters.push_back(&p1);
   parameters.push_back(&p2);
   std::string output(get(languageCode, tagID, &parameters));
   return output;
}

const std::string Languages::get(const std::string& languageCode,
				 const tagID_type tagID,
				 const parameter_type& p0,
				 const parameter_type& p1,
				 const parameter_type& p2,
				 const parameter_type& p3) const
{
   parameterList_type parameters;
   parameters.push_back(&p0);
   parameters.push_back(&p1);
   parameters.push_back(&p2);
   parameters.push_back(&p3);
   std::string output(get(languageCode, tagID, &parameters));
   return output;
}

const std::string Languages::get(const std::string& languageCode,
				 const tagID_type tagID,
				 const parameter_type& p0,
				 const parameter_type& p1,
				 const parameter_type& p2,
				 const parameter_type& p3,
				 const parameter_type& p4) const
{
   parameterList_type parameters;
   parameters.push_back(&p0);
   parameters.push_back(&p1);
   parameters.push_back(&p2);
   parameters.push_back(&p3);
   parameters.push_back(&p4);
   std::string output(get(languageCode, tagID, &parameters));
   return output;
}
