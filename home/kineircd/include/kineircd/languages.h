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

# include <kineircd/languagedata.h>
# include <kineircd/receiver.h>

namespace Kine {
   class Languages {
    public:
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
	 LanguageData::tagID_type tagID;
      };
      typedef TagMapEntry tagMap_type[];

    private:
      
      // A set full of language tag name to tag ID mapping arrays
      typedef std::set < tagMap_type* > tagMaps_type;
      tagMaps_type tagMaps;
      
      // Our single instance (we exist once, and only once)
      static Languages* instance;
      
      // Constructor
      Languages(void)
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

#endif // _INCLUDE_KINEIRCD_LANGUAGES_H_
