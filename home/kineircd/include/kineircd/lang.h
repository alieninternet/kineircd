/* $Id$
 * Text in a centralised location so it can be easily changed/fixed etc
 * 
 * Copyright (c) 2001,2002 KineIRCd Development Team
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

#ifndef _INCLUDE_KINEIRCD_LANG_H_
# define _INCLUDE_KINEIRCD_LANG_H_ 1

# ifdef STL_HAS_HASH
#  include <hash_map>
# else
#  include <map>
# endif

# include <vector>

# include "kineircd/langtags.h"
# include "kineircd/str.h"


namespace Kine {
   
   class LangData;
   
   
   // Main language class
   class Lang {
    public:
# ifdef STL_HAS_HASH
      typedef std::hash_map <String, LangData *> languages_map_t;
# else
      typedef std::map <String, LangData *> languages_map_t;
# endif
      
      // private:
    public:
      // List of language tags (for the parser)
      struct languageTagsStruct {
	 char const *name;
	 bool const required;
	 bool const oneword;
      };
      static languageTagsStruct languageTags[LangTags::_END_];
      static languages_map_t languages;		// Installed languages
      static LangData *defaultLanguage;		// Default language
      static String ISUPPORTcodes;		// ISUPPORT LANGUAGE= data
      
      Lang(void) {};				// Constructor (cannot be run)
      virtual ~Lang(void) {};			// Destructor
      
//      static void clear(void);			// Wipe the language lists

    public:
      static bool 
	loadLanguages(String const &,
		      String const &);		// Load language files
   
      static LangData *get(String const &);	// Find language dialogue data
      
      // Grab the data for the LANGUAGE= 'ISUPPORT' tag
      static String getISUPPORTcodes(void) {
	 return ISUPPORTcodes;
      };
      
      // Return language dialogue from the default language, if set
      static String lang(LangTags::tag_t const &);
      
      // Greeting lines sent when a user connects
      static char const *L_PINGPONG_NOTICE;
      
      // Misc/Disorganised
      static char const *L_REQUESTED_SHUTDOWN;
      static char const *L_BYE_BYE_USER;
      static char const *L_ERROR_CLOSING_LINK;
      static char const *L_ERROR_CLOSING_LINK_DEFAULT_REASON;
      
      static char const *L_DEFAULT_QUIT_MESSAGE;
      
      friend class LangData;
   };
   
   
   // Language data class
   class LangData {
    private:
      std::vector <String> dialogue;
      
    public:
      // Constructor
      LangData(void) {
	 dialogue.clear();
      };
      
      // Destructor
      ~LangData(void) {
	 dialogue.clear();
      };
      
      // Grab a string from the language dialogue data
      String get(LangTags::tag_t const &n) const { 
	 return dialogue[n]; 
      };
      
      // Check if something exists in the dialogue
      bool has(LangTags::tag_t const &n) const {
	 return (dialogue[n].length() > 0);
      };
      
      friend class Lang;
   };
   
};
   
#endif // _INCLUDE_KINEIRCD_LANG_H_
   
   
