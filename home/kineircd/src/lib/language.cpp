/* language.cpp
 * Stuff for manipulating language data on the server
 */

#include "autoconf.h"

#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <iostream.h>
#include <fstream.h>
#include <errno.h>

#include "lang.h"
#include "debug.h"


namespace Lang {
   // The map with the languages in it..
   Lang::languages_map_t languages;
   
   // Default language, start off with it being none...
   LangData *defaultLanguage = 0;
   
   // Language tag list for ISUPPORT
   String ISUPPORTcodes = "";
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


   /* Server notice output strings
    * Note: This ties in with GLOBOPS, HELPME and LOCOPS broadcasts too since
    *	    they are sent by the same notice mechanism.
    */
   char const *L_SERVNOTICE_LINK =
     "Link with %s established (%s)";
   char const *L_SERVNOTICE_DELINK =
     "Link with %s cancelled: %s";
   char const *L_SERVNOTICE_SIGNON =
     "*** Notice -- Client connecting: %s (%s@%s) <%s>";
   char const *L_SERVNOTICE_SIGNOFF =
     "*** Notice -- Client exiting: %s (%s@%s> [%s]";
   char const *L_SERVNOTICE_RECV_EOB =
     "Completed receiving Burst Data from %s";
   char const *L_SERVNOTICE_CMD_DIE =
     "%s initiated server shutdown";
   char const *L_SERVNOTICE_CMD_REHASH =
     "%s started server rehash";
   char const *L_SERVNOTICE_GARBO =
     "Garbologist completed cleaning";
   char const *L_SERVNOTICE_GARBO_ITEMS =
     "Garbologist completed cleaning %d items";
   char const *L_SERVNOTICE_GLOBOPS =
     "*** Global -- OPER from %s: %s";
   char const *L_SERVNOTICE_HELPME =
     "*** Help -- from %s: %s";
   char const *L_SERVNOTICE_LOCOPS =
     "*** Local -- from %s: %s";
   char const *L_SERVNOTICE_NOTIFY_ON_CMD_STATS =
     "*** Notice -- STATS %s by %s (%s)";
   
   // Misc/Disorganised
   char const *L_NOTIFY_PARANOID_OPERS_ON_WHOIS =
     "*** Notice -- %s (%s) did a /whois on you";

   char const *L_QUIT_KILLED =
     "Killed (%s (%s))";

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
 * Original 28/10/01, Simon Butcher <pickle@austnet.org>
 */
bool Lang::loadLanguages(String const &directory, 
			 String const &defaultCode)
{
#ifdef DEBUG
   debug("Loading language files...");
#endif
   
   struct dirent *dirEntry;
   DIR *dir = opendir(directory);

   // Make sure we opened that directory
   if (!dir) {
#ifdef DEBUG
      debug(String("Error opening language file directory: ") +
	    strerror(errno));
#endif
      return false;
   }
   
   // Stuff we need
#ifdef STL_HAS_HASH
   hash_map <String, String *> langData;
#else
   map <String, String *> langData;
#endif
   String line = "";
   String tag = "";
   String data = "";
   unsigned int lineNum = 0;
   
   // Run through the directory list and look for files we want
   while ((dirEntry = readdir(dir)) != NULL) {
      // Check the name...
      if (!strncasecmp(dirEntry->d_name, LANG_FILE_PREFIX,
		       LANG_FILE_PREFIX_LEN)) {
	 // Open the file
	 String fileName = String(dirEntry->d_name);
	 ifstream file(directory + fileName);

	 // Check that we opened the file properly
	 if (!file) {
#ifdef DEBUG
	    debug(String("Could not open language file: ") + fileName);
#endif
	    continue;
	 }
	 
#ifdef DEBUG_EXTENDED
	 debug(String("Loading language file ") + fileName);
#endif
	 
	 // Reset some variables
	 LangData *lang = new LangData();
	 langData.clear();
	 line = "";
	 tag = "";
	 data = "";
	 lineNum = 0;
	 
	 while (!file.eof()) {
	    // Clean-up
	    tag = "";
	    data = "";
	    
	    // Read the line
	    file >> line;
	    lineNum++;
	    
	    // Trim the line
	    line = line.trim();
	    
	    // Is the line a comment or a blank line?
	    if (!line.length() || (line[0] == '#')) {
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
	       debug(String::printf("Bad language line %u: %s",
				    lineNum,
				    (char const *)line));
#endif
	       continue;
	    }
	    
#ifdef DEBUG_EXTENDED
	    debug(String::printf(" +-> %s: %s",
				 (char const *)tag,
				 (char const *)data));
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
	 ISUPPORTcodes = ISUPPORTcodes + String(',');
      }
      
      // If this is the default, add it with braces, otherwise add it normally
      if ((*it).second == defaultLanguage) {
	 ISUPPORTcodes = ISUPPORTcodes + "(" + (*it).first + ")";
      } else {
	 ISUPPORTcodes = ISUPPORTcodes + (*it).first;
      }
   }
   
   
   // OK!
   return true;
};


/* get - Find language data on related to the given language code, if possible
 * Original 30/10/01, Simon Butcher <pickle@austnet.org>
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
 * Original 03/11/01, Simon Butcher <pickle@austnet.org>
 * Notes: This should be inline...
 */
String Lang::lang(LangTags::tag_t const &t)
{
   return (defaultLanguage ? 
	   defaultLanguage->get(t) : 
	   String("*"));
};
