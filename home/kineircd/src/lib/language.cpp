/* language.cpp
 * Text in a centralised location so it can be easily changed/fixed etc
 */

#include "autoconf.h"

#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <iostream.h>
#include <fstream.h>
#include <errno.h>

#include "language.h"
#include "debug.h"


namespace Language {
   /* This is a list of language tags so that the language parser knows 
    * where to shove the language data.
    * WARNING!!! Make sure this list is in perfect sync with the enumeration 
    * list in the language header file!
    */
   Language::languageTagsStruct languageTags[Language::_END_] =
     {
	  { "B_RPL_CREATED",				false,	false },
	  { "B_RPL_TIMEONSERVERIS",			false,	false },
	  { "CHARSET",					true,	true },
	  { "E_ERR_ACCEPTEXIST",			false,	false },
	  { "E_ERR_ACCEPTNOT",				false,	false },
	  { "E_ERR_NOLANGUAGE",				false,	false },
	  { "E_ERR_UNKNOWNMODE",			false,	false },
	  { "E_ERR_USERONCHANNEL",			false,	false },
	  { "E_RPL_ACCEPTED",				false,	false },
	  { "E_RPL_ACCEPTNOTICE",			false,	false },
	  { "E_RPL_ISUPPORT",				false,	false },
	  { "E_RPL_KNOCKING",				false,	false },
	  { "E_RPL_LOGOFF_CHANNEL",			false,	false },
	  { "E_RPL_LOGOFF_SERVER",			false,	false },
	  { "E_RPL_LOGOFF_USER",			false,	false },
	  { "E_RPL_LOGON_CHANNEL",			false,	false },
	  { "E_RPL_LOGON_SERVER",			false,	false },
	  { "E_RPL_LOGON_USER",				false,	false },
	  { "E_RPL_LUSERCHANNELS",			false,	false },
	  { "E_RPL_LUSERHELPERS",			false,	false },
	  { "E_RPL_LUSEROP",				false,	false },
	  { "E_RPL_LUSERUNKNOWN",			false,	false },
	  { "E_RPL_NOTACCEPTED",			false,	false },
	  { "E_RPL_NOWOFF",				false,	false },
	  { "E_RPL_NOWON_CHANNEL",			false,	false },
	  { "E_RPL_NOWON_SERVER",			false,	false },
	  { "E_RPL_NOWON_USER",				false,	false },
	  { "E_RPL_WATCHOFF",				false,	false },
	  { "E_RPL_WHOISIDLE",				false,	false },
	  { "E_RPL_WHOISOPERATOR",			false,	false },
	  { "E_RPL_WHOISSECURE",			false,	false },
	  { "L_DEFAULT_KNOCK_REASON",			false,	false },
	  { "L_ERR_ACCEPTFULL",				false,	false },
	  { "L_ERR_ALREADYREGISTERED",			false,	false },
	  { "L_ERR_BADCHANNELKEY",			false,	false },
	  { "L_ERR_BADCHANNAME",			false,	false },
	  { "L_ERR_BADCHANNAME_FAILMASK",		false,	false },
	  { "L_ERR_BADCHANNAME_SAFECHAN",		false,	false },
	  { "L_ERR_BANNEDFROMCHAN",			false,	false },
	  { "L_ERR_CANNOTSENDTOCHAN_BANNED",		false,	false },
	  { "L_ERR_CANNOTSENDTOCHAN_MODERATED",		false,	false },
	  { "L_ERR_CANNOTSENDTOCHAN_NOOUTSIDEMSG",	false,	false },
	  { "L_ERR_CANNOTSENDTOCHAN_REGNICKSONLY",	false,	false },
	  { "L_ERR_CANNOTSENDTONICK_REGNICKSONLY",	false,	false },
	  { "L_ERR_CANNOTCHANGEUMODE",			false,	false },
	  { "L_ERR_CANTKILLSERVICES",			false,	false },
	  { "L_ERR_CHANNELISFULL",			false,	false },
	  { "L_ERR_ERRONEUSNICKNAME",			false,	false },
	  { "L_ERR_EVENTNICKCHANGE",			false,	false },
	  { "L_ERR_INVITEONLYCHAN",			false,	false },
	  { "L_ERR_NEEDMOREPARAMS",			false,	false },
	  { "L_ERR_NICKNAMEINUSE",			false,	false },
	  { "L_ERR_NOKNOCK_INVITED",			false,	false },
	  { "L_ERR_NOKNOCK_ONCHANNEL",			false,	false },
	  { "L_ERR_NOKNOCK_OPENCHANNEL",		false,	false },
	  { "L_ERR_NOKNOCK_REGONLY",			false,	false },
	  { "L_ERR_NOMORELANGS",			false,	false },
	  { "L_ERR_NONICKNAMEGIVEN",			false,	false },
	  { "L_ERR_NONONREG_CHANNEL",			false,	false },
	  { "L_ERR_NOOPERHOST",				false,	false },
	  { "L_ERR_NOOPERHOST_NOOP",			false,	false },
	  { "L_ERR_NOPRIVILEGES_PLUS_HELPER",		false,	false },
	  { "L_ERR_NOPRIVILEGES_PLUS_IRCOPER",		false,	false },
	  { "L_ERR_NOPRIVILEGES_PLUS_SERVNOTICES",	false,	false },
	  { "L_ERR_NORECIPIENT",			false,	false },
	  { "L_ERR_NOSUCHCHANNEL",			false,	false },
	  { "L_ERR_NOSUCHNICK",				false,	false },
	  { "L_ERR_NOSUCHNICK_OR_SERVER",		false,	false },
	  { "L_ERR_NOSUCHSERVER",			false,	false },
	  { "L_ERR_NOTEXTTOSEND",			false,	false },
	  { "L_ERR_NOTEXTTOSEND_NEEDMORE",		false,	false },
	  { "L_ERR_NOTONCHANNEL",			false,	false },
	  { "L_ERR_PASSWDMISMATCH",			false,	false },
	  { "L_ERR_SERVERMODELOCK",			false,	false },
	  { "L_ERR_SERVERTOOFULL",			false,	false },
	  { "L_ERR_SILELISTFULL",			false,	false },
	  { "L_ERR_TOOMANYCHANNELS",			false,	false },
	  { "L_ERR_UMODEUNKNOWNFLAG",			false,	false },
	  { "L_ERR_UNKNOWNCOMMAND",			false,	false },
	  { "L_ERR_USERSDONTMATCH",			false,	false },
	  { "L_ERR_WHOTRUNC",				false,	false },
	  { "L_RPL_ADMINME",				false,	false },
	  { "L_RPL_CHANREDIR",				false,	false },
	  { "L_RPL_ENDOFACCEPT",			false,	false },
	  { "L_RPL_ENDOFHELP",				false,	false },
	  { "L_RPL_ENDOFHELP_SIMPLE",			false,	false },
	  { "L_RPL_ENDOFINFO",				false,	false },
	  { "L_RPL_ENDOFLANGUAGES",			false,	false },
	  { "L_RPL_ENDOFLINKS",				false,	false },
	  { "L_RPL_ENDOFNAMES",				false,	false },
	  { "L_RPL_ENDOFSILELIST",			false,	false },
	  { "L_RPL_ENDOFSTATS",				false,	false },
	  { "L_RPL_ENDOFWATCHLIST",			false,	false },
	  { "L_RPL_ENDOFWHO",				false,	false },
	  { "L_RPL_ENDOFWHOIS",				false,	false },
	  { "L_RPL_ENDOFWHOWAS",			false,	false },
	  { "L_RPL_KILLDONE",				false,	false },
	  { "L_RPL_LISTEND",				false,	false },
	  { "L_RPL_MODECHANGEWARN_HELPER_PLUS_REGONLY",	false,	false },
	  { "L_RPL_MODECHANGEWARN_OPER_MINUS_WALLOPS",	false,	false },
	  { "L_RPL_MODECHANGEWARN_MINUS_VWORLD",	false,	false },
	  { "L_RPL_NOTOPIC",				false,	false },
	  { "L_RPL_NOWAWAY",				false,	false },
	  { "L_RPL_REHASHING",				false,	false },
	  { "L_RPL_SERVLISTEND",			false,	false },
	  { "L_RPL_SPAM_A",				false,	false },
	  { "L_RPL_SPAM_B",				false,	false },
	  { "L_RPL_SPAM_C",				false,	false },
	  { "L_RPL_SPAM_D",				false,	false },
	  { "L_RPL_TRYAGAIN_AWAY",			false,	false },
	  { "L_RPL_TRYAGAIN_KNOCK",			false,	false },
	  { "L_RPL_UNAWAY",				false,	false },
	  { "L_RPL_VERSION",				false,	false },
	  { "L_RPL_YOUREOPER",				false,	false },
	  { "L_RPL_YOUREOPER_ALREADY",			false,	false },
	  { "LANGCODE",					true,	true },
	  { "LANGNAME",					true,	false },
	  { "LANGNOTE",					false,	false },
	  { "MAINTAINER",				true,	true },
	  { "P_NO_MATCH",				false,	false },
	  { "P_RPL_WHOISVIRT",				false,	false },
	  { "REVISION",					true,	true },
	  { "W_LANGUAGE_PL",				false,	false },
	  { "W_MATCH",					false,	false },
	  { "W_MATCH_PL",				false,	false }
     };

   // The map with the languages in it..
   Language::languages_map_t languages;
   
   // Default language, start off with it being none...
   LanguageData *defaultLanguage = 0;
   
   // Language tag list for ISUPPORT
   String ISUPPORTcodes = "";
}

/* Do not change these lines unless you know how they are used/referenced or
 * integrate with the IRC protocol etc. Often white-space before/after a
 * string of text is there for a very specific reason.
 */

namespace Language {
   // Greeting lines sent when a user connects
   char const *L_PINGPONG_NOTICE =
     ":*** If you are having problems connecting due to ping timeouts, "
     "type /raw PONG %s or /quote PONG %s now. If you are still having "
     "trouble connecting, please e-mail help@austnet.org with the name "
     "and version of the client you are using, and the server you tried "
     "to connect to (%s).";
   char const *L_RPL_WELCOME =
     ":Welcome to the AustNet IRC Network %s -- For more information "
     "visit http://www.austnet.org";
   char const *L_RPL_YOURHOST =
     ":Your server is %s, running version %s";

   // Channel related messages
   char const *L_ERR_USERNOTINCHANNEL =
     "%s %s :They aren't on that channel";

   char const *L_ERR_CHANOPRIVSNEEDED =
     " :You're not a channel operator";
   char const *L_ERR_CHANOPRIVSNEEDED_HALFOPERVSOPER =
     " :You're not a full channel operator";
   char const *L_ERR_UNIQOPRIVSNEEDED =
     ":You're not the original channel operator";

   // Mode related messages
   char const *L_RPL_ENDOFBANLIST =
     " :End of channel ban list";
   char const *L_RPL_ENDOFEXCEPTLIST =
     " :End of channel ban exception list";
   char const *L_RPL_ENDOFINVITELIST =
     " :End of channel invite list";
   char const *L_ERR_UNKNOWNSERVERMODE =
     "%c :is an unknown server mode";
   char const *L_ERR_CANNOTCHANGECHANMODE =
     "%c :Cannot modify this channel mode";
   char const *L_ERR_CANNOTCHANGESERVERMODE =
     "%c :Cannot modify this server mode";
   char const *L_ERR_NOCHANMODES =
     " :Channel doesn't support modes";
   char const *L_ERR_KEYSET =
     " :Channel key already set";

   // MOTD (Message of the day) replies
   char const *L_RPL_MOTDSTART =
     ":- %s Message of the day -";
   char const *L_RPL_MOTD_SHORT1 =
     ":- Please read the MOTD by typing /motd";
   char const *L_RPL_MOTD_SHORT2 =
     ":- Connection to this server from this point onwards assumes that";
   char const *L_RPL_MOTD_SHORT3 =
     ":- you have read and agree to all terms and conditions in the MOTD";
   char const *L_RPL_MOTD_PREFIX =
     ":- ";
   char const *L_RPL_ENDOFMOTD =
     ":End of MOTD";
   char const *L_ERR_NOMOTD =
     ":MOTD File is missing";
   char const *L_ERR_NOMOTD_NOREMOTE =
     ":MOTD on %s cannot be viewed remotely";

   // WATCH command messages
   char const *L_ERR_TOOMANYWATCH =
     "%s :Maximum size for WATCH-list is %d entries";
   char const *L_RPL_WATCHSTAT =
     ":You have %d and are on %d WATCH entries";

   // LUSERS command replies
   char const *L_RPL_LUSERCLIENT =
     ":There are %d users and %d services on %d servers";
   char const *L_RPL_LUSERME =
     ":I have %u clients and %u servers";
   char const *L_RPL_LOCALUSERS =
     ":Current local users: %u Peak: %u";
   char const *L_RPL_GLOBALUSERS =
     ":Current global users: %u Peak: %u";

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

   char const *L_ERR_ISCHANSERVICE =
     "%s %s :AustNet services cannot be kicked or deoped";

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

   char const *L_RPL_STATSUPTIME =
     ":Server Up %d days %d:%02d:%02d";
   char const *L_RPL_STATSCONN =
     ":Highest connection count: %u (%u clients)";

   char const *L_ERR_NOSUCHNICK_NICK =
     " :No such nickname";

   char const *L_DEFAULT_QUIT_MESSAGE =
     "Exit: Austnet Chat Network";
};


/* loadLanguages - Seek and load language files in the given directory
 * Original 28/10/01, Simon Butcher <pickle@austnet.org>
 */
bool Language::loadLanguages(String const &directory, 
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
      debug(String("Error opening language file direcotry: ") +
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
	 LanguageData *lang = new LanguageData();
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
	    unsigned int tn = Language::_END_;
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
	 for (int i = 0; i < Language::_END_; i++) {
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
	 languages[lang->get(LANGCODE)] = lang;

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
LanguageData *Language::get(String const &code)
{
   // Find the language
   LanguageData *ld = languages[code];
   
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
String Language::lang(Language::tag_t const &t)
{
   return (defaultLanguage ? 
	   defaultLanguage->get(t) : 
	   String("*"));
};
