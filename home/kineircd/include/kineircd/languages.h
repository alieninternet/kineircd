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

#ifndef _INCLUDE_KINEIRCD_LANGUAGES_H_
# define _INCLUDE_KINEIRCD_LANGUAGES_H_ 1

# include <set>
# include <map>
# include <string>
# include <vector>

# include <kineircd/receiver.h>

namespace Kine {
   class LanguageData;
   
   class Languages {
    public:
      /* The tag identifier type (also referred to as a 'TID').
       * Note that TID #0 has a special purpose -- "unknown".
       */
      typedef unsigned int tagID_type;
      
      /* This is the 'tag map' structure. In order for your module to know
       * the correlation of tag name to tag ID upon run-time, you need to
       * define this structure within your module and register it with the
       * registerMap() function (defined below). Once registered, and after
       * languages have been loaded (or reloaded), we will attempt to form
       * the link between a tag name and its run-time ID (a number) using
       * this map. Using the number set within this structure, you can then
       * look up your tags. If the tag name cannot be found (i.e. it does
       * not exist within any loaded language files) the tag ID will be set
       * to 0, and an empty string will be returned whenever you try to
       * reference that tag's data.
       * 
       * You do not need to fill in the tag ID field, only the tag name. Both
       * upper and lower case is acceptable within the tag name, since the
       * lookup performed uses case-insensitive checks.
       * 
       * Terminate the map array with a null tagName (0).
       */
      struct TagMapEntry {
	 const char* const tagName;
	 tagID_type tagID;
      };
      typedef TagMapEntry tagMap_type[];

      // The type of a 'parameter' (used in substitutions)
      typedef std::string parameter_type;
      
      /* The type of a 'parameter list'. Parameter lists are sent along side
       * a request to return language data in order to substitute parameter
       * tokens within the language data itself with their appropriate values
       * prior to being returned back to the software.
       */
      typedef std::vector < const parameter_type* > parameterList_type;
      
      /* Replacement glyphs, both used to substitute missing or erroneous
       * values of some description. These are UTF-8 Encoded.
       */
      static const char* const replacementObjectGlyph;
      static const char* const replacementCharacterGlyph;
      
    private:
      // A set full of language tag name to tag ID mapping arrays
      typedef std::set < tagMap_type* > tagMaps_type;
      tagMaps_type tagMaps;
      
      /* Our "Tag Dictionary". As language files are loaded, tags are thrown
       * into the tag dictionary with a unique identifier for easier reference.
       * Later, the tag table bindery can discover each tag's unique identifier
       * to find the appropriate data within the language data vectors.
       */
# ifdef KINE_STL_HAS_HASH
      typedef std::hash_map <std::string, Languages::tagID_type>
	tagDictionary_type;
# else
      typedef std::map <std::string, Languages::tagID_type>
	tagDictionary_type;
# endif
      tagDictionary_type tagDictionary;

      // The highest known TID
      tagID_type highestTagID;
      
      // Our default language, to use if all else fails..
      const LanguageData* defaultLanguage;
      
      // Our single instance (we exist once, and only once)
      static Languages* instance;
      
      // Constructor
      Languages(void)
	: highestTagID(0),
	  defaultLanguage(0)
	{};
      
    public:
      // Destructor
      ~Languages(void)
	{};
      
      // Create the single instance of this class
      static void initInstance(void);
      
      // Return the single instance of this class (hopefully it exists ;)
      static Languages& getInstance(void)
	{ return *instance; };
      
      // Add/remove tag name/ID correlation maps
      bool registerMap(tagMap_type map);
      void deregisterMap(const tagMap_type map);
      
      // Return the given language data, from the given language
      const std::string get(const std::string& languageCode,
			    const tagID_type tagID,
			    const parameterList_type* const parameters = 0);
      
      // Lazy functions for use when you have one to five parameters..
      const std::string get(const std::string& languageCode,
			    const tagID_type tagID,
			    const parameter_type& p0);
      const std::string get(const std::string& languageCode,
			    const tagID_type tagID,
			    const parameter_type& p0,
			    const parameter_type& p1);
      const std::string get(const std::string& languageCode,
			    const tagID_type tagID,
			    const parameter_type& p0,
			    const parameter_type& p1,
			    const parameter_type& p2);
      const std::string get(const std::string& languageCode,
			    const tagID_type tagID,
			    const parameter_type& p0,
			    const parameter_type& p1,
			    const parameter_type& p2,
			    const parameter_type& p3);
      const std::string get(const std::string& languageCode,
			    const tagID_type tagID,
			    const parameter_type& p0,
			    const parameter_type& p1,
			    const parameter_type& p2,
			    const parameter_type& p3,
			    const parameter_type& p4);
      
      /* Friends -- LanguageConfig:: needs to fiddle with our private bits..
       * *giggles* ;)
       */
      friend struct LanguageConfig;
   }; // class Languages

   
   /* Lazy reference thingy for lazy coders.. like me.. but shh, don't tell
    * anyone I'm lazy and use lazy things like this lazy reference function :(
    */
   inline static Languages& languages(void)
     { return Languages::getInstance(); };
   
   
   // Oh dear, another lazy function for the truely pathetic lazy sods ;)
   inline static Languages& langs(void)
     { return Languages::getInstance(); };
}; // namespace Kine

# include <kineircd/languagedata.h>

#endif // _INCLUDE_KINEIRCD_LANGUAGES_H_
