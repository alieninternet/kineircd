/* language.h
 * Text in a centralised location so it can be easily changed/fixed etc
 */

#ifndef __LANGUAGE_H_
# define __LANGUAGE_H_

/* Do not change these lines unless you know how they are used/referenced or
 * integrate with the IRC protocol etc. Often white-space before/after a
 * string of text is there for a very specific reason.
 */
# define LNG_RPL_YOURHOST_IRCII_KLUGE_NOTICE \
   "NOTICE %s :*** Your host is %s, running version %s"
# define LNG_ERR_NEEDMOREPARAMS \
   " :Not enough parameters"
# define LNG_SERVNOTICE_LINK \
   "Link with %s established"
# define LNG_SERVNOTICE_DELINK \
   "Link with %s cancelled: %s"
# define LNG_SERVNOTICE_HELPME \
   "*** Help -- from %s: %s"
# define LNG_ERR_NOTEXTTOSEND \
   ":No text to send"
# ifdef MINLEN_OP_BROADCAST
#  define LNG_ERR_NOTEXTTOSEND_NEEDMORE \
   ":No text to send, or a larger message is required for this command"
# endif
# define LNG_RPL_LOGOFF_SERVER \
   ":has net-split"
# define LNG_RPL_LOGOFF_CHANNEL \
   ":has disappeared"
# define LNG_RPL_LOGOFF_USER \
   ":has logged off"
# define LNG_RPL_LOGON_SERVER \
   ":has net-joined"
# define LNG_RPL_LOGON_CHANNEL \
   ":has been created"
# define LNG_RPL_LOGON_USER \
   ":has logged on"
# define LNG_RPL_NOWON_SERVER \
   ":is connected"
# define LNG_RPL_NOWON_CHANNEL \
   ":exists"
# define LNG_RPL_NOWON_USER \
   ":is online"
# define LNG_RPL_NOWOFF \
   ":is offline"
# define LNG_RPL_WATCHOFF \
   ":stopped watching"
# define LNG_ERROR_CLOSING_LINK \
   "ERROR :Closing Link: %s"


class Language {
 private:
   // This is here because this class CANNOT be constructed
   Language(void) {};
   
 public:
   // Greeting lines sent when a user connects
   static char const *L_PINGPONG_NOTICE;
   static char const *L_RPL_WELCOME;
   static char const *L_RPL_YOURHOST;
   static char const *L_RPL_YOURHOST_IRCII_KLUGE_NOTICE;
   static char const *L_RPL_CREATED;
   static char const *L_RPL_ISUPPORT_TAG;
   static char const *L_RPL_TIMEONSERVERIS;

   // 'Spam' notification, rather server policy that demands to be read
   static char const *L_RPL_SPAM_LINE1;
   static char const *L_RPL_SPAM_LINE2;
   static char const *L_RPL_SPAM_LINE3;
   static char const *L_RPL_SPAM_LINE4;
   static char const *L_RPL_SPAM_LINE5;
   static char const *L_RPL_SPAM_LINE6;
   
   // Generic responces
   static char const *L_ERR_UNKNOWNCOMMAND;
   static char const *L_ERR_PASSWDMISMATCH;
   static char const *L_ERR_SERVERTOOFULL;
   static char const *L_ERR_NEEDMOREPARAMS;
   static char const *L_ERR_ALREADYREGISTERED;
   static char const *L_ERR_SUMMONDISABLED;
   static char const *L_ERR_USERSDISABLED;
   static char const *L_ERR_NOPRIVILEGES;
   
   // Nickname related errors
   static char const *L_ERR_NONICKNAMEGIVEN;
   static char const *L_ERR_ERRONEUSNICKNAME;
   static char const *L_ERR_ERRONEUSNICKNAME_W_REASON;
   static char const *L_ERR_NICKNAMEINUSE;
   static char const *L_ERR_EVENTNICKCHANGE;

   
   // Channel related messages
   static char const *L_ERR_BADCHANNAME;
   static char const *L_ERR_BADCHANNAME_SAFECHAN;
   static char const *L_ERR_BADCHANNAME_FAILMASK;
   static char const *L_ERR_NOTONCHANNEL;
   static char const *L_ERR_USERNOTINCHANNEL;
   static char const *L_ERR_TOOMANYCHANNELS;
   static char const *L_ERR_BANNEDFROMCHAN;
   static char const *L_ERR_CHANOPRIVSNEEDED;
   static char const *L_ERR_CHANOPRIVSNEEDED_HALFOPERVSOPER;
   static char const *L_ERR_UNIQOPRIVSNEEDED;
   static char const *L_ERR_NOSUCHCHANNEL;
   static char const *L_ERR_INVITEONLYCHAN;
   static char const *L_ERR_USERONCHANNEL;
   static char const *L_ERR_CHANNELISFULL;
   static char const *L_ERR_BADCHANNELKEY;
   static char const *L_ERR_CANNOTSENDTOCHAN;
   static char const *L_ERR_CANNOTSENDTOCHAN_NOOUTSIDEMSG;
   static char const *L_ERR_CANNOTSENDTOCHAN_BANNED;
   static char const *L_ERR_CANNOTSENDTOCHAN_MODERATED;
   static char const *L_ERR_CANNOTSENDTOCHAN_REGNICKSONLY;
   static char const *L_RPL_CHANREDIR;
   static char const *L_RPL_NOTOPIC;
   
   // Mode related messages
   static char const *L_RPL_ENDOFBANLIST;
   static char const *L_RPL_ENDOFEXCEPTLIST;
   static char const *L_RPL_ENDOFINVITELIST;
   static char const *L_ERR_USERSDONTMATCH_MODE;
   static char const *L_ERR_UMODEUNKNOWNFLAG;
   static char const *L_ERR_UNKNOWNMODE;
   static char const *L_ERR_UNKNOWNSERVERMODE;
   static char const *L_ERR_NOPRIVILEGES_PLUS_IRCOPER;
   static char const *L_ERR_NOPRIVILEGES_PLUS_HELPER;
   static char const *L_ERR_NOPRIVILEGES_PLUS_SERVNOTICES;
   static char const *L_ERR_SERVERMODELOCK;
   static char const *L_RPL_MODECHANGEWARN_MINUS_VWORLD;
   static char const *L_RPL_MODECHANGEWARN_IRCOPER_MINUS_WALLOPS;
   static char const *L_RPL_MODECHANGEWARN_HELPER_PLUS_REGNICKSMSG;
   static char const *L_ERR_CANNOTCHANGEUMODE;
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
   
   // WHO messages
   static char const *L_ERR_WHOTRUNC;
   static char const *L_RPL_ENDOFWHO;
# ifdef DO_MATCH_COUNTING
   static char const *L_RPL_ENDOFWHO_NOMATCH;
   static char const *L_RPL_ENDOFWHO_MATCHES;
# endif
   
   // WHOIS messages
   static char const *L_RPL_WHOISVIRT;
   static char const *L_RPL_WHOISIDLE;
   static char const *L_RPL_WHOISOPERATOR;
   static char const *L_RPL_WHOISSECURE;
   static char const *L_RPL_ENDOFWHOIS;
   
   // WHOWAS messages
   static char const *L_RPL_ENDOFWHOWAS;
# ifdef DO_MATCH_COUNTING
   static char const *L_RPL_ENDOFWHOWAS_NOMATCH;
   static char const *L_RPL_ENDOFWHOWAS_MATCHES;
# endif
   
   // HELP command replies
   static char const *L_RPL_ENDOFHELP;
# ifdef DO_MATCH_COUNTING
   static char const *L_RPL_ENDOFHELP_NOMATCH;
   static char const *L_RPL_ENDOFHELP_MATCHES;
# endif
   static char const *L_RPL_ENDOFHELP_SIMPLE;
# ifdef DO_MATCH_COUNTING
   static char const *L_RPL_ENDOFHELP_SIMPLE_MATCHES;
# endif

   // LANGUAGE command messages
   static char const *L_RPL_ENDOFLANGUAGES;
   static char const *L_ERR_NOLANGUAGE;
   static char const *L_ERR_NOMORELANGS;
   
   // AWAY command messages
   static char const *L_RPL_NOWAWAY;
   static char const *L_RPL_UNAWAY;

   // WATCH command messages
   static char const *L_ERR_TOOMANYWATCH;
   static char const *L_RPL_WATCHSTAT;
   static char const *L_RPL_ENDOFWATCHLIST;
   static char const *L_RPL_LOGOFF_SERVER;
   static char const *L_RPL_LOGOFF_CHANNEL;
   static char const *L_RPL_LOGOFF_USER;
   static char const *L_RPL_LOGON_SERVER;
   static char const *L_RPL_LOGON_CHANNEL;
   static char const *L_RPL_LOGON_USER;
   static char const *L_RPL_NOWON_SERVER;
   static char const *L_RPL_NOWON_CHANNEL;
   static char const *L_RPL_NOWON_USER;
   static char const *L_RPL_NOWOFF;
   static char const *L_RPL_WATCHOFF;

   // ACCEPT command replies
   static char const *L_RPL_ENDOFACCEPT;
   static char const *L_ERR_ACCEPTFULL;
   static char const *L_ERR_ACCEPTEXIST;
   static char const *L_ERR_ACCEPTNOT;
   static char const *L_RPL_NOTACCEPTED;
   static char const *L_RPL_ACCEPTNOTICE;
   static char const *L_RPL_ACCEPTED;
   
   // SILENCE list replies
   static char const *L_RPL_ENDOFSILELIST;
   static char const *L_ERR_SILELISTFULL;

   // KNOCK replies
   static char const *L_ERR_NOKNOCK_ONCHANNEL;
   static char const *L_ERR_NOKNOCK_OPENCHANNEL;
   static char const *L_ERR_NOKNOCK_INVITED;
   static char const *L_ERR_NOKNOCK_REGONLY;
   static char const *L_RPL_KNOCKING; 
   static char const *L_DEFAULT_KNOCK_REASON;
   
   // LUSERS command replies
   static char const *L_RPL_LUSERCLIENT;
   static char const *L_RPL_LUSEROP;
   static char const *L_RPL_LUSERHELPERS;
   static char const *L_RPL_LUSERUNKNOWN;
   static char const *L_RPL_LUSERCHANNELS;
   static char const *L_RPL_LUSERME;
   static char const *L_RPL_LOCALUSERS;
   static char const *L_RPL_GLOBALUSERS;

   // Command not processed messages
   static char const *L_RPL_TRYAGAIN_AWAY;
   static char const *L_RPL_TRYAGAIN_KNOCK;
   
   // Server notice output strings
   static char const *L_SERVNOTICE_LINK;
   static char const *L_SERVNOTICE_DELINK;
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
   static char const *L_ERR_NOSUCHSERVER;
   static char const *L_ERR_ISCHANSERVICE;
   static char const *L_ERR_CANTKILLSERVICES;
   static char const *L_QUIT_KILLED;
   static char const *L_RPL_KILLDONE;
   static char const *L_ERR_NORECIPIENT;
   static char const *L_ERR_NOTEXTTOSEND;
   static char const *L_REQUESTED_SHUTDOWN;
   static char const *L_BYE_BYE_USER;
   static char const *L_ERROR_CLOSING_LINK;
   static char const *L_ERROR_CLOSING_LINK_DEFAULT_REASON;
   static char const *L_RPL_ENDOFINFO;
   static char const *L_RPL_LISTEND;
   static char const *L_RPL_ENDOFNAMES;
# ifdef DO_MATCH_COUNTING
   static char const *L_RPL_ENDOFNAMES_NOMATCH;
   static char const *L_RPL_ENDOFNAMES_MATCHES;
# endif
   static char const *L_RPL_ENDOFSTATS;
   static char const *L_RPL_VERSION;
   static char const *L_RPL_ADMINME;
   static char const *L_RPL_STATSUPTIME;
   static char const *L_RPL_STATSCONN;
   static char const *L_ERR_NONONREG_CHANNEL;
   static char const *L_ERR_NONONREG_USER;
   static char const *L_ERR_NOSUCHNICK;
   static char const *L_ERR_NOSUCHNICK_NICK;
   static char const *L_ERR_NOSUCHNICK_CHANNEL;
   static char const *L_ERR_NOSUCHNICK_OR_SERVER;
   static char const *L_ERR_CANNOTSENDTONICK_MUSTID;
   static char const *L_DEFAULT_QUIT_MESSAGE;
   static char const *L_ERR_NOOPERHOST;
   static char const *L_ERR_NOOPERHOST_NOOP;
   static char const *L_RPL_YOUREOPER;
   static char const *L_RPL_YOUREOPER_ALREADY;
   static char const *L_RPL_REHASHING;
   static char const *L_RPL_ENDOFLINKS;
};

#endif

