/* language.h
 * Text in a centralised location so it can be easily changed/fixed etc
 */

#ifndef __LANGUAGE_H_
# define __LANGUAGE_H_

# ifdef STL_HAS_HASH
#  include <hash_map>
# else
#  include <map>
# endif

# include <vector>

# include "str.h"


class LanguageData;


// Main language class
class Language {
 public:
# ifdef STL_HAS_HASH
   typedef hash_map <String, LanguageData *> languages_map_t;
# else
   typedef hash_map <String, LanguageData *> languages_map_t;
# endif
   
   /* Corresponding numbers for the language data vector
    * WARNING!! Make sure this list is in perfect sync with the language tag
    * list in language.cpp!!
    */
   enum tag_t {
	B_RPL_CREATED,
	B_RPL_TIMEONSERVERIS,
	CHARSET,
	E_ERR_ACCEPTEXIST,
	E_ERR_ACCEPTNOT,
	E_ERR_NOLANGUAGE,
	E_ERR_UNKNOWNMODE,
	E_ERR_USERONCHANNEL,
	E_RPL_ACCEPTED,
	E_RPL_ACCEPTNOTICE,
	E_RPL_ISUPPORT,
	E_RPL_KNOCKING,
	E_RPL_LOGOFF_CHANNEL,
	E_RPL_LOGOFF_SERVER,
	E_RPL_LOGOFF_USER,
	E_RPL_LOGON_CHANNEL,
	E_RPL_LOGON_SERVER,
	E_RPL_LOGON_USER,
	E_RPL_LUSERCHANNELS,
	E_RPL_LUSERHELPERS,
	E_RPL_LUSEROP,
	E_RPL_LUSERUNKNOWN,
	E_RPL_NOTACCEPTED,
	E_RPL_NOWOFF,
	E_RPL_NOWON_CHANNEL,
	E_RPL_NOWON_SERVER,
	E_RPL_NOWON_USER,
	E_RPL_WATCHOFF,
	E_RPL_WHOISIDLE,
	E_RPL_WHOISOPERATOR,
	E_RPL_WHOISSECURE,
	L_DEFAULT_KNOCK_REASON,
	L_ERR_ACCEPTFULL,
	L_ERR_ALREADYREGISTERED,
	L_ERR_BADCHANNELKEY,
	L_ERR_BADCHANNAME,
	L_ERR_BADCHANNAME_FAILMASK,
	L_ERR_BADCHANNAME_SAFECHAN,
	L_ERR_BANNEDFROMCHAN,
	L_ERR_CANNOTSENDTOCHAN_BANNED,
	L_ERR_CANNOTSENDTOCHAN_MODERATED,
	L_ERR_CANNOTSENDTOCHAN_NOOUTSIDEMSG,
	L_ERR_CANNOTSENDTOCHAN_REGNICKSONLY,
	L_ERR_CANNOTSENDTONICK_REGNICKSONLY,
	L_ERR_CANNOTCHANGEUMODE,
	L_ERR_CANTKILLSERVICES,
	L_ERR_CHANNELISFULL,
	L_ERR_ERRONEUSNICKNAME,
	L_ERR_EVENTNICKCHANGE,
	L_ERR_INVITEONLYCHAN,
	L_ERR_NEEDMOREPARAMS,
	L_ERR_NICKNAMEINUSE,
	L_ERR_NOKNOCK_INVITED,
	L_ERR_NOKNOCK_ONCHANNEL,
	L_ERR_NOKNOCK_OPENCHANNEL,
	L_ERR_NOKNOCK_REGONLY,
	L_ERR_NOMORELANGS,
	L_ERR_NONICKNAMEGIVEN,
	L_ERR_NONONREG_CHANNEL,
	L_ERR_NOOPERHOST,
	L_ERR_NOOPERHOST_NOOP,
	L_ERR_NOPRIVILEGES_PLUS_HELPER,
	L_ERR_NOPRIVILEGES_PLUS_IRCOPER,
	L_ERR_NOPRIVILEGES_PLUS_SERVNOTICES,
	L_ERR_NORECIPIENT,
	L_ERR_NOSUCHCHANNEL,
	L_ERR_NOSUCHNICK,
	L_ERR_NOSUCHNICK_OR_SERVER,
	L_ERR_NOSUCHSERVER,
	L_ERR_NOTEXTTOSEND,
	L_ERR_NOTEXTTOSEND_NEEDMORE,
	L_ERR_NOTONCHANNEL,
	L_ERR_PASSWDMISMATCH,
	L_ERR_SERVERMODELOCK,
	L_ERR_SERVERTOOFULL,
	L_ERR_SILELISTFULL,
	L_ERR_TOOMANYCHANNELS,
	L_ERR_UMODEUNKNOWNFLAG,
	L_ERR_UNKNOWNCOMMAND,
	L_ERR_USERSDONTMATCH_MODE,
	L_ERR_WHOTRUNC,
	L_RPL_ADMINME,
	L_RPL_CHANREDIR,
	L_RPL_ENDOFACCEPT,
	L_RPL_ENDOFHELP,
	L_RPL_ENDOFHELP_SIMPLE,
	L_RPL_ENDOFINFO,
	L_RPL_ENDOFLANGUAGES,
	L_RPL_ENDOFLINKS,
	L_RPL_ENDOFNAMES,
	L_RPL_ENDOFSILELIST,
	L_RPL_ENDOFSTATS,
	L_RPL_ENDOFWATCHLIST,
	L_RPL_ENDOFWHO,
	L_RPL_ENDOFWHOIS,
	L_RPL_ENDOFWHOWAS,
	L_RPL_KILLDONE,
	L_RPL_LISTEND,
	L_RPL_MODECHANGEWARN_HELPER_PLUS_REGONLY,
	L_RPL_MODECHANGEWARN_OPER_MINUS_WALLOPS,
	L_RPL_MODECHANGEWARN_MINUS_VWORLD,
	L_RPL_NOTOPIC,
	L_RPL_NOWAWAY,
	L_RPL_REHASHING,
	L_RPL_SERVLISTEND,
	L_RPL_SPAM_A,
	L_RPL_SPAM_B,
	L_RPL_SPAM_C,
	L_RPL_SPAM_D,
	L_RPL_TRYAGAIN_AWAY,
	L_RPL_TRYAGAIN_KNOCK,
	L_RPL_UNAWAY,
	L_RPL_VERSION,
	L_RPL_YOUREOPER,
	L_RPL_YOUREOPER_ALREADY,
	LANGCODE,
	LANGNAME,
	LANGNOTE,
	MAINTAINER,
	P_NO_MATCH,
	P_RPL_WHOISVIRT,
        REVISION,
	W_LANGUAGE_PL,
	W_MATCH,
	W_MATCH_PL,
	_END_
   };
   // 'Spam' notification, rather server policy that demands to be read
   static char const *L_RPL_SPAM_LINE1;
   static char const *L_RPL_SPAM_LINE2;
   static char const *L_RPL_SPAM_LINE3;
   static char const *L_RPL_SPAM_LINE4;
   static char const *L_RPL_SPAM_LINE5;
   static char const *L_RPL_SPAM_LINE6;
   
   
// private:
 public:
   // List of language tags (for the parser)
   struct languageTagsStruct {
      char const *name;
      bool const required;
      bool const oneword;
   };
   static languageTagsStruct languageTags[Language::_END_];
   static languages_map_t languages;		// Installed languages
   static LanguageData *defaultLanguage;	// Default language
   static String ISUPPORTcodes;			// ISUPPORT LANGUAGE= data
   
   Language(void) {};				// Constructor (cannot be run)
   virtual ~Language(void) {};			// Destructor
   
//   static void clear(void);			// Wipe the language lists

 public:
   static bool loadLanguages(String const &,
			     String const &);	// Load language files
   
   static LanguageData *get(String const &);	// Find language dialogue data
   
   // Grab the data for the LANGUAGE= 'ISUPPORT' tag
   static String getISUPPORTcodes(void)
     {
	return ISUPPORTcodes;
     };
   
   // Return language dialogue from the default language, if set
   static String lang(Language::tag_t const &);
   
   // Greeting lines sent when a user connects
   static char const *L_PINGPONG_NOTICE;
   static char const *L_RPL_WELCOME;
   static char const *L_RPL_YOURHOST;


   // Channel related messages
   static char const *L_ERR_USERNOTINCHANNEL;
   static char const *L_ERR_CHANOPRIVSNEEDED;
   static char const *L_ERR_CHANOPRIVSNEEDED_HALFOPERVSOPER;
   static char const *L_ERR_UNIQOPRIVSNEEDED;

   
   // Mode related messages
   static char const *L_RPL_ENDOFBANLIST;
   static char const *L_RPL_ENDOFEXCEPTLIST;
   static char const *L_RPL_ENDOFINVITELIST;
   static char const *L_ERR_UNKNOWNSERVERMODE;
   static char const *L_ERR_CANNOTCHANGECHANMODE;
   static char const *L_ERR_CANNOTCHANGESERVERMODE;
   static char const *L_ERR_NOCHANMODES;
   static char const *L_ERR_KEYSET;
   
   // MOTD (Message of the day) replies
   static char const *L_RPL_MOTDSTART;
   static char const *L_RPL_MOTD_SHORT1;
   static char const *L_RPL_MOTD_SHORT2;
   static char const *L_RPL_MOTD_SHORT3;
   static char const *L_RPL_MOTD_PREFIX;
   static char const *L_RPL_ENDOFMOTD;
   static char const *L_ERR_NOMOTD;
   static char const *L_ERR_NOMOTD_NOREMOTE;
   
   // WATCH command messages
   static char const *L_ERR_TOOMANYWATCH;
   static char const *L_RPL_WATCHSTAT;

   // LUSERS command replies
   static char const *L_RPL_LUSERCLIENT;
   static char const *L_RPL_LUSERME;
   static char const *L_RPL_LOCALUSERS;
   static char const *L_RPL_GLOBALUSERS;

   // Server notice output strings
   static char const *L_SERVNOTICE_LINK;
   static char const *L_SERVNOTICE_DELINK;
   static char const *L_SERVNOTICE_SIGNON; 
   static char const *L_SERVNOTICE_SIGNOFF;
   static char const *L_SERVNOTICE_RECV_EOB;
   static char const *L_SERVNOTICE_CMD_DIE;
   static char const *L_SERVNOTICE_CMD_REHASH;
   static char const *L_SERVNOTICE_GARBO;
   static char const *L_SERVNOTICE_GARBO_ITEMS;
   static char const *L_SERVNOTICE_GLOBOPS;
   static char const *L_SERVNOTICE_HELPME;
   static char const *L_SERVNOTICE_LOCOPS;
   static char const *L_SERVNOTICE_NOTIFY_ON_CMD_STATS;
   
   // Misc/Disorganised
   static char const *L_NOTIFY_PARANOID_OPERS_ON_WHOIS;
   static char const *L_ERR_ISCHANSERVICE;
   static char const *L_QUIT_KILLED;
   static char const *L_REQUESTED_SHUTDOWN;
   static char const *L_BYE_BYE_USER;
   static char const *L_ERROR_CLOSING_LINK;
   static char const *L_ERROR_CLOSING_LINK_DEFAULT_REASON;

   static char const *L_RPL_STATSUPTIME;
   static char const *L_RPL_STATSCONN;
   static char const *L_ERR_NOSUCHNICK_NICK;
   static char const *L_DEFAULT_QUIT_MESSAGE;
   
   friend class LanguageData;
};


// Language data class
class LanguageData {
 private:
   vector <String> dialogue;
   
 public:
   // Constructor
   LanguageData(void)
     {
	dialogue.clear();
     };
   
   // Destructor
   ~LanguageData(void)
     {
	dialogue.clear();
     };

   // Grab a string from the language dialogue data
   String get(Language::tag_t const &n) const
     { 
	return dialogue[n]; 
     };

   // Check if something exists in the dialogue
   bool has(Language::tag_t const &n) const
     {
	return (dialogue[n].length() > 0);
     };
   
   friend class Language;
};

#endif

