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
#include <cstring>
#include <aisutil/string/tokens.h>
#include <aisutil/utils.h>

#include "kineircd/languages.h"
#include "lib/debug.h"
#include "lib/language/subst.h"

using namespace Kine;
using AIS::Util::String;
using AIS::Util::StringTokens;

// The conversion output buffer size
#define ICONV_BUFFER_SIZE 256 // 256 * 4 = 1kB if wchar_t is 32-bits long

// Our instance...
Languages* Languages::instance = 0;


// The code of a null language ('none')
const char* const Languages::nullLanguageCode =
  "none";

// The code which will reference the default language (see RFC2277)
const char* const Languages::defaultLanguageCode =
  "i-default";


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
      // Find the tag ID for this entry
      map[i].tagID = getTagID(map[i].tagName);
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

   // Open our conversion descriptor for iconv()
#ifdef KINE_DEBUG
   debug("Languages::loadFile() - Opening iconv() conversion descriptor");
#endif
   iconv_t convDesc = iconv_open(KINE_INTERNAL_CHARSET,
				 KINE_LANGFILE_CHARSET);
   
   // Make sure the conversion descriptor opened happily
   if (convDesc == (iconv_t)(-1)) {
      errString =
	"Could not open conversion descriptor for " 
	KINE_LANGTAGS_LANGFILE_CHARSET " -> " KINE_LANGTAGS_INTERNAL_CHARSET
	" (";
      errString += strerror(errno);
      errString += ')';
      return false;
   }
   
#ifdef KINE_DEBUG
   debug("Languages::loadFile() - Beginning read loop");
#endif
   
   // Line number counter (for error message convenience)
   unsigned long lineNum = 0;

   // Output buffer for iconv()
   wchar_t convOutBuffer[ICONV_BUFFER_SIZE];
   
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
      // Line we read into
      String line;
      
      // Read the line
      std::getline(file, line);

      // Make sure the file is still happy..
      if (!file.good() || file.eof()) {
	 break;
      }
      
      // Increase the line counter
      ++lineNum;
      
      // Trim the line
      line = line.trim();
      
      // Is the line empty, or perhaps even a comment?
      if (line.empty() || (line[0] == '#')) {
	 // Skip this line!
	 continue;
      }
      
      // Rip out the two sides to the token
      StringTokens st(line);
      String tag = st.nextToken('=').trim().toUpper();
      String data = st.rest().trim();

      // Clear the line variable.. We will reuse it soon..
      line.clear();

      // Process the data: substitute anything we know
      for (std::string::size_type i = 0; i < data.length(); ++i) {
	 if (data[i] == '%') {
	    /* Check the next character. If it's another percentage sign, let
	     * it get copied, otherwise convert it over to the super secret
	     * hush hush magical mystical char... which is a null.. ;)
	     */
	    if (data[i + 1] == '%') {
	       ++i;
	    } else {
	       line += Internal::LangTags::parameterMarkerChar;
	       continue;
	    }
	 }
	 
	 // Escaped char?
	 if (data[i] == '\\') {
	    // Next char
	    ++i;
	    
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
	       // Iterate over the substitution list to find this char..
	       for (unsigned int j = 0;
		    Internal::LangTags::substitutionMap[j].from != 0;
		    ++j) {
		  // Does this match?
		  if (Internal::LangTags::substitutionMap[j].from == data[i]) {
		     // Append this..
		     line += Internal::LangTags::substitutionMap[j].to;
		     
		     // No need to continue the loop..
		     break;
		  }
	       }
	       
	       // No need to do any further processing on this char..
	       continue;
	    }
	 }

	 // If we got here, just copy the char over
	 line += data[i];
      }

#ifdef KINE_DEBUG
      debug("Languages::loadFile() - Line " + String::convert(lineNum) +
	    " data: '" + line + '\'');
#endif

      // Stuff needed for iconv()'ing the data
      size_t convInBytes = line.length();
      size_t convOutBytes = ICONV_BUFFER_SIZE * sizeof(wchar_t);
      char* convInPtr = const_cast<char*>(line.data());
      char* convOutPtr = reinterpret_cast<char*>(convOutBuffer);

      // Initialise the conversion descriptor for this run
      (void)iconv(convDesc, NULL, NULL, &convOutPtr, &convOutBytes);
   
      // Make a new data string (where the output buffer will go)
      std::wstring* const convertedData = new std::wstring();

      // Convert the data
      for (;;) {
	 const size_t convVal = iconv(convDesc,
				      &convInPtr, &convInBytes,
				      &convOutPtr, &convOutBytes);

	 // Is iconv happy?
	 if (convVal != (size_t)(-1)) {
	    // Yay, all converted - break the loop
	    break;
	 }

	 // An error occurred.. Check what it was..
	 switch (errno) {
	  case E2BIG: // No more room left in the output buffer
#ifdef KINE_DEBUG
	    debug("Languages::loadFile() - Conversion output buffer "
		  "exhausted, dumping..");
#endif
	    
	    // Copy everything from the output buffer to the data string
	    convertedData->append(convOutBuffer, ICONV_BUFFER_SIZE);
	    
	    // Reset the static output buffer
	    convOutBytes = ICONV_BUFFER_SIZE * sizeof(wchar_t);
	    convOutPtr = reinterpret_cast<char*>(convOutBuffer);
	    continue;
	    
	  case EILSEQ: // An illegal multibyte sequence was found in the input
	  case EINVAL: // An incomplete multibyte sequence was found..
	    errString =
	      "Illegal " KINE_LANGTAGS_LANGFILE_CHARSET
	      " sequence or corrupt language file";
	    (void)iconv_close(convDesc);
	    return false;
	    
	  default: // No idea what this error is..
	    errString = "Unknown conversion error: ";
	    errString += strerror(errno);
	    (void)iconv_close(convDesc);
	    return false;
	 }
      }
      
      // Okay, was anything in the buffer after that?
      const std::wstring::size_type convOutChars =
	(((int)convOutPtr - (int)convOutBuffer) / sizeof(wchar_t));
      if (convOutChars > 0) {
#ifdef KINE_DEBUG
	 debug("Languages::loadFile() - Dumping the rest of the output "
	       "buffer");
#endif
	 convertedData->append(convOutBuffer, convOutChars);
      }
      
      // Run over the tag data (erk, again - I *REALLY* do not like this)
      for (std::wstring::size_type i = 0; i < convertedData->length(); ++i) {
	 /* If the character is one of those naughty control characters,
	  * up-convert it into the pictoral range so it will be viewed
	  * graphically, but not thrown around. These are worst-case naughty
	  * chars from the C0 control set, and should be stomped on.
	  */
	 if (((*convertedData)[i] == 0x0000) || // NUL
	     ((*convertedData)[i] == 0x0001) || // SOH
	     ((*convertedData)[i] == 0x0008) || // BS
	     (((*convertedData)[i] >= 0x000A) &&
	      ((*convertedData)[i] <= 0x000D)) || // LF, VT, FF, CR
	     (((*convertedData)[i] >= 0x0011) &&
	      ((*convertedData)[i] <= 0x0014))) { // DC1 -> DC4
	    /* The pictoral range for the C1 controls is 0x2400 -> 0x2419,
	     * and directly correspond with the C1 controls themselves
	     * (0x00 -> 0x2419), so we simply need to add 0x2400 to change
	     * these.
	     */
	    (*convertedData)[i] += 0x2400;
	    continue;
	 }
	 
	 // DEL chars are equally bad, but have their own special control pic
	 if ((*convertedData)[i] == 0x007F) {
	    (*convertedData)[i] = 0x2421;
	    continue;
	 }
	 
	 /* There are some other naughty chars, such as those you find in the
	  * C1 control group (ISO 6429, and of course ISO 2022), and UCS
	  * specials. These don't have pretty pictoral forms elsewhere in the
	  * UCS range yet, so we just have to munge them into one, very boring,
	  * replacement character.
	  */
	 if (((*convertedData)[i] == 0x0085) || // NEL
	     ((*convertedData)[i] == 0x008D) || // RI
	     (((*convertedData)[i] >= 0xFFFC) &&
	      ((*convertedData)[i] <= 0xFFFF))) { // UCS Specials
	    (*convertedData)[i] =
	      Internal::LangTags::replacementCharacterGlyph;
	 } 
      }
      
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
	    languageData->languageName = *convertedData;
	 } else if (tag == "LANGNOTE") {
	    languageData->languageNote = *convertedData;
	 } else if (tag == "MAINTAINER") {
	    languageData->maintainer = *convertedData;
	 } else if (tag == "REVISION") {
	    languageData->fileRevision = atol(data.c_str());
	 }
	 
	 // Free the converted tag data - it would have been ignored/copied..
	 delete convertedData;
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
	 
	 // Copy the tag's data pointer into this language's tag data vector
	 languageData->tagData[tagID - 1] = convertedData;

	 // Increase the tag counter..
	 ++languageData->tagCount;
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
   
   // Close the iconv() conversion descriptor
   (void)iconv_close(convDesc);
   
   // Close the file!
   file.close();

   /* If the daemon is not in the middle of configuration, we need to trigger
    * the tag maps to be updated, otherwise any new tags that were needed and
    * now actually exist won't be discovered automatically..
    */
#ifdef KINE_DEBUG
# warning "Language maps are processed, regardless of config().isConfiguring()"
#endif
//   if (!config().isConfiguring()) {
      processMaps();
//   }
   
   // Presume everything went okay!
   return true;
}


/* getTagID - Find a tag ID associated with the given tag name
 * Original 11/04/2003 simonb
 */
const Languages::tagID_type
  Languages::getTagID(const std::string& tagName) const
{
   // Try to find this tag in the tag dictionary
   tagDictionary_type::const_iterator it =
     tagDictionary.find(static_cast<AIS::Util::String>(tagName).toUpper());
      
   // Did we find it?
   if (it == tagDictionary.end()) {
#ifdef KINE_DEBUG
      debug(String("Languages::getTagID() - Did not find ") + tagName);
#endif
      
      // We didn't find it
      return unknownTagID;
   } 
      
#ifdef KINE_DEBUG
   debug("Languages::getTagID() - Found tag " + (*it).first + " with TID of " +
	 String::convert((*it).second));
#endif

   // We must have found it - return it to the caller
   return (*it).second;
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
	++it) {
      processTagMap(*(*it));
   }
}


/* findByCode - Find a language by the given language code
 * Original 22/03/2003 simonb
 */
Languages::LanguageData* const
  Languages::findByCode(const std::string& code) const
{
   String langCode = code;
   langCode = langCode.toLower();
   
   // Find the given language..
   const languageDataMap_type::const_iterator it =
     languageDataMap.find(langCode);
   
   // Did we find the language?
   if (it != languageDataMap.end()) {
      return (*it).second;
   }

   // Okay.. well, maybe it is the default language?
   if (langCode == defaultLanguageCode) {
      return defaultLanguage;
   }
   
   // Could not find it..
   return 0;
}


/* get - Return the given language data, from the given language
 * Original 15/03/2003 simonb
 */
const std::wstring
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
      std::wstring tagData = languageData->get(tagID, parameters);
      
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
	 tagData = defaultLanguage->get(tagID, parameters);
	 
	 // If we got some data, return it
	 if (!tagData.empty()) {
	    return tagData;
	 }
      }
   }

   // Give up! Return a replacement object character..
   return std::wstring(1, Internal::LangTags::replacementObjectGlyph);
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
   (void)((callFunction)
	  (std::wstring(1, Internal::LangTags::replacementObjectGlyph)));
}


/* get - Return the given language data, from a language in the given list
 * Original 04/04/2003 simonb
 */
const std::wstring
  Languages::get(const languageDataList_type& languageDataList,
		 const tagID_type tagID,
		 const parameterList_type* const parameters) const
{
   /* If there is no language info, or the first item on the list is null,
    * return a replacement object character..
    */
   if (languageDataList.empty() ||
       (languageDataList[0] == 0)) {
      return std::wstring(1, Internal::LangTags::replacementObjectGlyph);
   }
   
   std::wstring tagData;
   
   // Okay.. run through the list of languages and see what happens..
   for (languageDataList_type::const_iterator it = languageDataList.begin();
	it != languageDataList.end(); 
	++it) {
      // If it's null, break
      if ((*it) == 0) {
	 return std::wstring(1, Internal::LangTags::replacementObjectGlyph);
      }
      
      // Try to grab the data from this language
      tagData = (*it)->get(tagID, parameters);
      
      // If we got some data, return it
      if (!tagData.empty()) {
	 return tagData;
      }
   }
   
   // If we still have nothing, try the default language (if possible)
   if (tagData.empty() && 
       (defaultLanguage != 0)) {
      // Grab the data..
      tagData = defaultLanguage->get(tagID, parameters);
      
      // Got something?
      if (!tagData.empty()) {
	 return tagData;
      }
   }
   
   // Return whatever we got..
   return std::wstring(1, Internal::LangTags::replacementObjectGlyph);
}
