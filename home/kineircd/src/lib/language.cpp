/* language.cpp
 * Text in a centralised location so it can be easily changed/fixed etc
 */

#include "config.h"

#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <iostream.h>
#include <fstream.h>
#include <errno.h>

#include "language.h"
#include "daemon.h"


namespace Language {
   /* This is a list of language tags so that the language parser knows 
    * where to shove the language data.
    * WARNING!!! Make sure this list is in perfect sync with the enumeration 
    * list in the language header file!
    */
   Language::languageTagsStruct languageTags[NUM_LANG_TAGS] =
     {
	{ "REVISION",			true,	true },
	{ "MAINTAINER",			true,	true },
	{ "LANGNAME",			true,	false },
	{ "LANGNOTE",			false,	false },
	{ "CHARSET",			true,	true }
     };

   // The map with the languages in it..
   Language::languages_map_t languages;
   
   // Default language, start off with it being none...
   LanguageData *defaultLanguage = 0;
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
   char const *L_RPL_YOURHOST_IRCII_KLUGE_NOTICE =
     "NOTICE %s :*** Your host is %s, running version %s";
   char const *L_RPL_CREATED =
     ":This server was created %s";
   char const *L_RPL_ISUPPORT_TAG =
     " :are supported by this server";
   char const *L_RPL_TIMEONSERVERIS =
     "%lu %ld %s %s :time according to server";

   // 'Spam' notification, rather server policy that demands to be read
   char const *L_RPL_SPAM_LINE1 =
     ": ";
   char const *L_RPL_SPAM_LINE2 =
     ": Please be advised that use of this service constitutes consent to";
   char const *L_RPL_SPAM_LINE3 =
     ":  all network policies and server conditions of use, which can be";
   char const *L_RPL_SPAM_LINE4 =
     ": sighted at http://www.austnet.org/agreement/ and is in accordance";
   char const *L_RPL_SPAM_LINE5 =
     ": with the Electronic Communications Privacy Act,  18 USC 2701-2711";
   char const *L_RPL_SPAM_LINE6 =
     ": ";

   // Generic responces
   char const *L_ERR_UNKNOWNCOMMAND =
     " :Unknown command";
   char const *L_ERR_PASSWDMISMATCH =
     ":Password incorrect";
   char const *L_ERR_SERVERTOOFULL =
     ":This server cannot accept any more user connections";
   char const *L_ERR_NEEDMOREPARAMS =
     " :Not enough parameters";
   char const *L_ERR_ALREADYREGISTERED =
     ":You may only claim registration type once";
   char const *L_ERR_SUMMONDISABLED =
     ":SUMMON has been disabled";
   char const *L_ERR_USERSDISABLED =
     ":USERS has been disabled";
   char const *L_ERR_NOPRIVILEGES =
     ":Permission Denied - You're not an AustNet IRC operator";

   // Nickname related errors
   char const *L_ERR_NONICKNAMEGIVEN =
     ":No nickname given";
   char const *L_ERR_ERRONEUSNICKNAME =
     " :Erroneous nickname";
   char const *L_ERR_ERRONEUSNICKNAME_W_REASON =
     "%s :Bad nickname specified: %s";
   char const *L_ERR_NICKNAMEINUSE =
     " :Nickname is already in use";
   char const *L_ERR_EVENTNICKCHANGE =
     " :Sorry, cannot change nickname during a channel event (+E)";

   // Channel related messages
   char const *L_ERR_BADCHANNAME =
     " :Bad channel name";
   char const *L_ERR_BADCHANNAME_SAFECHAN =
     " :Safe channels must be created first";
   char const *L_ERR_BADCHANNAME_FAILMASK =
     "%s :Channel name is unavailable: %s";

   char const *L_ERR_NOTONCHANNEL =
     " :You're not on that channel";
   char const *L_ERR_USERNOTINCHANNEL =
     "%s %s :They aren't on that channel";

   char const *L_ERR_TOOMANYCHANNELS =
     " :You have joined too many channels";
   char const *L_ERR_BANNEDFROMCHAN =
     " :Cannot join channel (+b)";
   char const *L_ERR_CHANOPRIVSNEEDED =
     " :You're not a channel operator";
   char const *L_ERR_CHANOPRIVSNEEDED_HALFOPERVSOPER =
     " :You're not a full channel operator";
   char const *L_ERR_UNIQOPRIVSNEEDED =
     ":You're not the original channel operator";
   char const *L_ERR_NOSUCHCHANNEL =
     " :No such channel";

   char const *L_ERR_INVITEONLYCHAN =
     " :Cannot join channel (+i)";
   char const *L_ERR_USERONCHANNEL =
     "%s %s :is already on channel";
   char const *L_ERR_CHANNELISFULL =
     " :Cannot join channel (+l)";
   char const *L_ERR_BADCHANNELKEY =
     " :Cannot join channel (+k)";

   char const *L_ERR_CANNOTSENDTOCHAN =
     " :Cannot send to channel";
   char const *L_ERR_CANNOTSENDTOCHAN_NOOUTSIDEMSG =
     " :Cannot send to channel, you are not on that channel (+n)";
   char const *L_ERR_CANNOTSENDTOCHAN_BANNED =
     " :Cannot send to channel, you are banned (+b)";
   char const *L_ERR_CANNOTSENDTOCHAN_MODERATED =
     " :Cannot send to channel, channel is moderated (+m)";
   char const *L_ERR_CANNOTSENDTOCHAN_REGNICKSONLY =
     " :Cannot send to channel, you need to be identified (+R)";

   char const *L_RPL_CHANREDIR =
     "%s %s :You are being redirected";
   char const *L_RPL_NOTOPIC =
     " :No topic is set";

   // Mode related messages
   char const *L_RPL_ENDOFBANLIST =
     " :End of channel ban list";
   char const *L_RPL_ENDOFEXCEPTLIST =
     " :End of channel ban exception list";
   char const *L_RPL_ENDOFINVITELIST =
     " :End of channel invite list";
   char const *L_ERR_USERSDONTMATCH_MODE =
     ":Cannot change mode for other users";
   char const *L_ERR_UMODEUNKNOWNFLAG =
     " :Unknown user MODE flag";
   char const *L_ERR_UNKNOWNMODE =
     "%c :is an unknown mode char to me for %s";
   char const *L_ERR_UNKNOWNSERVERMODE =
     "%c :is an unknown server mode";
   char const *L_ERR_NOPRIVILEGES_PLUS_IRCOPER =
     ":Permission Denied - Use the OPER command instead";
   char const *L_ERR_NOPRIVILEGES_PLUS_HELPER =
     ":Permission Denied - Identify yourself to services";
   char const *L_ERR_NOPRIVILEGES_PLUS_SERVNOTICES =
     ":Permission Denied - Only network operators are allowed to see "
     "server notices";
   char const *L_ERR_SERVERMODELOCK =
     " :Cannot change server modes (+L)";
   char const *L_RPL_MODECHANGEWARN_MINUS_VWORLD =
     "-v :Warning - You have changed your mode to -v (Virtual World). "
     "You are now vulnerable to Denial of Service attacks.";
   char const *L_RPL_MODECHANGEWARN_IRCOPER_MINUS_WALLOPS =
     "-w :You may miss important messages by disabling the receive "
     "wallops mode";
   char const *L_RPL_MODECHANGEWARN_HELPER_PLUS_REGNICKSMSG =
     "+R :Setting +R (Only registered nicks can send to you) is not "
     "recommended for helpers";
   char const *L_ERR_CANNOTCHANGEUMODE =
     "%c :Cannot toggle this user mode";
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

   // WHO messages
   char const *L_ERR_WHOTRUNC =
     ":Who list too long - Truncated.";
   char const *L_RPL_ENDOFWHO =
     "%s :End of WHO list";
#ifdef DO_MATCH_COUNTING
   char const *L_RPL_ENDOFWHO_NOMATCH =
     "%s :End of WHO list (no match)";
   char const *L_RPL_ENDOFWHO_MATCHES =
     "%s :End of WHO list (%d matches)";
#endif

   // WHOIS messages
   char const *L_RPL_WHOISVIRT =
     " :on Virtual World as ";
   char const *L_RPL_WHOISIDLE =
     " %lu %lu :seconds idle, signon time";
   char const *L_RPL_WHOISOPERATOR =
     " :is an AustNet IRC Operator";
   char const *L_RPL_WHOISSECURE =
     " :is connected via a secure connection";
   char const *L_RPL_ENDOFWHOIS =
     " :End of WHOIS list";

   // WHOWAS messages
   char const *L_RPL_ENDOFWHOWAS =
     "%s :End of WHOWAS";
#ifdef DO_MATCH_COUNTING
   char const *L_RPL_ENDOFWHOWAS_NOMATCH =
     "%s :End of WHOWAS (no match, or matches have expired)";
   char const *L_RPL_ENDOFWHOWAS_MATCHES =
     "%s :End of WHOWAS (%d matches)";
#endif

   // HELP command replies
   char const *L_RPL_ENDOFHELP =
     "%s :End of HELP";
#ifdef DO_MATCH_COUNTING
   char const *L_RPL_ENDOFHELP_NOMATCH =
     "%s :End of HELP (no match)";
   char const *L_RPL_ENDOFHELP_MATCHES =
     "%s :End of HELP (%d matches)";
#endif
   char const *L_RPL_ENDOFHELP_SIMPLE =
     "%s :End of HELP; Try 'HELP -%s' for more information";
#ifdef DO_MATCH_COUNTING
   char const *L_RPL_ENDOFHELP_SIMPLE_MATCHES =
     "%s :End of HELP (%d matches); Try 'HELP -%s' for more information";
#endif

   // LANGUAGE command messages
#ifdef DO_MATCH_COUNTING
   char const *L_RPL_ENDOFLANGUAGES =
     ":End of LANGUAGES list (%d language(s))";
#else
   char const *L_RPL_ENDOFLANGUAGES =
     ":End of LANGUAGES list";
#endif
   char const *L_ERR_NOLANGUAGE =
     " :is an unknown language to me";
   char const *L_ERR_NOMORELANGS =
     "%u :No more languages can be set";
   
   // AWAY command messages
   char const *L_RPL_NOWAWAY =
     ":You have been marked as being away";
   char const *L_RPL_UNAWAY =
     ":You are no longer marked as being away";

   // WATCH command messages
   char const *L_ERR_TOOMANYWATCH =
     "%s :Maximum size for WATCH-list is %d entries";
   char const *L_RPL_WATCHSTAT =
     ":You have %d and are on %d WATCH entries";
   char const *L_RPL_ENDOFWATCHLIST =
     ":End of WATCH %c";

   char const *L_RPL_LOGOFF_SERVER =
     ":has net-split";
   char const *L_RPL_LOGOFF_CHANNEL =
     ":has disappeared";
   char const *L_RPL_LOGOFF_USER =
     ":has logged off";

   char const *L_RPL_LOGON_SERVER =
     ":has net-joined";
   char const *L_RPL_LOGON_CHANNEL =
     ":has been created";
   char const *L_RPL_LOGON_USER =
     ":has logged on";

   char const *L_RPL_NOWON_SERVER =
     ":is connected";
   char const *L_RPL_NOWON_CHANNEL =
     ":exists";
   char const *L_RPL_NOWON_USER =
     ":is online";

   char const *L_RPL_NOWOFF =
     ":is offline";
   char const *L_RPL_WATCHOFF =
     ":stopped watching";

   // ACCEPT command replies
   char const *L_RPL_ENDOFACCEPT =
     ":End of ACCEPT list";
   char const *L_ERR_ACCEPTFULL =
     ":Accept list is full";
   char const *L_ERR_ACCEPTEXIST =
     " :already exists";
   char const *L_ERR_ACCEPTNOT =
     " :doesn't exist";
   char const *L_RPL_NOTACCEPTED =
     " :is in server side ignore mode (+g) but has been notified you wish to "
     "talk to them";
   char const *L_RPL_ACCEPTNOTICE =
     "%s %s %s :is messaging you. You are in server side ignore mode (+g). "
     "Use the ACCEPT command if you want to talk to them";
   char const *L_RPL_ACCEPTED =
     "%s %s %s :has put you on their ACCEPT list";

   // SERVLIST replies
   char const *L_RPL_SERVLISTEND =
     " :End of services listing";
   
   // SILENCE list replies
   char const *L_RPL_ENDOFSILELIST =
     " :End of SILENCE list";
   char const *L_ERR_SILELISTFULL =
     " :Your silence list is full";

   // KNOCK replies
   char const *L_ERR_NOKNOCK_ONCHANNEL =
     " :You are already on this channel";
   char const *L_ERR_NOKNOCK_OPENCHANNEL =
     " :Channel is not invitation only (-i)";
   char const *L_ERR_NOKNOCK_INVITED =
     " :You have already been invited to this channel";
   char const *L_ERR_NOKNOCK_REGONLY =
     " :You have not identified yourself, and this channel is for registered "
     "users only (+R)";
   char const *L_RPL_KNOCKING =
     " :has received your KNOCKing";
   char const *L_DEFAULT_KNOCK_REASON =
     "This user is requesting to be invited to the channel";
   
   // LUSERS command replies
   char const *L_RPL_LUSERCLIENT =
     ":There are %d users and %d services on %d servers";
   char const *L_RPL_LUSEROP =
     "%u :operator(s) online";
   char const *L_RPL_LUSERHELPERS =
     "%u :helper(s) online";
   char const *L_RPL_LUSERUNKNOWN =
     "%u :unknown connection(s)";
   char const *L_RPL_LUSERCHANNELS =
     "%u :channels formed";
   char const *L_RPL_LUSERME =
     ":I have %u clients and %u servers";
   char const *L_RPL_LOCALUSERS =
     ":Current local users: %u Peak: %u";
   char const *L_RPL_GLOBALUSERS =
     ":Current global users: %u Peak: %u";

   // Command not processed messages
   char const *L_RPL_TRYAGAIN_AWAY =
     "AWAY :You have set your away message too quickly, please try again soon";
   char const *L_RPL_TRYAGAIN_KNOCK =
     "KNOCK :You have KNOCKed too quickly! Please wait and knock again soon";

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

   char const *L_ERR_NOSUCHSERVER =
     " :No such server";

   char const *L_ERR_ISCHANSERVICE =
     "%s %s :AustNet services cannot be kicked or deoped";
   char const *L_ERR_CANTKILLSERVICES =
     ":You are not allowed to kill Austnet Services!";

   char const *L_QUIT_KILLED =
     "Killed (%s (%s))";
   char const *L_RPL_KILLDONE =
     " :Killed.";

   char const *L_ERR_NORECIPIENT =
     ":No recipient given ";
   char const *L_ERR_NOTEXTTOSEND =
     ":No text to send";
# ifdef MINLEN_OP_BROADCAST
   char const *L_ERR_NOTEXTTOSEND_NEEDMORE =
     ":No text to send, or a larger message is required for this command";
# endif

   char const *L_REQUESTED_SHUTDOWN =
     " requested shutdown";
   char const *L_BYE_BYE_USER =
     "Exit: %s";
   char const *L_ERROR_CLOSING_LINK =
     "ERROR :Closing Link: %s\r\n";
   char const *L_ERROR_CLOSING_LINK_DEFAULT_REASON =
     "Unknown error";

   char const *L_RPL_ENDOFINFO =
     ":End of INFO";
   char const *L_RPL_LISTEND =
     ":End of LIST";
   char const *L_RPL_ENDOFNAMES =
     "%s :End of NAMES list";
#ifdef DO_MATCH_COUNTING
   char const *L_RPL_ENDOFNAMES_NOMATCH =
     "%s :End of NAMES list (no match)";
   char const *L_RPL_ENDOFNAMES_MATCHES =
     "%s :End of NAMES list (%d matches)";
#endif
   char const *L_RPL_ENDOFSTATS =
     " :End of STATS report";

   char const *L_RPL_VERSION =
     "%s %s :%s (Type /INFO for more details)";

   char const *L_RPL_ADMINME =
     " :Administration information";

   char const *L_RPL_STATSUPTIME =
     ":Server Up %d days %d:%02d:%02d";
   char const *L_RPL_STATSCONN =
     ":Highest connection count: %u (%u clients)";

   char const *L_ERR_NONONREG_CHANNEL =
     " :You must be identified with NickOP to join this channel";
   char const *L_ERR_NONONREG_USER =
     " :You must be identified with NickOP to talk privately with this user";

   char const *L_ERR_NOSUCHNICK =
     " :No such nickname or channel";
   char const *L_ERR_NOSUCHNICK_NICK =
     " :No such nickname";
   char const *L_ERR_NOSUCHNICK_CHANNEL =
     " :No such channel";
   char const *L_ERR_NOSUCHNICK_OR_SERVER =
     " :No such nickname, channel or server";

   char const *L_ERR_CANNOTSENDTONICK_MUSTID =
     " :You must be identified to send to this person";

   char const *L_DEFAULT_QUIT_MESSAGE =
     "Exit: Austnet Chat Network";

   char const *L_ERR_NOOPERHOST =
     ":Access Denied: No operator details configured for your host";
   char const *L_ERR_NOOPERHOST_NOOP =
     ":Access Denied: This server is in NOOP mode.";
   char const *L_RPL_YOUREOPER =
     ":You are now an unpaid network babysitter";
   char const *L_RPL_YOUREOPER_ALREADY =
     ":You're ALREADY an IRC Operator!";
   char const *L_RPL_REHASHING =
     " :Rehashing";

   char const *L_RPL_ENDOFLINKS =
     " :End of LINKS list";
};


/* loadLanguages - Seek and load language files in the given directory
 * Original 28/10/01, Simon Butcher <pickle@austnet.org>
 */
bool Language::loadLanguages(String const &directory)
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
      return 1;
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
	 
	 // Grab the language code from the filename
	 String code = fileName.subString(LANG_FILE_PREFIX_LEN).toLower();

#ifdef DEBUG_EXTENDED
	 debug(String::printf("Loading language file %s (%s)",
			      (char const *)fileName,
			      (char const *)code));
#endif
	 
	 // Reset some variables
	 LanguageData *lang = new LanguageData(code);
	 langData.clear();
	 line = "";
	 tag = "";
	 data = "";
	 lineNum = 0;
	 
	 while (!file.eof()) {
	    // Reset some more variables
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
	    unsigned int tn = NUM_LANG_TAGS;
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
	    debug(String::printf(" +-> Tag: '%s', data: '%s'",
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
	 for (int i = 0; i < NUM_LANG_TAGS; i++) {
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
       	 languages[code] = lang;

	 // Clean out the map
	 String *foo;
	 while (!langData.empty()) {
	    foo = (*langData.begin()).second;
	    langData.erase(langData.begin());
	    delete foo;
	 }
      }
   }
   
   // OK!
   return 0;
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
