/* $Id$
 * Handle the languages list which resides in the daemon's configuration
 *
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

#ifndef _INCLUDE_KINEIRCD_LANGUAGELIST_H_
# define _INCLUDE_KINEIRCD_LANGUAGELIST_H_ 1

# include "kineircd/kineircdconf.h"

# ifdef KINE_STL_HAS_HASH
#  include <hash_map>
# else
#  include <map>
# endif

# include <libais/string/string.h>

# include "kineircd/languagedata.h"

namespace Kine {
   class LanguageList {
    private:
# ifdef KINE_STL_HAS_HASH
      typedef std::hash_map <LibAIS::String, LanguageData::tagID_type>
	tagDictionary_type;
# else
      typedef std::map <LibAIS::String, LanguageData::tagID_type>
	tagDictionary_type;
# endif

      /* Our "Tag Dictionary". As language files are loaded, tags are thrown
       * into the tag dictionary with a unique identifier for easier reference.
       * Later, tag tables can discover their unique identifier for their tags
       * to find the appropriate data within the language data vectors.
       */
      tagDictionary_type tagDictionary;

      /* The highest identifier within the tag dictionary, for checking etc..
       * Note that the ID '0' is used for a special purpose (no data)..
       */
      LanguageData::tagID_type highestTagID;
      
      // Our default language, to use if all else fails..
      LanguageData* defaultLanguage;
      
    public:
      // Constructor
      LanguageList()
	: highestTagID(0),
          defaultLanguage(0)
	{};
      
      // Load a new language data file
      bool loadFile(const LibAIS::String& filename, LibAIS::String& errString);
   };
};

#endif // _INCLUDE_KINEIRCD_LANGUAGELIST_H_

