/* lang.h
 * Text in a centralised location so it can be easily changed/fixed etc
 */

#ifndef __LANG_H_
# define __LANG_H_

# ifdef STL_HAS_HASH
#  include <hash_map>
# else
#  include <map>
# endif

# include <vector>

# include "langtags.h"
# include "str.h"


class LangData;


// Main language class
class Lang {
 public:
# ifdef STL_HAS_HASH
   typedef hash_map <String, LangData *> languages_map_t;
# else
   typedef hash_map <String, LangData *> languages_map_t;
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
   static String ISUPPORTcodes;			// ISUPPORT LANGUAGE= data
   
   Lang(void) {};				// Constructor (cannot be run)
   virtual ~Lang(void) {};			// Destructor
   
//   static void clear(void);			// Wipe the language lists

 public:
   static bool loadLanguages(String const &,
			     String const &);	// Load language files
   
   static LangData *get(String const &);	// Find language dialogue data
   
   // Grab the data for the LANGUAGE= 'ISUPPORT' tag
   static String getISUPPORTcodes(void)
     {
	return ISUPPORTcodes;
     };
   
   // Return language dialogue from the default language, if set
   static String lang(LangTags::tag_t const &);
   
   // Greeting lines sent when a user connects
   static char const *L_PINGPONG_NOTICE;

   // Misc/Disorganised
   static char const *L_NOTIFY_PARANOID_OPERS_ON_WHOIS;
   static char const *L_QUIT_KILLED;
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
   vector <String> dialogue;
   
 public:
   // Constructor
   LangData(void)
     {
	dialogue.clear();
     };
   
   // Destructor
   ~LangData(void)
     {
	dialogue.clear();
     };

   // Grab a string from the language dialogue data
   String get(LangTags::tag_t const &n) const
     { 
	return dialogue[n]; 
     };

   // Check if something exists in the dialogue
   bool has(LangTags::tag_t const &n) const
     {
	return (dialogue[n].length() > 0);
     };
   
   friend class Lang;
};

#endif

