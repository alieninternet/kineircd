/* irc2user.cpp
 * Handle USER connection data using the IRC-3 server<->user protocol
 */

#include "autoconf.h"

#include <ctype.h>

#include "irc2user.h"
#include "daemon.h"
#include "debug.h"
#include "language.h"
#include "version.h"
#include "utils.h"
#include "operator.h"


/* Functions table. In the interests of efficiency should this table be 
 * broken down since a compiler would not be able to efficiently create a
 * structure multiplier for the CPU?
 */
#define ANYONE 0 // little macro for the table -- shh! :)
struct irc2userHandler::functionTableStruct const 
  irc2userHandler::functionsTable[] = {
     { "ACCEPT",	parseACCEPT,		1,
	  ANYONE,
	  "{ [ '+' | '-' ] <mask> ( ',' [ '+' | '-' ] <mask> ) } | '*'",
	  "This command is only useful with the user mode '+g' set. When set, "
	  "this command allows you to accept people, thus enabling them to "
	  "talk to you while you are ignoring everyone with user-mode '+g'. "
	  "If no parameters are given, a list of currently accepted masks "
	  "will be sent."
     },
     { "ADMIN",		parseADMIN,		2,
	  ANYONE,
	  "[ <target> ]",
	  "Returns information about the administrators of the given server. "
	  "If no server is specified, this returns information regarding "
	  "the administrators of the server you are locally connected to."
     }, 
     { "AWAY",		parseAWAY,		1,
	  ANYONE,
	  "[ <message> ]",
	  "If a message is given, this will mark you as 'away', using the "
	  "given message as your reason. If you do not give a message, this "
	  "will mark you as 'unaway', or that you have returned, and will "
	  "remove the given away message."
     }, 
     { "CONNECT",	parseCONNECT,		0,
	  User::isOper,
	  "<target server>  [ <port>  [ <remote server> ] ]",
	  "Instruct a server to connect to the target server on the default "
	  "port found in the configuration file for that server. If a port is "
	  "given, the server will connect on that port instead. If a remote "
	  "server is given, that server will be instructed to connect to "
	  "the target server rather than the local server forfilling the "
	  "request."
     },
     { "DIE",		parseDIE,		0,
	  User::isOper,
	  "[ <reason> ]",
	  "This command shuts down the server you are currently connected to. "
	  "Optionally a reason can be specified which will be broadcast to "
	  "other operators on the network as the reason for the server's "
	  "disappearance."
     },
     { "GLOBOPS",	parseGLOBOPS,		0,
	  User::isGlobalOper,
	  "<message>",
	  "Sends the given message to all global operators online (provided "
	  "they have their server-notices usermode set to receive them)."
     },
     { "HELP",		parseHELP,		3,
	  ANYONE,
	  "[ '-' ] [ <command> | <command mask> ]",
	  "Returns command parameters for the given command or mask. If no "
	  "mask it specified, it is assuming you mean all commands (or a mask "
	  "of '*'). When the '-' prefix is added to the command or mask, "
	  "additional help will be returned, such as what you are reading now."
     },
     { "HELPME",	parseHELPME,		4,
	  User::isHelper,
	  "<message>",
	  "Sends the given message to operators on-line for requesting help "
	  "with something. This is usually used to ask for an operator's "
	  "assistance with something a 'helper' cannot acheive on their own, "
	  "such as a KILL or GLINE."
     }, 
     { "INFO",		parseINFO,		1,
	  ANYONE,
	  "",
	  "Returns extended information about the version of the given "
	  "server. If no server is specified, this returns information "
	  "regarding the server you are locally connected to."
     }, 
     { "INVITE",	parseINVITE,		3,
	  ANYONE,
	  "[ <nickname>  <channel>  [ <timeout> ]",
	  "Invite the person under the given nickname to the specified "
	  "channel. You must be on the given channel for this to work. If "
	  "the channel is invite-only, you must also currently be either "
	  "an 'Operator' or 'Half-Operator' on that channel. If a timeout "
	  "value is given, it is the number of seconds the invitation is "
	  "valid for."
     }, 
     { "ISON",		parseISON,		2,
	  ANYONE,
	  "<nickname> ( SPACE <nickname> )",
	  "Check if the given nickname(s) are online. The server will "
	  "compile a list of nicknames who are online now in the exact "
	  "order of the list given."
     },
     { "JOIN",		parseJOIN,		3,
	  ANYONE,
	  "{ <channel> ( ',' <channel> )  [ <key> ( ',' <key> ) ] } | '0'",
	  "Join the given channels. If the channel(s) require a key, or "
	  "keys, then the keys list (if it exists) will be checked for a "
	  "valid key. If the special channel name '0' is used, it is "
	  "considered a 'partall' command, and will part you from any "
	  "channel you are currently on. Any channels specified before the "
	  "'0' on the list will not be joined."
     }, 
     { "KICK",		parseKICK,		1,
	  ANYONE,
	  "<channel> ( ',' <channel> )  <user> ( ',' <user> )  [ <reason> ]",
	  "Kick the given user(s) from the given channel(s), optionally "
	  "with a reason for your actions."
     },
     { "KILL",		parseKILL,		0,	
	  User::isOper,
	  "<nickname>  <reason>",
	  "Kill the given nickname with the specified reason. The reason is "
	  "manditory as you must have a reason for KILLing someone, "
	  "naturally :)"
     },
     { "KNOCK",		parseKNOCK,		0,
	  ANYONE,
	  "<channel>  [ <reason> ]",
	  "'Knock' on the 'door' of an invite-only channel. This only works "
	  "when you are not able to enter normally (the channel is invite "
	  "only and you are not on the invites list). You may also give a "
	  "reason for requesting an invitation, however this reason may not "
	  "be sent if the channel is set +n (no outside messages) to avoid "
	  "spaming."
     },
     { "LANGUAGE",	parseLANGUAGE,		0,
	  ANYONE,
	  "[ <language code> ( ',' <language code> ) ]",
	  "Select your preferred languages, in order from most preferred to "
	  "least preferred. When no parameters are specified, a list of "
	  "languages available on the local server are given. If languages "
	  "are unavailable to the server, the server will continue through "
	  "the list of languages for a language it does support, otherwise "
	  "the server language will not be changed, only the WHOIS language "
	  "information."
     },
     { "LINKS",		parseLINKS,		2,
	  ANYONE,
	  "[ <server mask> ]",
	  "Generate a list of server links. If a server mask is specified, "
	  "only servers matching the mask will be shown. If a remote server "
	  "is given, the request will be generated from the given server "
	  "instead of this server. Only helpers and operators will see "
	  "hidden servers."
     },
     { "LIST",		parseLIST,		5,
	  ANYONE,
	  "[ <channel> ( ',' <channel> ) ]",
	  "Return a list of channels currently created on the network. "
	  "The information returned also includes the number of users on "
	  "the channels, and the current topic."
     },
     { "LOCOPS",	parseLOCOPS,		0,
	  User::isOper,
	  "<message>",
	  "Sends a message to any operator connected to the same server as "
	  "you are. This is not to be confused with 'local operators', as "
	  "this command will send the given message to both local AND global "
	  "operators, limiting its reception to this server. Like GLOBOPS, "
	  "the message will only be received by an operator who has their "
	  "server-notices usermode set appropriately."
     },
     { "LUSERS",	parseLUSERS,		2,
	  ANYONE,
	  "[ <mask>  [ <target> ] ]",
	  "Show the current local-user status. This command has been "
	  "extended to also include global users, and peak user counts."
     },
     { "MAP",		parseMAP,		2,
	  ANYONE,
	  "",
	  "Display a map of the major bone links in the network, along with "
	  "other server information."
     },
     { "MODE",		parseMODE,		2,
	  ANYONE,
	  "{ <channel> | <nickname> | <server> }  ( [ '-' | '+' ] <mode> )  "
	  "[ <modeparams> ]",
	  0
     },
     { "MOTD",		parseMOTD,		3,
	  ANYONE,
	  "[ <target> ]",
	  "Display the MOTD (Message Of The Day). If a server is specified, "
	  "the MOTD displayed will come from that server, provided the given "
	  "server will allow you to receive it remotely."
     }, 
     { "NAMES",		parseNAMES,		1,
	  ANYONE,
	  "[ <channel> ( ',' <channel> ) ]",
	  0
     },
     { "NICK",		parseNICK,		3,
	  ANYONE,
	  "<new nickname>",
	  "Change your current nickname to the given nickname. This command "
	  "will fail if the given nickname is already in use, the nickname is "
	  "reserved for another purpose by the IRC Operators, you are in a "
	  "channel which is holding an event (channel mode +E), or your "
	  "connection is restricted."
     },
     { "NOTICE",	parseNOTICE,		4,
	  ANYONE,
	  "{ <nickname> | <channel> } ( ',' { <nickname> | <channel> } )  "
	  "<message>",
	  "Send the given NOTICE message to the given channel(s)/"
	  "nickname(s). Any combination of nicknames/channels can be "
	  "specified. Note that the NOTICE interface does not return any "
	  "responce, including erroneous target replies. This is done "
	  "intentionally to avoid never-ending automated replies bouncing "
	  "back and forth."
     },
     { "OPER",		parseOPER,		5,
	  ANYONE,
	  "<nickname>  <password>",
	  "This command allows you to become an IRC operator. You must give "
	  "your operator nickname and password for this command to be "
	  "successful."
     },
     { "PART",		parsePART,		3,
	  ANYONE,
	  "<channel> ( ',' <channel> )  [ <reason> ]",
	  "Leave the given channel(s), optionally giving a reason for your "
	  "departure."
     },
     { "PING",		parsePING,		1,
	  ANYONE,
	  "[ <text> ]",
	  "Ping the server, optionally giving text. The server will reply "
	  "with the given text, or just a PONG. Some clients use this "
	  "for 'lag checking', to measure the lag between the client and "
	  "the server."
     },
     { "PONG",		parsePONG,		1,
	  ANYONE,
	  "[ <text> ]",
	  "Used as a reply to a server ping, has no other purpose but for a "
	  "client to automatically respond to prove to the server the client "
	  "is still active."
     },
     { "PRIVMSG",	parsePRIVMSG,		4,
	  ANYONE,
	  "{ <nickname> | <channel> } ( ',' { <nickname> | <channel> } )  "
	  "<message>",
	  "Send the given PRIVMSG message to the given channel(s)/"
	  "nickname(s). Any combination of nicknames/channels can be "
	  "specified. Note that if you are being silenced by one of the "
	  "given nicks, the message may simply be ignored without warning. "
     },
     { "QUIT",		parseQUIT,		0,
	  ANYONE,
	  "[ <reason> ]",
	  "Leave the IRC network, optionally giving a reason for "
	  "disconnecting."
     },
     { "REHASH",	parseREHASH,		0,
	  User::isOper,
	  "",
	  "'Rehash' the server. This command (although incorrectly named) "
	  "will reload and parse the configuration file, as well as perform "
	  "some quick checks similar to start-up operation."
     },
     { "RESTART",	parseRESTART,		0,
	  User::isOper,
	  "",
	  "This will restart the server. This command will load an entirely "
	  "new process for the IRC Daemon."
     },
     { "SERVLIST",	parseSERVLIST,		0,
	  ANYONE,
	  "[ <mask>  [ <type> ] ]",
	  "List services currently visible on the network. If a mask is "
	  "specified, only services matching the given mask will be shown. If "
	  "a type is given, only services matching the mask and the type will "
	  "be listed."
     },
     { "SILENCE",	parseSILENCE,		1,
	  ANYONE,
	  "{ { '+' | '-' } { <mask> | <nickname } } | [ <nickname > ]",
	  "Silence the given mask or nickname. This will stop any privage "
	  "messages or notices being sent to you from anyone matching the "
	  "mask or nickname (excluding services, servers, operators and "
	  "helpers). The mask or nickname must be specified with a '+' or '-' "
	  "prefix to denote adding the silence or removing the silence "
	  "respectively. If only a nickname is given, and you are a helper "
	  "or an operator, you will see that person's current silence list."
     }, // This help message may be too big?
     { "SQUIT",		parseSQUIT,		0,
	  User::isOper,
	  "<server>  <reason>",
	  "Force the given server to depart the network with the given "
	  "reason. Warning: this command may cause a netsplit."
     },
     { "STATS",		parseSTATS,		0,
	  User::isOper,
	  "[ <query>  [ <target> ] ]",
	  "Ask for a status/statistical report from the current server. If "
	  "another server is given, the report will be generated there. If "
	  "no query is given, a list of possible queries you can specify."
     },
     { "TIME",		parseTIME,		1,
	  ANYONE,
	  "[ <target> ]",
	  "Returns the current time on the given server, or the server the "
	  "given user or service is connected to. This is useful when talking "
	  "to people around the world, provided they are connected to a "
	  "server that is geographically local to where they are situated."
     },
     { "TOPIC",		parseTOPIC,		3,
	  ANYONE,
	  "<channel>  [ <topic> ]",
	  "If no topic is specified, this returns the current topic on the "
	  "given channel. If a topic is specified, this command will change "
	  "the topic to the one given, provided you have access to do so."
     },
     { "TRACE",		parseTRACE,		0,
	  User::isRoutingStaff,
	  "[ <target> ]",
	  "Trace the path (internally on the network) if a given nickname, "
	  "service or server. This is useful for locating lagging "
	  "connections that need to be re-routed."
     },
#ifdef HAVE_CMD_TRACEROUTE
     { "TRACEROUTE",	parseTRACEROUTE,	0,
	  User::isRoutingStaff,
	  "<destination>  [ '-' <options> ]  [ <target> ]",
	  "Trace the path of IP packets (externally to the network) of the "
	  "given server. This is useful for finding better routes and should "
	  "be used to confirm a better solution for a route."
     },
#endif
     { "USERHOST",	parseUSERHOST,		2,
	  ANYONE,
	  "<nickname> ( SPACE <nickname> )",
	  "Returns the hosts of the nicknames on the given list. Note that "
	  "the host may in fact be a virtual-world host rather than the "
	  "real host. Also see USERIP."
     },
     { "USERIP",	parseUSERIP,		2,
	  ANYONE,
	  "<nickname> ( SPACE <nickname> )",
	  "Returns the IP addresses of the nicknames on the given list. Note "
	  "that the IP may in fact be a virtual-world IP rather than the "
	  "real IP. Also see USERHOST."
     },
     { "VERSION",	parseVERSION,		1,
	  ANYONE,
	  "[ <target> ]",
	  "Returns the version of the given server, or the server the given "
	  "user or service is connected to."
     },
     { "WATCH",		parseWATCH,		3,
	  ANYONE,
	  "'C' | 'S' | 'L' | { '+' | '-' } { <nickname> | <channel> | <server> } ( SPACE { '+' | '-' } { <nickname> | <channel> | "
	  "<server> } )",
	  "Used as an alternative to ISON, WATCH sets up a list of "
	  "nicknames, channels or servers you can 'watch'. You will be "
	  "notified of their appearance and disappearances on the network. "
	  "Note that only helpers and operators can watch a server."
     },
     { "WALLCHOPS",	parseWALLCHOPS,		4,
	  ANYONE,
	  "*tba*",
	  0
     },
#ifdef ALLOW_OPER_WALLOPS
     { "WALLOPS",	parseWALLOPS,		0,
	  User::isOper,
	  "<message>",
	  "Broadcasts the given message to all users on the network with the "
	  "user mode '+w' set."
     },
#endif
     { "WHO",		parseWHO,		2,
	  ANYONE,
	  "[ <channel> | <nickname mask> | '0' ]  "
	  "[ 'H' / 'I' / 'N' / 'O' / 'R' / 'S' / 'U' / 'X' ] "
	  "[ '%' { 'C' / 'D' / 'F' / 'H' / 'I' / 'N' / 'R' / 'S' / 'T' / "
	  "'U' } [ ',' <query type> ] ]",
	  0
     },
     { "WHOIS",		parseWHOIS,		2,
	  ANYONE,
	  "[ <target> ]  <nickname> ( ',' <nickname> )",
	  "Return information on who the given nickname(s) is/are. If a "
	  "server is specified, that server will generate the reply. This is "
	  "an extended version of the simplistic WHO command."
     },
     { "WHOWAS",	parseWHOWAS,		2,
	  ANYONE,
	  "<nickname> ( ',' <nickname> )  [ <count> ]",
	  "Return information on who the given nickname(s) was/were. The "
	  "information on who somebody was is kept for a short amount of "
	  "time after they have disconnected from the network. If 'count' is "
	  "given, it is the number of entries that should be shown."
     },
     { 0, 0, 0, 0, 0, 0 }
};


/* irc2userHandler - Constructor for the user handler sub-class
 * Original 12/08/01, Simon Butcher <pickle@austnet.org>
 */
irc2userHandler::irc2userHandler(Connection *c, User *u, String modes)
: Handler(c),
#ifdef FLOODLOCK_AWAY_SET
  lastAwaySet(0),
  lastAwaySetGrace(true),
#endif
  user(u)
{
#ifdef DEBUG
   debug("New Handler: irc2userHandler");
#endif

   // Send a nice notice to those who care
   Daemon::serverNotice(LocalUser::SN_SIGNONOFF,
			String::printf((char *)Language::L_SERVNOTICE_SIGNON,
				       (char const *)user->getNickname(),
				       (char const *)user->getUsername(),
				       (char const *)user->getHost(),
				       (char const *)user->getVWHost()));
   
   // Create a LocalUser class for this user since they are obviouslly local
   user->local = new LocalUser(user, this, Language::defaultLanguage);
   
   // Add this user to the connection list
   Daemon::addUser(user);
	    
   // Increase the client connection count, and the peak too if we need to
   Daemon::numClientConns++;
   if (Daemon::numClientConns > Daemon::numClientConnsPeak) {
      Daemon::numClientConnsPeak = Daemon::numClientConns;
   }

   // Send the greeting lines
   sendNumeric(Numerics::RPL_WELCOME,
	       String::printf(":%s %s %s",
			      (char const *)lang(Language::B_RPL_WELCOME),
			      (char const *)user->getAddress(),
			      (char const *)lang(Language::E_RPL_WELCOME)));
   sendNumeric(Numerics::RPL_YOURHOST,
	       String::printf(":%s %s, %s %s",
			      (char const *)lang(Language::B_RPL_YOURHOST),
			      (char const *)Daemon::myServer()->getHostname(),
			      (char const *)lang(Language::M_RPL_YOURHOST),
			      (char const *)Version::version));
   
#ifdef HAVE_IRC2USER_IRCII_KLUGE
   /* I hate this. It makes the signon look ugly, and us like a pack of idiots.
    * This is essentially THE SAME as RPL_YOURHOST, except the ircII client
    * desparately needs it.. or so they say - First person to confirm that to
    * be false gets the honours of removing this crap :)
    * Note how this is NOT in the language file. Originally it was, but upon
    * further investigation this string MUST be exactly like this, so it
    * really is a dodgy kluge :(
    */
   getConnection()->
     sendRaw(String::printf("NOTICE %s :*** Your host is %s, running version %s%s",
			    (char const *)user->nickname,
			    (char const *)Daemon::myServer()->getHostname(),
			    (char const *)Version::version,
			    IRC2USER_EOL_CHARS));
#endif
   
   sendNumeric(Numerics::RPL_CREATED,
	       String::printf(":%s %s",
			      (const char *)lang(Language::B_RPL_CREATED),
			      Version::build));
   sendNumeric(Numerics::RPL_MYINFO,
	       String::printf("%s %s %s %s %s %s %s %s %s",
			      (char const *)Daemon::myServer()->getHostname(),
			      Version::version,
			      User::modeStr,
			      Channel::modeStr,
			      Channel::modeParamStr,
			      User::modeParamStr,
			      Server::modeStr,
			      Server::modeParamStr,
			      Version::versionChars));
   sendNumeric(Numerics::RPL_ISUPPORT, 
	       Daemon::makeISUPPORT() + " :" + 
	       user->lang(Language::E_RPL_ISUPPORT));
   sendNumeric(Numerics::RPL_TIMEONSERVERIS,
	       String::printf("%lu %ld %s %s :%s ...fix-me...",
			      Daemon::getTime(),
			      Daemon::getTimeUsecs(),
			      (char const *)Daemon::getTimeZone(),
			      (char const *)Daemon::getTimeFlags(),
			      (char const *)lang(Language::B_RPL_TIMEONSERVERIS)));

   // Set up the user language stuff
   if (!user->local->noLang()) {
      sendNumeric(Numerics::RPL_YOURLANGUAGEIS,
		  String::printf("%s %s * :%s",
				 (char const *)user->local->lang(Language::LANGCODE),
				 (char const *)user->local->lang(Language::CHARSET),
				 (char const *)user->local->lang(Language::LANGNAME)));
   } else {
      sendNumeric(Numerics::RPL_YOURLANGUAGEIS, "none * * :None");
   }
   
   // Send some stuff that clients seem to rely on being sent... (urgh)
   doLUSERS(this, user, 0);
   
   // Should we send a minature motd or the full thing? and can we?
   if (Daemon::myServer()->isModeSet(Server::M_SHORTMOTD) /*&&
       user->getLanguage()->has(Language::L_RPL_MOTD_SHORT_A)*/) {
      // Send a fake three-line motd
      sendNumeric(Numerics::RPL_MOTDSTART,
		  String::printf(":- %s %s -",
				 (char const *)Daemon::myServer()->hostname,
				 (char const *)lang(Language::L_RPL_MOTDSTART)));
      sendNumeric(Numerics::RPL_MOTD, String(" :") + lang(Language::L_RPL_MOTD_SHORT_A));
      sendNumeric(Numerics::RPL_MOTD, String(" :") + lang(Language::L_RPL_MOTD_SHORT_B));
      sendNumeric(Numerics::RPL_MOTD, String(" :") + lang(Language::L_RPL_MOTD_SHORT_C));
      sendNumeric(Numerics::RPL_ENDOFMOTD, 
		  String(':') + lang(Language::L_RPL_ENDOFMOTD));
   } else {
      doMOTD(this, user);
   }
   
   // Send the 6-line spam notice thingy
//   if (user->getLanguage()->has(Language::L_RPL_SPAM_A)) {
      sendNumeric(Numerics::RPL_SPAM, ": ");
      sendNumeric(Numerics::RPL_SPAM, String(": ") + lang(Language::L_RPL_SPAM_A));
      sendNumeric(Numerics::RPL_SPAM, String(": ") + lang(Language::L_RPL_SPAM_B));
      sendNumeric(Numerics::RPL_SPAM, String(": ") + lang(Language::L_RPL_SPAM_C));
      sendNumeric(Numerics::RPL_SPAM, String(": ") + lang(Language::L_RPL_SPAM_D));
      sendNumeric(Numerics::RPL_SPAM, ": ");
//   }

#ifdef USER_REGISTRATION_MODES
   // Set the user modes to the default
   user->modes = USER_REGISTRATION_MODES;
#endif

   /* Some clients send the modes string the same way the /mode command works,
    * so we will look for that behaviour here and act on it if necessary. It
    * is technically incorrect, however...
    */
   if (modes[0] == '+') {
      StringTokens dummy("");
      processUserModes(modes, &dummy, true);
   } else {
      /* The correct method as mentioned in the RFC is to send an 8-bit bitmask
       * of the modes the user wishes to select. This is the correct behaviour,
       * but the old RFC specifies that field as the 'hostname' field which
       * is incorrect thesedays as the hostname field is replaced by a DNS
       * lookup, so we should double check that the integer conversion really
       * worked.
       */
      unsigned char modeBitMask = modes.toInt();
      
      // Check..
      if (modeBitMask > 0) {
	 /* There are only two modes defined in RFC2812, so we will check for
	  * both of them only. We have the first 8-bits of our usermode bit
	  * mask setup appropriately. This mess is just for sanity sake, just
	  * in case the user specifies something really ugly.
	  */
	 if (modeBitMask & User::M_WALLOPS) {
	    user->modes |= User::M_WALLOPS;
	 }
	 if (modeBitMask & User::M_INVISIBLE) {
	    user->modes |= User::M_INVISIBLE;
	 }
      }
   }
   
   // Send the modes if there are any set. Note we ignore any parameters...
   if (user->modes != 0) {
      sendUserMode(0, User::makeModes(user->modes));
   }
}


/* ~irc2userHandler - Class destructor
 * Original 13/08/01, Simon Butcher <pickle@austnet.org>
 */
irc2userHandler::~irc2userHandler(void)
{
   // Lower the connection count
   Daemon::numClientConns--;
}


/* goodbye - Handle a connection closure
 * Original 02/09/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::goodbye(String const &reason)
{
   // Send a notice to those who care
   Daemon::serverNotice(LocalUser::SN_SIGNONOFF,
			String::printf((char *)Language::L_SERVNOTICE_SIGNOFF,
				       (char const *)user->getNickname(),
				       (char const *)user->getUsername(),
				       (char const *)user->getHost(),
				       (char const *)reason));

   // Tell the user why the link is being closed if we need to
   sendGoodbye(reason);

   // Deregister us from the network
   Daemon::quitUser(user, reason, true);
}


/* kill - Handle a connection closure (killed)
 * Original 22/09/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::kill(String const &caller, String const &reason)
{
   // Tell the user why the link is being closed
   sendKill(user, caller, reason);
   
   // Shut down the connection
   getConnection()->kill();
}


/* processUserModes - Process a user mode string
 * Original 24/08/01, Simon Butcher <pickle@austnet.org>
 */
String irc2userHandler::processUserModes(String &modes, StringTokens *tokens, 
					 bool silent)
{
   bool toggle = true;
   int numModes = 0;
   String param = "";
   String toggleOnStr = "";
   String toggleOffStr = "";
   String toggleParamsOn = "";
   String toggleParamsOff = "";

   for (String::length_t i = 0; 
	((i < modes.length()) && (numModes < MAX_MODES_PER_COMMAND));
	i++) {
      switch (modes[i]) {
       case '+':
	 toggle = true;
	 continue;
       case '-':
	 toggle = false;
	 continue;
       default:
	 // Increment the mode counter
	 bool gotModeChar = false;
	 numModes++;
	 
	 /* Run through the user mode list and look for this char.
	  * Note that this is a case sensitive check
	  */
	 for (int ii = 0; User::modeTable[ii].letter != 0; ii++) {
	    if (User::modeTable[ii].letter == modes[i]) {
	       // Check if this mode can be toggled by the user
	       if (User::modeTable[ii].userToggle) {
		  // If this mode needs a parameter, grab the next token...
		  if ((toggle && User::modeTable[ii].hasParamOn) ||
		      (!toggle && User::modeTable[ii].hasParamOff)) {
		     param = tokens->nextToken();
		  } else {
		     param = "";
		  }
		  
		  // If this runs, we will need to add the output char
		  if (User::modeTable[ii].toggler(toggle, 
						  (silent ? 0 : this),
						  user,
						  &modes[i], &param) &&
		      !silent) {

		     // Check which toggle string to add this to
		     if (toggle) {
			toggleOnStr = toggleOnStr + String(modes[i]);
			
			// If this mode had a paramter, add it to the list
			if (param.length()) {
			   toggleParamsOn = toggleParamsOn + " " + param;
			}
		     } else {
			toggleOffStr = toggleOffStr + String(modes[i]);

			// If this mode had a paramter, add it to the list
			if (param.length()) {
			   toggleParamsOff = toggleParamsOff + " " + param;
			}
		     }
		  }
	       } else {
		  // Tell the user they cannot change that mode, if we can
		  if (!silent) {
		     sendNumeric(Numerics::ERR_CANNOTCHANGEUMODE,
				 String::printf("%c :%s",
						modes[i],
						(char const *)lang(Language::L_ERR_CANNOTCHANGEUMODE)));
		  }
	       }
	       
	       gotModeChar = true;
	    }
	 }
	 
	 /* Check if we did not find a valid char for that mode.
	  * Here's just another bit of proof this protocol is an oddity;
	  * Personally I beleive that like the channel mode not known message,
	  * this should include the mode character that failed. I have seen
	  * in debugging output from another irc server that DOES include
	  * this character, so I'm sneaking it in too. Yes, standards are there
	  * for a reason, but this doesn't break any clients since most of them
	  * seem to ignore anything before the colon anyway!!
	  */
	 if (!gotModeChar && !silent) {
	    sendNumeric(Numerics::ERR_UMODEUNKNOWNFLAG,
			String(modes[i]) + " :" +
			lang(Language::L_ERR_UMODEUNKNOWNFLAG));
	 }
      }
   }
   
   /* Assemble a full mode string from the toggle on and off strings.
    * This is for a smarter output, and also to follow the original IRC
    * specs.. grr
    */
   if (!silent) {
      String modeString = "";
      
      if (toggleOnStr.length()) {
	 modeString = String('+') + toggleOnStr;
      }
      if (toggleOffStr.length()) {
	 modeString = modeString + String('-') + toggleOffStr;
      }
      if (toggleParamsOn.length()) {
	 modeString = modeString + toggleParamsOn;
      }
      if (toggleParamsOff.length()) {
	 modeString = modeString + toggleParamsOff;
      }
      
      return modeString;
   }
   
   return "";
}


/* sendChannelMode - Send a channel mode change
 * Original 30/08/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::sendChannelMode(Channel *c, User *u,
				      String const &modes) const
{
   getConnection()->
     sendRaw(String::printf(":%s MODE %s %s" IRC2USER_EOL_CHARS,
			    (char const *)u->getAddress(user),
			    (char const *)c->name,
			    (char const *)modes));
}


/* sendGoodbye - Send an ERROR message to tell the client we are saying bye
 * Original 14/08/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::sendGoodbye(String const &reason) const
{
   getConnection()->
     sendRaw(String::printf((char *)Language::L_ERROR_CLOSING_LINK,
			    (reason.length() ?
			     (char const *)reason :
			     Language::L_ERROR_CLOSING_LINK_DEFAULT_REASON)));
}


/* sendInvite - Send an invitation to a channel
 * Original 19/10/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::sendInvite(User *user, Channel *channel, User *from,
				 time_t expiry) const
{
   getConnection()->
     sendRaw(String::printf(":%s INVITE %s %s %lu" IRC2USER_EOL_CHARS,
			    (char const *)from->getAddress(user),
			    (char const *)user->getNickname(),
			    (char const *)channel->getName(),
			    expiry));
}


/* sendJoin - Send a channel join message
 * Original 14/08/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::sendJoin(Channel *c, User *u) const
{
   getConnection()->
     sendRaw(String::printf(":%s JOIN %s" IRC2USER_EOL_CHARS,
			    (char const *)u->getAddress(user),
			    (char const *)c->name));
}


/* sendKick - Send a channel kick message
 * Original 20/09/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::sendKick(Channel *c, User *kicker, User *kickee,
			       String const &reason) const
{
   getConnection()->
     sendRaw(String::printf(":%s KICK %s %s%s" IRC2USER_EOL_CHARS,
			    (char const *)kicker->getAddress(user),
			    (char const *)c->name,
			    (char const *)kickee->nickname,
			    ((reason.length() ?
			      (char const *)(String(" :") + 
					     reason) : ""))));
}


/* sendKill - Send a user kill message
 * Original 22/09/01, Simon Butcher <pickle@austnet.org>
 * Note: The user field is ignored since any kill MUST be to us anyway
 */
void irc2userHandler::sendKill(User *u, String const &caller, 
			       String const &reason) const
{
   getConnection()->
     sendRaw(String::printf(":%s KILL %s :%s (%s)" IRC2USER_EOL_CHARS,
			    (char const *)caller,
			    (char const *)user->nickname,
			    (char const *)caller,
			    (char const *)reason));
}


/* sendKnock - Send a channel knocking message
 * Original 24/10/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::sendKnock(User *u, Channel *c, 
				String const &reason) const
{
   getConnection()->
     sendRaw(String::printf(":%s KNOCK %s :%s" IRC2USER_EOL_CHARS,
			    (char const *)u->getAddress(user),
			    (char const *)c->getName(),
			    (char const *)reason));
}


/* sendLanguage - Send a channel knocking message
 * Original 24/10/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::sendLanguage(User *u, String const &languages,
				   String const &charset) const
{
   getConnection()->
     sendRaw(String::printf(":%s LANGUAGE %s %s" IRC2USER_EOL_CHARS,
			    (char const *)u->getAddress(user),
			    (char const *)languages,
			    (char const *)charset));
}


/* sendNickChange - Send a nickname change event
 * Original 24/08/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::sendNickChange(User *u, String const &nick) const
{
   getConnection()->
     sendRaw(String::printf(":%s NICK :%s" IRC2USER_EOL_CHARS,
			    (char const *)u->getAddress(user),
			    (char const *)nick));
}


/* sendNotice - [Various Forms] Send a NOTICE message
 * Original 01/08/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::sendNotice(Server *from, User *destination, 
				 String const &message) const
{
   getConnection()->
     sendRaw(String::printf(":%s NOTICE %s :%s" IRC2USER_EOL_CHARS,
			    (char const *)from->getHostname(),
			    (char const *)destination->getNickname(),
			    (char const *)message));
}

void irc2userHandler::sendNotice(User *from, Channel *destination,
				 String const &message) const
{
   getConnection()->
     sendRaw(String::printf(":%s NOTICE %s :%s" IRC2USER_EOL_CHARS,
			    (char const *)from->getAddress(user),
			    (char const *)destination->name,
			    (char const *)message));
}

void irc2userHandler::sendNotice(User *from, User *destination, 
				 String const &message) const
{
   getConnection()->
     sendRaw(String::printf(":%s NOTICE %s :%s" IRC2USER_EOL_CHARS,
			    (char const *)from->getAddress(user),
			    (char const *)destination->nickname,
			    (char const *)message));
}

void irc2userHandler::sendNotice(User *from, StringMask const &destination,
				 String const &message) const
{
   getConnection()->
     sendRaw(String::printf(":%s NOTICE %s :%s" IRC2USER_EOL_CHARS,
			    (char const *)from->getAddress(user),
			    (char const *)destination.getMask(),
			    (char const *)message));
}


/* sendNoticeAnon - Send a *anonymous* NOTICE message
 * Original 12/10/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::sendNoticeAnon(Channel *destination,
				     String const &message) const
{
   getConnection()->
     sendRaw(String::printf(":" ANONYMOUS_IDENT_MASK " NOTICE %s :%s"
			    IRC2USER_EOL_CHARS,
			    (char const *)destination->name,
			    (char const *)message));
}


/* sendNumeric - [Various forms] Send a numeric to the connection
 * Original 12/08/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::sendNumeric(Numerics::numeric_t numeric,
				  String const &line) const
{
   getConnection()->
     sendRaw(String::printf(":%s %03d %s %s" IRC2USER_EOL_CHARS,
			    (char const *)Daemon::myServer()->getHostname(),
			    numeric,
			    (char const *)user->getNickname(),
			    (char const *)line));
}

void irc2userHandler::sendNumeric(Server *from, Numerics::numeric_t numeric, 
				  User *to, String const &line) const
{
   getConnection()->
     sendRaw(String::printf(":%s %03d %s %s" IRC2USER_EOL_CHARS,
			    (char const *)from->getHostname(),
			    numeric,
			    (char const *)user->getNickname(),
			    (char const *)line));
}


/* sendPart - Send a channel part message
 * Original 14/08/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::sendPart(Channel *c, User *u, String const &reason) const
{
   getConnection()->
     sendRaw(String::printf(":%s PART %s%s" IRC2USER_EOL_CHARS,
			    (char const *)u->getAddress(user),
			    (char const *)c->name,
			    (char const *)(reason.length() ?
					   (String(" ") + reason) :
					   String(""))));
}


/* sendPing - Send a PING request to this connection
 * Original 24/08/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::sendPing(void) const
{
   getConnection()->
     sendRaw(String::printf("PING :%s" IRC2USER_EOL_CHARS,
			    (char const *)Daemon::myServer()->getHostname()));
}


/* sendPong - Send a PING reply (PONG)
 * Original 20/08/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::sendPong(String const &reply) const
{
   getConnection()->
     sendRaw(String::printf(":%s PONG %s %s" IRC2USER_EOL_CHARS,
			    (char const *)Daemon::myServer()->getHostname(),
			    (char const *)Daemon::myServer()->getHostname(),
			    (char const *)reply));
}


/* sendPrivmsg - [Various Forms] Send a PRIVMSG message
 * Original 01/08/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::sendPrivmsg(Server *from, User *destination,
				  String const &message) const
{
   getConnection()->
     sendRaw(String::printf(":%s PRIVMSG %s :%s" IRC2USER_EOL_CHARS,
			    (char const *)from->getHostname(),
			    (char const *)destination->getNickname(),
			    (char const *)message));
}

void irc2userHandler::sendPrivmsg(User *from, Channel *destination, 
				  String const &message) const
{
   getConnection()->
     sendRaw(String::printf(":%s PRIVMSG %s :%s" IRC2USER_EOL_CHARS,
			    (char const *)from->getAddress(user),
			    (char const *)destination->name,
			    (char const *)message));
}

void irc2userHandler::sendPrivmsg(User *from, User *destination,
				  String const &message) const
{
   getConnection()->
     sendRaw(String::printf(":%s PRIVMSG %s :%s" IRC2USER_EOL_CHARS,
			    (char const *)from->getAddress(user),
			    (char const *)destination->getNickname(),
			    (char const *)message));
}

void irc2userHandler::sendPrivmsg(User *from, StringMask const &destination,
				  String const &message) const
{
   getConnection()->
     sendRaw(String::printf(":%s PRIVMSG %s :%s" IRC2USER_EOL_CHARS,
			    (char const *)from->getAddress(user),
			    (char const *)destination.getMask(),
			    (char const *)message));
}


/* sendPrivmsgAnon - Send a *anonymous* PRIVMSG message
 * Original 12/10/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::sendPrivmsgAnon(Channel *destination,
				      String const &message) const
{
   getConnection()->
     sendRaw(String::printf(":" ANONYMOUS_IDENT_MASK " PRIVMSG %s :%s" 
			    IRC2USER_EOL_CHARS,
			    (char const *)destination->name,
			    (char const *)message));
}


/* sendQuit - Send a network quit message
 * Original 22/08/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::sendQuit(User *u, String const &reason) const
{
   getConnection()->
     sendRaw(String::printf(":%s QUIT :%s" IRC2USER_EOL_CHARS,
			    (char const *)u->getAddress(user),
			    (reason.length() ?
			     (char const *)reason :
			     Language::L_DEFAULT_QUIT_MESSAGE)));
}


/* sendServerMode - [Various forms] Send a server mode change
 * Original 21/09/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::sendServerMode(Server *s, Server *setter, 
				     String const &modes) const
{
   getConnection()->
     sendRaw(String::printf(":%s MODE %s %s" IRC2USER_EOL_CHARS,
			    (char const *)setter->getHostname(),
			    (char const *)s->getHostname(),
			    (char const *)modes));
}

void irc2userHandler::sendServerMode(Server *s, User *setter, 
				     String const &modes) const
{
   getConnection()->
     sendRaw(String::printf(":%s MODE %s %s" IRC2USER_EOL_CHARS,
			    (char const *)setter->getAddress(user),
			    (char const *)s->getHostname(),
			    (char const *)modes));
}


/* sendSilence - Send a SILENCE reply
 * Original 23/09/01, Simon Butcher <pickle@austnet.org>
 * Note: The 'silencer' is ignored here, since this is always going to be
 *       the current user.
 */
void irc2userHandler::sendSilence(User *silencer, bool setting, 
				  StringMask const &mask) const
{
   getConnection()->
     sendRaw(String::printf(":%s SILENCE %c%s" IRC2USER_EOL_CHARS,
			    (char const *)user->getAddress(),
			    (setting ? '+' : '-'),
			    (char const *)mask.getMask()));
}


/* sendTopic - [Various Forms] Send a channel topic change
 * Original 19/09/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::sendTopic(Channel *chan, Server *from,
				String const &topic) const
{
   getConnection()->
     sendRaw(String::printf(":%s TOPIC %s :%s" IRC2USER_EOL_CHARS,
			    (char const *)from->getHostname(),
			    (char const *)chan->name,
			    (char const *)topic));
}

void irc2userHandler::sendTopic(Channel *chan, User *from, 
				String const &topic) const
{
   getConnection()->
     sendRaw(String::printf(":%s TOPIC %s :%s" IRC2USER_EOL_CHARS,
			    (char const *)from->getAddress(user),
			    (char const *)chan->name,
			    (char const *)topic));
}


/* sendUserMode - Send a user mode change
 * Original 25/08/01, Simon Butcher <pickle@austnet.org>
 * Note: This is a strange one, but this is how it's supposed to work
 *       according to the implementations (not the RFC). Here the given user
 *       is ignored
 */
void irc2userHandler::sendUserMode(User *u, String const &modes) const
{
   getConnection()->
     sendRaw(String::printf(":%s MODE %s :%s" IRC2USER_EOL_CHARS,
			    (char const *)user->nickname,
			    (char const *)user->nickname,
			    (char const *)modes));
}


/* sendWhoReply - Send a WHO reply
 * Original 22/08/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::sendWhoReply(User *u, Channel *c, 
				   ChannelMember *cm) const
{
   sendNumeric(Numerics::RPL_WHOREPLY,
	       String::printf("%s %s %s %s %s %c%s :%d %s" IRC2USER_EOL_CHARS,
			      (c ? 
			       (char const *)c->name : 
			       "*"),
			      (char const *)u->username,
			      (u->showVW(user) ?
			       (char const *)u->vwhostname :
			       (char const *)u->hostname),
			      (((u->server->isModeSet(Server::M_HIDDEN)) &&
				!User::isOper(user)) ?
			       (char const *)u->server->getHostname() :
			       SERVER_NAME_MASK),
			      (char const *)u->nickname,
			      (u->awayMessage.length() ?
			       'G' : 'H'),
			      ((char const *)
			       (String(User::isOper(u) ?
				       "*" : "") +
				(cm ?
				 (cm->isModeSet(ChannelMember::M_OPPED) ?
				  "@" :
				  (cm->isModeSet(ChannelMember::M_HALFOPPED) ?
				   "%" :
				   (cm->isModeSet(ChannelMember::M_VOICED) ?
				    "+" : ""))) : ""))),
			      u->server->getNumHops(),
			      (char const *)u->realname));
}


/* sendWallops - [Various Forms] Send a WALLOPS message
 * Original 18/09/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::sendWallops(Server *from, String const &message) const
{
   getConnection()->
     sendRaw(String::printf(":%s WALLOPS :%s" IRC2USER_EOL_CHARS,
			    (char const *)from->getHostname(),
			    (char const *)message));
}

void irc2userHandler::sendWallops(User *from, String const &message) const
{
   getConnection()->
     sendRaw(String::printf(":%s WALLOPS :%s" IRC2USER_EOL_CHARS,
			    (char const *)from->getAddress(user),
			    (char const *)message));
}


/* sendWatchOff - [Various Forms] Send an object sign-off notification message
 * Original 24/09/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::sendWatchOff(Server *target) const
{
   getConnection()->
     sendRaw(String::printf(":%s %d %s %s * * 0 :%s" IRC2USER_EOL_CHARS,
			    (char const *)Daemon::myServer()->getHostname(),
			    Numerics::RPL_LOGOFF,
			    (char const *)user->nickname,
			    (char const *)target->getHostname(),
			    (char const *)lang(Language::E_RPL_LOGOFF_SERVER)));
}

void irc2userHandler::sendWatchOff(Channel *target) const
{
   getConnection()->
     sendRaw(String::printf(":%s %d %s %s * * %lu :%s" IRC2USER_EOL_CHARS,
			    (char const *)Daemon::myServer()->getHostname(),
			    Numerics::RPL_LOGOFF,
			    (char const *)user->nickname,
			    (char const *)target->name,
			    target->creationTime,
			    (char const *)lang(Language::E_RPL_LOGOFF_CHANNEL)));
}

void irc2userHandler::sendWatchOff(User *target) const
{
   getConnection()->
     sendRaw(String::printf(":%s %d %s %s %s %s %lu :%s" IRC2USER_EOL_CHARS,
			    (char const *)Daemon::myServer()->getHostname(),
			    Numerics::RPL_LOGOFF,
			    (char const *)user->nickname,
			    (char const *)target->nickname,
			    (char const *)target->username,
			    (char const *)target->getHost(user),
			    target->lastNickChange,
			    (char const *)lang(Language::E_RPL_LOGOFF_USER)));
}


/* sendWatchOn - [Various Forms] Send an object sign-on notification message
 * Original 24/09/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::sendWatchOn(Server *target) const
{
   getConnection()->
     sendRaw(String::printf(":%s %d %s %s * * 0 :%s" IRC2USER_EOL_CHARS,
			    (char const *)Daemon::myServer()->getHostname(),
			    Numerics::RPL_LOGON,
			    (char const *)user->getNickname(),
			    (char const *)target->getHostname(),
			    (char const *)lang(Language::E_RPL_LOGON_SERVER)));
}

void irc2userHandler::sendWatchOn(Channel *target, String const &creator) const
{
   getConnection()->
     sendRaw(String::printf(":%s %d %s %s %s * %lu :%s" IRC2USER_EOL_CHARS,
			    (char const *)Daemon::myServer()->getHostname(),
			    Numerics::RPL_LOGON,
			    (char const *)user->getNickname(),
			    (char const *)target->getName(),
			    (char const *)creator,
			    target->creationTime,
			    (char const *)lang(Language::E_RPL_LOGON_CHANNEL)));
}

void irc2userHandler::sendWatchOn(User *target, String const &newNick) const
{
   getConnection()->
     sendRaw(String::printf(":%s %d %s %s %s %s %lu :%s" IRC2USER_EOL_CHARS,
			    (char const *)Daemon::myServer()->getHostname(),
			    Numerics::RPL_LOGON,
			    (char const *)user->nickname,
			    (newNick.length() ?
			     (char const *)newNick :
			     (char const *)target->nickname),
			    (char const *)target->username,
			    (char const *)target->getHost(user),
			    target->lastNickChange,
			    (char const *)lang(Language::E_RPL_LOGON_USER)));
}


// Pull out a language string
inline String irc2userHandler::lang(Language::tag_t const &t) const
{
   return user->lang(t);
}


/* doNAMES - Process and reply to a NAMES command
 * Original 15/08/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::doNAMES(String const &param)
{
#ifdef HAVE_IRC2USER_MATCH_COUNTING
   unsigned int matches = 0;
#endif
   
   // Grab the channels list
   StringTokens channels(param);
   
   for (String chan = channels.nextToken(','); chan.length(); 
	chan = channels.nextToken(',')) {
      Channel *c = Daemon::getChannel(chan);
      
      // Make sure we got a channel, otherwise we just ignore it
      if (c) {
#ifdef HAVE_IRC2USER_MATCH_COUNTING
	 // Increase the match counter
	 matches++;
#endif
	 
	 String reply = "";

	 for (Channel::member_map_t::iterator it = c->members.begin();
	      it != c->members.end(); it++) {
	    // Do we need to write a new prefix?
	    if (!reply.length()) {
	       reply = String(((c->modes & Channel::M_PRIVATE) ?
			       "*" :
			       ((c->modes & Channel::M_SECRET) ?
				"@" : "="))) + " " + c->name + " :";
	    }
	    
	    reply = reply +
	      ((*it).second->isModeSet(ChannelMember::M_OPPED) ?
	       "@" : 
	       ((*it).second->isModeSet(ChannelMember::M_HALFOPPED) ?
		"%" : 
		((*it).second->isModeSet(ChannelMember::M_VOICED) ?
		 "+" : ""))) +
	      (*it).second->getUser()->nickname + " ";
	    
	    // Check if we are close to breaking a limit here
	    if (reply.length() > 400) {
	       sendNumeric(Numerics::RPL_NAMREPLY, reply);
	       
	       // reset the reply
	       reply = "";
	    }
	 }
	 
	 sendNumeric(Numerics::RPL_NAMREPLY, reply);
      }
   }
   
   // Send end of names list
#ifdef HAVE_IRC2USER_MATCH_COUNTING
   sendNumeric(Numerics::RPL_ENDOFNAMES,
	       ((matches > 0) ?
		((matches == 1) ?
		 String::printf("%s :%s",
				(char const *)param,
				(char const *)lang(Language::L_RPL_ENDOFNAMES)) :
		 String::printf("%s :%s (%u %s)",
				(char const *)param,
				(char const *)lang(Language::L_RPL_ENDOFNAMES),
				matches,
				(char const *)lang(Language::W_MATCH_PL))) :
		String::printf("%s :%s (%s)",
			       (char const *)param,
			       (const char *)lang(Language::L_RPL_ENDOFNAMES),
			       (const char *)lang(Language::P_NO_MATCH))));
#else
   sendNumeric(Numerics::RPL_ENDOFNAMES,
	       String::printf("%s :%s"
			      (char const *)param,
			      (char *)lang(Language::L_RPL_ENDOFNAMES)));
#endif
}


/* parseLine - Parse an incoming line
 * Original 12/08/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::parseLine(String const &line)
{
   // Flood protection check?!

   // Break the line apart
   StringTokens st(line);
   String command = st.nextToken().toUpper();
   
   // Run through the list and find a function..
   for (int i = 0; functionsTable[i].command != 0; i++) {
      // Check if the command matches
      if (command == functionsTable[i].command) {
	 // Check the command access permissions against this user
	 if (!functionsTable[i].access ||
	     functionsTable[i].access(user)) {
	    // Run the command
	    functionsTable[i].function(this, &st);
	 } else {
	    /* Act dumb, break the loop to tell the user we do not know
	     * what that command was. Duhhhhhhhh :)
	     */
	    break;
	 }
	 return;
      }
   }

   // Command not found, complain about it..
   sendNumeric(Numerics::ERR_UNKNOWNCOMMAND,
	       command + " :" + lang(Language::L_ERR_UNKNOWNCOMMAND));
}


/* parseACCEPT - Accept a user for receiving messages (efnet +g style)
 * Original 23/10/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::parseACCEPT(irc2userHandler *handler, StringTokens *tokens)
{
   // Check we have a parameter
   if (tokens->countTokens() < 2) {
      handler->sendNumeric(Numerics::ERR_NEEDMOREPARAMS,
			   String("ACCEPT :") + 
			   handler->lang(Language::L_ERR_NEEDMOREPARAMS));
      return;
   }

   // Grab the parameter
   String param = tokens->nextToken();
   
   // Are we listing?
   if (param == "*") {
      String reply = "";
      
      for (User::accept_set_t::iterator it = handler->user->accepts.begin();
	   it != handler->user->accepts.end(); it++) {
	 reply = reply + " " + *it;
	 
	 // Check if we are close to breaking a limit here
	 if (reply.length() > 400) {
	    handler->sendNumeric(Numerics::RPL_ACCEPTLIST, reply);

	    // reset the reply
	    reply = "";
	 }
      }
	 
      // Is there anything left to send?
      if (reply.length()) {
	 handler->sendNumeric(Numerics::RPL_ACCEPTLIST, reply);
      }
	 
      // Send the end of list message
      handler->sendNumeric(Numerics::RPL_ENDOFACCEPT, String(':') +
			   handler->lang(Language::L_RPL_ENDOFACCEPT));
      
      return;
   }
   
   // Tokenise..
   StringTokens st(param);
   
//   handler->sendNumeric(Numerics::ERR_ACCEPTFULL, String(':') +
//			handler->lang(Language::L_ERR_ACCEPTFULL));
   
   // Run through the parameters
   for (String nick = st.nextToken(',').IRCtoLower(); nick.length();
	nick = st.nextToken(',').IRCtoLower()) {
      // Are we removing a mask?
      if (nick[0] == '-') {
	 // Fix the string appropriately
	 nick = nick.subString(1);
	
	 // Make sure there is a string left to do stuff with
	 if (!nick.length()) {
	    continue;
	 }
	 
	 // Find this user
	 User *u = Daemon::getUser(nick);
	 
	 // Check
	 if (!u) {
	    handler->
	      sendNumeric(Numerics::ERR_NOSUCHNICK,
			  nick + " :" +
			  handler->lang(Language::L_ERR_NOSUCHNICK_NICK));
	    continue;
	 }
	 
	 // Delete the nick
	 if (Daemon::delUserAccept(handler->user, u)) {
	    // Done! Apparently we do not send any confirmation of this...
	    continue;
	 } 
	 
	 // Tell the user the nickname was not found
	 handler->sendNumeric(Numerics::ERR_ACCEPTNOT, 
			      nick + " :" +
			      handler->lang(Language::E_ERR_ACCEPTNOT));
	 
	 continue;
      }

      // If we got here we are adding.. Check if this is explicit (+ prefixed)
      if (nick[0] == '+') {
	 // Fix the string appropriately
	 nick = nick.subString(1);

	 // Make sure there is a string left to do stuff with
	 if (!nick.length()) {
	    continue;
	 }
      }
      
      // Find this user
      User *u = Daemon::getUser(nick);
	 
      // Check the user
      if (!u) {
	 handler->
	   sendNumeric(Numerics::ERR_NOSUCHNICK,
		       nick + " :" +
		       handler->lang(Language::L_ERR_NOSUCHNICK_NICK));
	 continue;
      }
      
      // Add the nick
      if (Daemon::addUserAccept(handler->user, u)) {
	 // Done! Send confirmation to the remote party
	 Daemon::routeTo(u)->
	   sendNumeric(Daemon::myServer(),
		       Numerics::RPL_ACCEPTED, handler->user,
		       String::printf("%s %s %s :%s",
				      (char const *)handler->user->getNickname(),
				      (char const *)handler->user->getUsername(),
				      (char const *)handler->user->getHost(u),
				      (char const *)handler->lang(Language::E_RPL_ACCEPTED)));
	 continue;
      }
      
      // The nick is already on the list, complain
      handler->sendNumeric(Numerics::ERR_ACCEPTEXIST,
			   nick + " :" +
			   handler->lang(Language::E_ERR_ACCEPTEXIST));
   }
}


/* parseADMIN
 * Original 27/08/01, Simon Butcher <pickle@austnet.org> 
 */
void irc2userHandler::parseADMIN(irc2userHandler *handler, StringTokens *tokens)
{
   String server = tokens->nextToken();
   
   // Server not specified, or it is us?
   if (!server.length()) {
      doADMIN(handler, handler->user);
   } else {
      Server *s = Daemon::getServer(StringMask(server));
      
      // Check
      if (!s) {
	 // Ok.. well try and look up a user to grab a server..
	 User *u = Daemon::getUser(server);
	 
	 // Check
	 if (!u) {
	    handler->sendNumeric(Numerics::ERR_NOSUCHSERVER,
				 server + " :" +
				 handler->lang(Language::L_ERR_NOSUCHSERVER));
	    return;
	 }
	 
	 // Copy the server pointer across
	 s = u->getServer();
      }
      
      // Is this US??
      if (s == Daemon::myServer()) {
	 doADMIN(handler, handler->user);
	 return;
      }
   
      // Poll the remote server
      Daemon::routeTo(s)->callADMIN(s, handler->user);
   }
}


/* parseAWAY
 * Original 13/08/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::parseAWAY(irc2userHandler *handler, StringTokens *tokens)
{
   // Grab the away message
   String message = tokens->nextColonToken();
   
   // If we have a message, the user is going away, otherwise they are 'unaway'
   if (message.length()) {
#ifdef FLOODLOCK_AWAY_SET
      // Check they can change this, first look at the time...
      if ((handler->lastAwaySet + FLOODLOCK_AWAY_SET) >
	  Daemon::getTime()) {
	 // Ok... maybe they have a grace period then?
	 if (!handler->lastAwaySetGrace) {
	    // Tell them they have to be a little more patient :)
	    handler->sendNumeric(Numerics::RPL_TRYAGAIN, String("AWAY :") +
				 handler->lang(Language::L_RPL_TRYAGAIN_AWAY));
	    return;
	 } else {
	    // Chew up the grace setting
	    handler->lastAwaySetGrace = false;
	 }
      } else {
	 // Give them a grace setting after this one
	 handler->lastAwaySetGrace = true;
      }
      
#endif

      // Do the change
      handler->user->markAway(message);
	 
#ifdef FLOODLOCK_AWAY_SET
      // Mark the change down..
      handler->lastAwaySet = Daemon::getTime();
#endif
      
      // Tell them it worked
      handler->sendNumeric(Numerics::RPL_NOWAWAY,
			   String(':') + 
			   handler->lang(Language::L_RPL_NOWAWAY));
   } else {
      // Do the change
      handler->user->markAway(message);

      // Tell them it worked
      handler->sendNumeric(Numerics::RPL_UNAWAY,
			   String(':') + 
			   handler->lang(Language::L_RPL_UNAWAY));
   }
}


/* parseCONNECT
 * Original , Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::parseCONNECT(irc2userHandler *handler, StringTokens *tokens)
{
   handler->sendNumeric(Numerics::RPL_DUMMY_NUMERIC, ":Oops, sorry, havn't coded this yet :(");
}


/* parseDIE
 * Original 14/08/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::parseDIE(irc2userHandler *handler, StringTokens *tokens)
{
   String reason = tokens->rest();

   // Send out a server notice
   Daemon::
     serverNotice(LocalUser::SN_OPER,
		  String::printf((char *)Language::L_SERVNOTICE_CMD_DIE,
				 ((char const *)
				  handler->user->nickname)));
   
   // Check if we have a reason
   if (reason.length()) {
      Daemon::shutdown(reason + " (" + 
					  handler->user->nickname + ")");
   } else {
      Daemon::shutdown(handler->user->nickname + 
			  Language::L_REQUESTED_SHUTDOWN);
   }
}


/* parseGLOBOPS
 * Original 19/09/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::parseGLOBOPS(irc2userHandler *handler, StringTokens *tokens)
{
   String message = tokens->rest();
   
   // Check that we have been given something
   if (
#ifdef MINLEN_OP_BROADCAST
       message.length() >= MINLEN_OP_BROADCAST
#else
       message.length()
#endif
       ) {
      // Strip the colon if it is there (smart client if it is :)
      if (message[0] == ':') {
	 message = message.subString(1);
      }
   
      Daemon::
	serverNotice(LocalUser::SN_GLOBOPS,
		     String::printf((char *)Language::L_SERVNOTICE_GLOBOPS,
				    ((char const *)
				     handler->user->nickname), 
				    (char const *)message));
      return;
   }
   
   // Complain bitterly!
#ifdef MINLEN_OP_BROADCAST
   handler->sendNumeric(Numerics::ERR_NOTEXTTOSEND, String(':') +
			handler->lang(Language::L_ERR_NOTEXTTOSEND_NEEDMORE));
#else
   handler->sendNumeric(Numerics::ERR_NOTEXTTOSEND, String(':') +
			handler->lang(Language::L_ERR_NOTEXTTOSEND));
#endif
}


/* parseHELP
 * Original 13/08/01, Simon Butcher <pickle@austnet.org>
 * Note: Not the best use on CPU since it's checking a match no matter how
 *       this is called, but since this is very rarely called I decided the 
 *       code should be small rather than faster :)
 */
void irc2userHandler::parseHELP(irc2userHandler *handler, StringTokens *tokens)
{
#ifdef HAVE_IRC2USER_MATCH_COUNTING
   unsigned int matches = 0;
#endif
   bool extended = false;
   
   // Rip out the command mask we are looking for, if any
   String maskStr = tokens->nextToken().toUpper();

   // Check if the first char is a '-' (for extended help)
   if (maskStr[0] == '-') {
      extended = true;
      maskStr = maskStr.subString(1);
   }
   
   // Check the line, if there is nothing there we will assume '*'
   if (!maskStr.length()) {
      maskStr = "*";
   }
   
   // Convert it into a StringMask
   StringMask mask(maskStr);
   
   // Run through the list of functions
   for (int i = 0; functionsTable[i].command != 0; i++) {
      // Check if this is the command our user was looking for
      if (mask.matches((char const *)functionsTable[i].command)) {
	 // Display this command if the user has the rights to use it
	 if (!functionsTable[i].access ||
	     functionsTable[i].access(handler->user)) {
#ifdef HAVE_IRC2USER_MATCH_COUNTING
	    // Increase the match counter
	    matches++;
#endif
	    
	    // Send the user the help for this function
	    handler->sendNumeric(Numerics::RPL_HELP,
				 String::printf("%s :%s",
						functionsTable[i].command,
						functionsTable[i].parameters));
	    
	    // If we are doing extended help, send the extended lines
	    if (extended && functionsTable[i].help) {
	       handler->sendNumeric(Numerics::RPL_MOREHELP,
				    String::printf("%s :%s",
						   functionsTable[i].command,
						   functionsTable[i].help));
	    }
	 }
      }
   }

   // Send the end of help numeric
   if (extended) {
#ifdef HAVE_IRC2USER_MATCH_COUNTING
      handler->
	sendNumeric(Numerics::RPL_ENDOFHELP,
		    ((matches > 0) ?
		     ((matches == 1) ?
		      String::printf("%s :%s",
				     (char const *)maskStr,
				     (char const *)handler->lang(Language::L_RPL_ENDOFHELP)) :
		      String::printf("%s :%s (%u %s)",
				     (char const *)maskStr,
				     (char const *)handler->lang(Language::L_RPL_ENDOFHELP),
				     matches,
				     (char const *)handler->lang(Language::W_MATCH_PL))) :
		     String::printf("%s :%s (%s)",
				    (char const *)maskStr,
				    (char const *)handler->lang(Language::L_RPL_ENDOFHELP),
				    (char const *)handler->lang(Language::P_NO_MATCH))));
#else
      handler->
	sendNumeric(Numerics::RPL_ENDOFHELP,
		    String::printf("%s :%s",
				   (char const *)maskStr,
				   (char const *)handler->lang(Language::L_RPL_ENDOFHELP)));
#endif
   } else {
#ifdef HAVE_IRC2USER_MATCH_COUNTING
      handler->
	sendNumeric(Numerics::RPL_ENDOFHELP,
		    ((matches > 0) ?
		     ((matches == 1) ?
		      String::printf("%s :%s",
				     (char const *)maskStr,
				     (char const *)handler->lang(Language::L_RPL_ENDOFHELP_SIMPLE)) :
		      String::printf("%s :%s (%u %s)",
				     (char const *)maskStr,
				     (char const *)handler->lang(Language::L_RPL_ENDOFHELP_SIMPLE),
				     matches,
				     (char const *)handler->lang(Language::W_MATCH_PL))) :
		     String::printf("%s :%s (%s)",
				    (char const *)maskStr,
				    (char const *)handler->lang(Language::L_RPL_ENDOFHELP_SIMPLE),
				    (char const *)handler->lang(Language::P_NO_MATCH))));
#else
      handler->
	sendNumeric(Numerics::RPL_ENDOFHELP,
		    String::printf("%s :%s",
				   (char const *)maskStr,
				   (char const *)handler->lang(Language::L_RPL_ENDOFHELP_SIMPLE)));
#endif
   }
}


/* parseHELPME
 * Original 19/09/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::parseHELPME(irc2userHandler *handler, StringTokens *tokens)
{
   String message = tokens->rest();

   // Check that we have been given something
   if (
#ifdef MINLEN_OP_BROADCAST
       message.length() >= MINLEN_OP_BROADCAST
#else
       message.length()
#endif
       ) {
      // Strip the colon if it is there (smart client if it is :)
      if (message[0] == ':') {
	 message = message.subString(1);
      }
   
      Daemon::
	serverNotice(LocalUser::SN_HELPME,
		     String::printf((char *)Language::L_SERVNOTICE_HELPME,
				    ((char const *)
				     handler->user->nickname),
				    (char const *)message));
      return;
   }
   
   // Complain bitterly!
#ifdef MINLEN_OP_BROADCAST
   handler->sendNumeric(Numerics::ERR_NOTEXTTOSEND, String(':') +
			handler->lang(Language::L_ERR_NOTEXTTOSEND_NEEDMORE));
#else
   handler->sendNumeric(Numerics::ERR_NOTEXTTOSEND, String(':') +
			handler->lang(Language::L_ERR_NOTEXTTOSEND));
#endif
}


/* parseINFO
 * Original 14/08/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::parseINFO(irc2userHandler *handler, StringTokens *tokens)
{
   // Send info lines...
   for (register unsigned int i = 0; Version::versionInfo[i]; i++) {
      handler->sendNumeric(Numerics::RPL_INFO,
			   String(':') + Version::versionInfo[i]);
   }
   
   // End of the list
   handler->sendNumeric(Numerics::RPL_ENDOFINFO, String(':') +
			handler->lang(Language::L_RPL_ENDOFINFO));
}


/* parseINVITE
 * Original 19/10/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::parseINVITE(irc2userHandler *handler, StringTokens *tokens)
{
   // Check we have at least two parameters
   if (tokens->countTokens() < 3) {
      handler->sendNumeric(Numerics::ERR_NEEDMOREPARAMS,
			   String("INVITE :") + 
			   handler->lang(Language::L_ERR_NEEDMOREPARAMS));
      return;
   }

   // Assemble variables
   String nick = tokens->nextToken();
   String chan = tokens->nextToken();
   long timeout = tokens->nextToken().toLong();

   // Work out the invitation expiry time
   time_t expiry;
   if (!timeout) {
      timeout = 0;
   }
   
   // The expiry is the current time plus the timeout length...
   expiry = (time_t)(Daemon::getTime() + timeout);
   
   // .. But this might be in the past due to an overload, so check
   if (expiry < (Daemon::getTime() + MIN_INVITE_TIMEOUT)) {
      expiry = 0;
   }
   
   // Try to look up the given user
   User *u = Daemon::getUser(nick);
   
   // Check
   if (!u) {
      handler->
	sendNumeric(Numerics::ERR_NOSUCHNICK,
		    nick + " :" +
		    handler->lang(Language::L_ERR_NOSUCHNICK_NICK));
      return;
   }
   
   // Look up the channel
   Channel *c = Daemon::getChannel(chan);
   
   // Check
   if (!c) {
      handler->sendNumeric(Numerics::ERR_NOSUCHCHANNEL,
			   chan + " :" +
			   handler->lang(Language::L_ERR_NOSUCHCHANNEL));
      return;
   }

   // look up our channel member record
   ChannelMember *cm = c->getMember(handler->user);
   
   // Check
   if (!cm) {
      handler->sendNumeric(Numerics::ERR_NOTONCHANNEL,
			   chan + " :" + 
			   handler->lang(Language::L_ERR_NOTONCHANNEL));
      return;
   }
   
   // Make sure the person we are looking for is not already on the channel
   if (c->getMember(u)) {
      handler->
	sendNumeric(Numerics::ERR_USERONCHANNEL,
		    String::printf("%s %s :%s",
				   (char const *)nick, 
				   (char const *)chan,
				   (char const *)handler->lang(Language::E_ERR_USERONCHANNEL)));
      return;
   }
   
   // If this channel is invite only, we must be a full op or half op..
   if (c->isModeSet(Channel::M_INVITE) && 
       !cm->isChanOper()) {
      handler->sendNumeric(Numerics::ERR_CHANOPRIVSNEEDED,
			   chan + " :" +
			   handler->lang(Language::L_ERR_CHANOPRIVSNEEDED));
     return;
   }

   // Check if this user is local
   if (u->isLocal()) {
      // Add the invitation to this user record
      u->getLocalInfo()->addInvitation(c, handler->user, expiry);
   }
   
   // Do the invitation
   Daemon::routeTo(u)->sendInvite(u, c, handler->user, expiry);
   
   // Tell the user the invitation is happening
   handler->sendNumeric(Numerics::RPL_INVITING,
			String::printf("%s %s 0",
				       (char const *)nick,
				       (char const *)chan));
}


/* parseISON
 * Original 13/08/01, Simon Butcher <pickle@austnet.org>
 * Note: I don't like this.. there has to be a better way. We cannot simply
 *       run through the list and check the line since the RFC says we must
 *       return the ISON members in exact order as given, just 'edited'.
 *       There has to be a better way of doing this, and I'm too lazy at the
 *       moment to do anything about it. Only problem here is that many
 *       irc clients poll the server for their /notify commands that often
 *       that this command should be considered 'expensive' in its current
 *       state. This is an example of where 'WATCH' might be of some benifit,
 *       since the /notify command is practically universal. Notice how we
 *       do not check for the string being too long, this is because the
 *       client should have sent us a properly formatted string in the first
 *       place - we're too trusting perhaps :)
 */
void irc2userHandler::parseISON(irc2userHandler *handler, StringTokens *tokens)
{
   // Check we have at least one parameter
   if (tokens->countTokens() < 2) {
      handler->sendNumeric(Numerics::ERR_NEEDMOREPARAMS,
			   String("ISON :") + 
			   handler->lang(Language::L_ERR_NEEDMOREPARAMS));
      return;
   }

   String reply = ":";

   // Run through the parameter tokens
   for (String iter = tokens->nextToken(); iter.length(); 
	iter = tokens->nextToken()) {
      // Check if we have that nick in our userlist
      if (Daemon::getUser(iter)) {
	 reply = reply + iter + " ";
      }
   }
   
   // Send the reply
   handler->sendNumeric(Numerics::RPL_ISON, reply);
}


/* parseJOIN
 * Original 14/08/01, Simon Butcher <pickle@austnet.org>
 * Needs: '0' support. I would have done this already, except to stop people
 *        being able to flood it should scan the tokens for a 0, part all,
 *        then join channels AFTER that 0; eg. /join #foo,#bar,0,#baz should
 *        part-all then join #baz instead of joining #foo and #bar then doing
 *        the part-all... does that make sense? :-/
 */
void irc2userHandler::parseJOIN(irc2userHandler *handler, StringTokens *tokens)
{
   // Check we have at least one parameter
   if (tokens->countTokens() < 2) {
      handler->sendNumeric(Numerics::ERR_NEEDMOREPARAMS,
			   String("JOIN :") + 
			   handler->lang(Language::L_ERR_NEEDMOREPARAMS));
      return;
   }

   // This command effects the user idle time, so we must change it here
   handler->getConnection()->touch();
   
   // Rip out the two token sequences
   StringTokens channels(tokens->nextToken());
   StringTokens keys(tokens->nextToken());
   
   // Run through the parameter tokens
   for (String chan = channels.nextToken(','); chan.length(); 
	chan = channels.nextToken(',')) {
      if (!User::isHelper(handler->user)) {
	 // Check if this channel is being redirected to another channel
	 String redir = Daemon::redirectedChannel(chan);
	 if (redir.length()) {
	    // Notify a savvy client
	    handler->
	      sendNumeric(Numerics::RPL_CHANREDIR,
			  String::printf("%s %s :%s",
					 (char const *)chan,
					 (char const *)redir,
					 (char const *)handler->lang(Language::L_RPL_CHANREDIR)));
	    // Do the swap
	    chan = redir;
	 }
      }
	    
      // Make sure joining this channel does not go over our limit
      if (handler->user->channels.size() >= MAX_CHANNELS_PER_USER) {
	 handler->sendNumeric(Numerics::ERR_TOOMANYCHANNELS,
			      chan + " :" +
			      handler->lang(Language::L_ERR_TOOMANYCHANNELS));
	 return; // no point going through other channels.
      }
      
      // Make sure the channel name is acceptable
      if (!Channel::okName(chan)) {
	 handler->sendNumeric(Numerics::ERR_BADCHANNAME,
			      chan + " :" + 
			      handler->lang(Language::L_ERR_BADCHANNAME));
	 continue;
      }

      // Find this channel in the channel list
      Channel *c = Daemon::getChannel(chan);
      
      // Check if we got this channel or not
      if (c) {
	 // Check that this user is not already on this channel
	 if (c->getMember(handler->user)) {
	    // Apparently we just ignore this...?
	    return;
	 }
	 
	 /* Check if this channel is a 'registered nicknames only' channel and
	  * that this user is identified
	  */
	 if ((c->modes & Channel::M_REGNICKSONLY) &&
	     !(handler->user->modes & User::M_IDENTIFIED)) {
	    handler->
	      sendNumeric(Numerics::ERR_NONONREG,
			  chan + " :" +
			  handler->lang(Language::L_ERR_NONONREG_CHANNEL));
	    continue;
	 }
	 
	 // Check if this user is banned
	 if (c->onBan(handler->user)) {
	    // if this user does not have an exemption, they cannot be let in
	    if (!c->onBanExcept(handler->user)) {
	       handler->
		 sendNumeric(Numerics::ERR_BANNEDFROMCHAN,
			     chan + " :" +
			     handler->lang(Language::L_ERR_BANNEDFROMCHAN));
	       continue;
	    }
	 }
	 
	 // Has this user been invited here?
	 if (handler->user->local->invitedTo(c)) {
	    // Remove the invitation as it has been accepted
	    handler->user->local->delInvitation(c);
	 } else {
	    // Check if this channel is set invite only
	    if (c->isModeSet(Channel::M_INVITE)) {
	       // Check this user is not invited or not on the invite list
	       if (!c->onInvite(handler->user)) {
		  handler->
		    sendNumeric(Numerics::ERR_INVITEONLYCHAN,
				chan + " :" +
				handler->lang(Language::L_ERR_INVITEONLYCHAN));
		  continue;
	       }
	    }
	 }

	 // check if this user joining the channel would break a +l (limit)
	 if ((c->limit > 0) && 
	     (c->members.size() >= c->limit)) {
	    handler->sendNumeric(Numerics::ERR_CHANNELISFULL,
				 chan + " :" +
				 handler->lang(Language::L_ERR_CHANNELISFULL));
	    continue;
	 }
	 
	 // Check if this channel needs a key, and it is correct
	 if (c->key.length() && 
	     (c->key != keys.nextToken(','))) {
	    handler->sendNumeric(Numerics::ERR_BADCHANNELKEY,
				 chan + " :" +
				 handler->lang(Language::L_ERR_BADCHANNELKEY));
	    continue;
	 }
	 
	 // Join the user to their new channel
	 Daemon::joinChannel(c, handler->user);
      } else {
	 // This channel did not exist. Is it a safe channel?
	 if (chan[0] == '!') {
	    // Reject their request outright. We should fix this one day?
	    handler->sendNumeric(Numerics::ERR_BADCHANNAME,
				 chan + " :" +
				 handler->lang(Language::L_ERR_BADCHANNAME_SAFECHAN));
	    continue;
	 }

	 // Check for stuff that helpers/operators skip
	 if (!User::isHelper(handler->user)) {
	    // Check if this channel is not marked on the fail mask list
	    String reason = Daemon::failedChannel(chan);
	    if (reason.length()) {
	       // Tell the user they cannot join, giving them the reaso
	       handler->
		 sendNumeric(Numerics::ERR_BADCHANNAME,
			     String::printf("%s :%s (%s)",
					    (char const *)chan,
					    (char const *)handler->lang(Language::L_ERR_BADCHANNAME_FAILMASK),
					    (char const *)reason));
	       continue;
	    }
	 }
	 
	 // Create this channel with this user as the channel creator
	 c = new Channel(chan, Daemon::getTime());
	 Daemon::addChannel(c, handler->user->getNickname());
      
	 // Join the user to their new channel
	 Daemon::joinChannel(c, handler->user);

	 /* If this is an open channel we cannot +O/+o the user. The only
	  * mode that gets set is +t, which means only server can change the
	  * topic. This is an unregulated channel, basically.
	  */
	 if (chan[0] == '+') {
	    
	 } else {
	    /* Else this user gets +O (creator) and +o (oper) status
	     * Note: This is a 'mysterious hidden mode change'.
	     */
	    c->members[handler->user->nickname.IRCtoLower()]->modes |=
	      ChannelMember::M_CREATOR | ChannelMember::M_OPPED;
	 }
      }
      
      // Send topic information (if there is a topic set)
      if (c->topic.length()) {
	 handler->sendNumeric(Numerics::RPL_TOPIC,
			      String::printf("%s :%s",
					     (char const *)c->name,
					     (char const *)c->topic));
	 handler->sendNumeric(Numerics::RPL_TOPICWHOTIME,
			      String::printf("%s %s :%lu",
					     (char const *)c->name,
					     (char const *)c->topicWho,
					     c->topicTime));
      }
      
      // Send the names of the ppl in this channel.
      handler->doNAMES(c->name);
   }
}


/* parseKICK
 * Original 20/09/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::parseKICK(irc2userHandler *handler, StringTokens *tokens)
{
   // Check we have at least one parameter
   if (tokens->countTokens() < 3) {
      handler->sendNumeric(Numerics::ERR_NEEDMOREPARAMS,
			   String("KICK :") + 
			   handler->lang(Language::L_ERR_NEEDMOREPARAMS));
      return;
   }
   
   // Grab the variables
   StringTokens channels(tokens->nextToken());
   StringTokens targets(tokens->nextToken());
   String reason = tokens->nextColonToken();
   
   // Run through the parameter tokens
   for (String chan = channels.nextToken(','); chan.length(); 
	chan = channels.nextToken(',')) {
      // Grab the target
      String target = targets.nextToken(',');
      
      // Check, ignore it if we have no target (thick head client :)
      if (!target.length()) {
	 continue;
      }

      // Try to grab the channel
      Channel *c = handler->user->getChannel(chan);
      
      // Check
      if (!c) {
	 handler->sendNumeric(Numerics::ERR_NOTONCHANNEL,
			      chan + " :" + 
			      handler->lang(Language::L_ERR_NOTONCHANNEL));
	 continue;
      } 
      
      // Try and grab the member record for this person
      ChannelMember *kicker = c->getMember(handler->user);
      
      // Check, if in OPTIMISE_FOR_SPEED mode, this is wrong
      if (!kicker) {
#ifdef DEBUG
	 debug("irc2userHandler KICK sanity check on channel member failed. "
	       "No channel member found when the channel was taken from a "
	       "member -- VERY WRONG!");
#endif
	 handler->sendNumeric(Numerics::ERR_NOTONCHANNEL,
			      chan + " :" + 
			      handler->lang(Language::L_ERR_NOTONCHANNEL));
	 continue;
      }
      
      // Does this fella have the rights to even contemplate kicking someone?
      if (!kicker->isChanOper()) {
	 handler->sendNumeric(Numerics::ERR_CHANOPRIVSNEEDED,
			      chan + " :" +
			      handler->lang(Language::L_ERR_CHANOPRIVSNEEDED));
	 continue;
      }
      
      // Try and get the target user record
      User *u = Daemon::getUser(target);
      
      // Check
      if (!u) {
	 handler->
	   sendNumeric(Numerics::ERR_NOSUCHNICK,
		       target + " :" +
		       handler->lang(Language::L_ERR_NOSUCHNICK_NICK));
	 continue;
      }

      // Try and get the channel member record for this user
      ChannelMember *kickee = c->getMember(u);
      
      // Check
      if (!kickee) {
	 handler->
	   sendNumeric(Numerics::ERR_USERNOTINCHANNEL,
		       String::printf("%s %s :%s",
				      (char const *)u->nickname,
				      (char const *)chan,
				      (char const *)handler->lang(Language::L_ERR_USERNOTINCHANNEL)));
	 continue;
      }
      
      // Check if this user is immune from kicks
      if (u->modes & User::M_NONKICKABLE) {
	 handler->
	   sendNumeric(Numerics::ERR_ISCHANSERVICE,
		       String::printf("%s %s :%s",
				      (char const *)u->nickname,
				      (char const *)c->name,
				      (char const *)handler->lang(Language::L_ERR_ISCHANSERVICE)));
	 continue;
      }
      
      // Check that the kickee is not an op while this guy is just a half op
      if (kickee->isModeSet(ChannelMember::M_OPPED) &&
	  kicker->isModeSet(ChannelMember::M_HALFOPPED) &&
	  !kicker->isModeSet(ChannelMember::M_OPPED)) {
	 handler->
	   sendNumeric(Numerics::ERR_CHANOPRIVSNEEDED,
		       chan + " :" +
		       handler->lang(Language::L_ERR_CHANOPRIVSNEEDED_HALFOPERVSOPER));
	 continue;
      }
      
      // Finally, all things say we can kick this bloke out!
      Daemon::kickChannelMember(c, handler->user, u, reason);
   }
}


/* parseKILL
 * Original 22/09/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::parseKILL(irc2userHandler *handler, StringTokens *tokens)
{
   // Check we have at least two parameters
   if (tokens->countTokens() < 3) {
      handler->sendNumeric(Numerics::ERR_NEEDMOREPARAMS,
			   String("KILL :") + 
			   handler->lang(Language::L_ERR_NEEDMOREPARAMS));
      return;
   }

   // Grab the varables
   String target = tokens->nextToken();
   String reason = tokens->nextColonToken();
   
   // Find the target
   User *u = Daemon::getUser(target);
   
   // Check
   if (!u) {
      handler->sendNumeric(Numerics::ERR_NOSUCHNICK,
			   target + " :" +
			   handler->lang(Language::L_ERR_NOSUCHNICK_NICK));
      return;
   }
   
   // Check if this user is immune from kills
   if (u->modes & User::M_NONKICKABLE) {
      handler->sendNumeric(Numerics::ERR_CANTKILLSERVICES, String(':') +
			   handler->lang(Language::L_ERR_CANTKILLSERVICES));
      // Broadcast this blunder?
      return;
   }
   
   // Check permissions here
   
   // Do the kill
   Daemon::killUser(u, handler->user->nickname, reason);

   /* This is 'obsolete' but it is a good idea if the oper has server notices
    * turned off just to confirm the kill.
    */
   handler->sendNumeric(Numerics::RPL_KILLDONE,
			target + " :" +
			handler->lang(Language::L_RPL_KILLDONE));
}


/* parseKNOCK
 * Original 24/10/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::parseKNOCK(irc2userHandler *handler, StringTokens *tokens)
{
   // Check we have at least one parameter
   if (tokens->countTokens() < 2) {
      handler->sendNumeric(Numerics::ERR_NEEDMOREPARAMS,
			   String("KNOCK :") + 
			   handler->lang(Language::L_ERR_NEEDMOREPARAMS));
      return;
   }
   
   // Grab the variables
   String channel = tokens->nextToken();
   String reason = tokens->nextColonToken();

   // Look up the channel
   Channel *chan = Daemon::getChannel(channel);
   
   // Check the channel
   if (!chan) {
      handler->sendNumeric(Numerics::ERR_NOSUCHCHANNEL,
			   channel + " :" +
			   handler->lang(Language::L_ERR_NOSUCHCHANNEL));
      return;
   }

   // Check if this user is a member of the channel
   if (chan->getMember(handler->user)) {
      handler->sendNumeric(Numerics::ERR_NOKNOCK,
			   channel + " :" +
			   handler->lang(Language::L_ERR_NOKNOCK_ONCHANNEL));
      return;
   }
   
   // Check if the channel is open (not invite only)
   if (!chan->isModeSet(Channel::M_INVITE)) {
      handler->sendNumeric(Numerics::ERR_NOKNOCK,
			   channel + " :" +
			   handler->lang(Language::L_ERR_NOKNOCK_OPENCHANNEL));
      return;
   }
       
   // Check if this user is on the channel invite list
   if (chan->onInvite(handler->user) ||
       handler->user->local->invitedTo(chan)) {
      handler->sendNumeric(Numerics::ERR_NOKNOCK,
			   channel + " :" +
			   handler->lang(Language::L_ERR_NOKNOCK_INVITED));
      return;
   }
   
   /* Check that this channel is not a registered-user-only channel and this
    * user has not identified themselves
    */
   if (chan->isModeSet(Channel::M_REGNICKSONLY) &&
       !handler->user->isModeSet(User::M_IDENTIFIED)) {
      handler->sendNumeric(Numerics::ERR_NOKNOCK,
			   channel + " :" +
			   handler->lang(Language::L_ERR_NOKNOCK_REGONLY));
      return;
   }
   
#ifdef FLOODLOCK_KNOCK_MSG
   // Check if the user has not knocked too soon...
   if (handler->user->local->onKnockMessageFloodlock(chan)) {
      handler->sendNumeric(Numerics::RPL_TRYAGAIN, String("KNOCK :") +
			   handler->lang(Language::L_RPL_TRYAGAIN_KNOCK));
      return;
   }
#endif
   
   /* Check if this channel can receive outside messages, or there is no 
    * reason to use, and replace the reason if necessary
    */
   if (chan->isModeSet(Channel::M_NOOUTSIDEMSG) ||
       !reason.length()) {
      reason = handler->lang(Language::L_DEFAULT_KNOCK_REASON);
   }
   
   // OK! Finally, do the channel knock
   chan->sendKnock(handler->user, reason);

   // Confirm the knock
   handler->sendNumeric(Numerics::RPL_KNOCKING,
			channel + " :" +
			handler->lang(Language::E_RPL_KNOCKING));
}


/* parseLANGUAGE
 * Original 26/10/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::parseLANGUAGE(irc2userHandler *handler, StringTokens *tokens)
{
   // Check we have at least one parameter
   if (tokens->countTokens() < 2) {
#ifdef HAVE_IRC2USER_MATCH_COUNTING
      unsigned int numLanguages = 0;
#endif
      
      // Run through known languages and list them to the client
      for (Language::languages_map_t::iterator it =
	   Language::languages.begin(); 
	   it != Language::languages.end(); it++) {
#ifdef HAVE_IRC2USER_MATCH_COUNTING
	 numLanguages++;
#endif
	 handler->
	   sendNumeric(Numerics::RPL_LANGUAGE,
		       String::printf("%s %s %s %s %s * :%s",
				      (char const *)(*it).first,
				      (char const *)(*it).second->get(Language::REVISION),
				      (char const *)(*it).second->get(Language::MAINTAINER),
				      (char const *)(*it).second->get(Language::CHARSET),
				      "*", // flags
				      (char const *)(*it).second->get(Language::LANGNAME)));
      }
      
      // Send the end of list tag
#ifdef HAVE_IRC2USER_MATCH_COUNTING
      if (numLanguages > 1) {
	 handler->
	   sendNumeric(Numerics::RPL_ENDOFLANGUAGES,
		       String::printf(":%s (%u %s)",
				      (char const *)handler->lang(Language::L_RPL_ENDOFLANGUAGES),
				      numLanguages,
				      (char const *)handler->lang(Language::W_LANGUAGE_PL)));
      } else {
#endif
	 handler->sendNumeric(Numerics::RPL_ENDOFLANGUAGES, String(':') +
			      handler->lang(Language::L_RPL_ENDOFLANGUAGES));
#ifdef HAVE_IRC2USER_MATCH_COUNTING
      }
#endif
      return;
   }
   
   // Grab the languages list
   StringTokens st(tokens->nextToken());
   String languages = "";
   int langCount = 0;
   LanguageData *ld = 0;
   bool gotLocal = false;
   
   // Run through the given languages
   for (String lang = st.nextToken(','); lang.length();
	lang = st.nextToken(',')) {
      // Fix the language code up..
      lang = lang.toLower().subString(0, MAXLEN_LANGCODE);
      
      // Check if we have taken too many languages
      if (++langCount > DEFAULT_MAX_LANGS_PER_USER) {
	 // Complain!
	 handler->
	   sendNumeric(Numerics::ERR_NOMORELANGS,
		       String::printf("%u :%s",
				      DEFAULT_MAX_LANGS_PER_USER,
				      (char const *)handler->lang(Language::L_ERR_NOMORELANGS)));
	 break;
      }

      // Look up this language
      if (lang != "none") {
	 ld = Language::get(lang);
      } else {
	 ld = 0;
      }
      
      // If we have not already got our 'local' langauge, look for it
      if (!gotLocal && 
	  (ld || (lang == "none"))) {
	 // Set the language
	 handler->user->getLocalInfo()->setLang(ld);
	 gotLocal = true;
	 
	 // Tell the user about the new local server language setting
	 if (lang == "none") {
	    handler->sendNumeric(Numerics::RPL_YOURLANGUAGEIS,
				 "none * * :None");
	 } else {
	    handler->
	      sendNumeric(Numerics::RPL_YOURLANGUAGEIS,
			  String::printf("%s %s * :%s%s",
					 (char const *)lang,
					 (char const *)ld->get(Language::CHARSET),
					 (char const *)ld->get(Language::LANGNAME),
					 (ld->has(Language::LANGNOTE) ?
					  ((char const *)
					   (String(" (Note: ") +
					    ld->get(Language::LANGNOTE) + 
					    ")")) :
					  "")));
	 }
      }
      
      // Add the language to the languages list
#ifndef ACCEPT_UNKNOWN_LANGS
      if (ld) {
#endif
	 if (languages.length()) {
	    languages = languages + String(",") + lang;
	 } else {
	    languages = languages + lang;
	 }
#ifndef ACCEPT_UNKNOWN_LANGS
      } else {
	 handler->sendNumeric(Numerics::ERR_NOLANGUAGE,
			      lang + " :" + 
			      handler->lang(Language::E_ERR_NOLANGUAGE));
      }
#endif
   }
   
   // Throw a LANGUAGE confirmation line at the user, if we have any
   if (languages.length()) {
      handler->user->
	setLangInfo(languages,
		    (handler->user->local->noLang() ? String("") :
		     handler->user->local->lang(Language::CHARSET)));
   }
}


/* parseLINKS
 * Original 27/08/01, Simon Butcher <pickle@austnet.org>
 * Note: In accordance with other servers around the world, this command works
 *       somewhat differently to how traditional links works. Traditionally,
 *       this command gives enough information to know the topology of the
 *       network, but this can lead to showing weak-points on the network
 *       perfect for attack. This command has been modified so that it only
 *       returns information about other servers, even the hop count has been
 *       replaced with the number of users, on purpose. The result of this is
 *       that the command no longer needs to be used remotely so that
 *       component of it has been removed as well.
 */
void irc2userHandler::parseLINKS(irc2userHandler *handler, StringTokens *tokens)
{
   // Grab the appropriate mask requested
   String maskStr = tokens->nextToken().toLower();
   
   if (!maskStr.length()) {
      maskStr = "*";
   }
   
   StringMask mask(maskStr);
   
   // Run through the list
   for (Daemon::server_map_t::iterator it = Daemon::servers.begin();
	it != Daemon::servers.end(); it++) {
      // Check for a match, and send this if the server is not hidden
      if (mask.matches((*it).second->getHostname()) &&
	  !((*it).second->isModeSet(Server::M_HIDDEN))) {
	 // Send the user this record
	 handler->
	   sendNumeric(Numerics::RPL_LINKS,
		       String::printf("%s * :%u %s",
				      (char const *)(*it).second->getHostname(),
				      (*it).second->getNumUsers(),
				      (char const *)(*it).second->getDescription()));
      }
   }
   
   // End of the list
   handler->sendNumeric(Numerics::RPL_ENDOFLINKS,
			maskStr + " :" +
			handler->lang(Language::L_RPL_ENDOFLINKS));
}


/* parseLIST
 * Original 15/08/01, Simon Butcher <pickle@austnet.org>
 * Note: This should be a 'remote', but since the content is so large nobody
 *       really needs to list channels remotely, unless of course they wanted
 *       local channels. Bah, local channels are *LOCAL* anyway! Many other
 *       networks (including austnet) already have this as a 'local only'
 *       command in any case.
 */
void irc2userHandler::parseLIST(irc2userHandler *handler, StringTokens *tokens)
{
   String request = tokens->nextToken();

   // List relayed channels
   for (Daemon::channel_map_t::iterator it = Daemon::channels.begin();
	it != Daemon::channels.end(); it++) {
      handler->sendNumeric(Numerics::RPL_LIST,
			   String::printf("%s %d :%s",
					  (char const *)(*it).second->name,
					  (*it).second->members.size(),
					  (char const *)(*it).second->topic));
   }

   // Then local channels
   for (Daemon::channel_map_t::iterator it = 
	Daemon::localChannels.begin();
	it != Daemon::localChannels.end(); 
	it++) {
      handler->sendNumeric(Numerics::RPL_LIST,
			   String::printf("%s %d :%s",
					  (char const *)(*it).second->name,
					  (*it).second->members.size(),
					  (char const *)(*it).second->topic));
   }
   
   // Send the end of the list message
   handler->sendNumeric(Numerics::RPL_LISTEND, String(':') +
			handler->lang(Language::L_RPL_LISTEND));
}


/* parseLOCOPS
 * Original 19/09/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::parseLOCOPS(irc2userHandler *handler, StringTokens *tokens)
{
   String message = tokens->rest();
   
   // Check that we have been given something
   if (
#ifdef MINLEN_OP_BROADCAST
       message.length() >= MINLEN_OP_BROADCAST
#else
       message.length()
#endif
       ) {
      // Strip the colon if it is there (smart client if it is :)
      if (message[0] == ':') {
	 message = message.subString(1);
      }
   
      Daemon::
	serverNotice(LocalUser::SN_LOCOPS,
		     String::printf((char *)Language::L_SERVNOTICE_LOCOPS,
				    ((char const *)
				     handler->user->nickname), 
				    (char const *)message));
      return;
   }
   
   // Complain bitterly!
#ifdef MINLEN_OP_BROADCAST
   handler->sendNumeric(Numerics::ERR_NOTEXTTOSEND, String(':') +
			handler->lang(Language::L_ERR_NOTEXTTOSEND_NEEDMORE));
#else
   handler->sendNumeric(Numerics::ERR_NOTEXTTOSEND, String(':') +
			handler->lang(Language::L_ERR_NOTEXTTOSEND));
#endif
}


/* parseLUSERS
 * Original 27/08/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::parseLUSERS(irc2userHandler *handler, StringTokens *tokens)
{
   String mask = tokens->nextToken();
   String server = tokens->nextToken();
   
   // Server not specified, or it is us?
   if (!server.length()) {
      doLUSERS(handler, handler->user, mask);
   } else {
      Server *s = Daemon::getServer(StringMask(server));

      // Check
      if (!s) {
	 // Ok.. well try and look up a user to grab a server..
	 User *u = Daemon::getUser(server);
	 
	 // Check
	 if (!u) {
	    handler->sendNumeric(Numerics::ERR_NOSUCHSERVER,
				 server + " :" +
				 handler->lang(Language::L_ERR_NOSUCHSERVER));
	    return;
	 }
	 
	 // Copy the server pointer across
	 s = u->getServer();
      }
   
      // Is this US??
      if (s == Daemon::myServer()) {
	 doLUSERS(handler, handler->user, mask);
	 return;
      }
   
      // Poll the remote server
      Daemon::routeTo(s)->callLUSERS(s, handler->user, mask);
   }
}


/* parseMAP
 * Original 01/11/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::parseMAP(irc2userHandler *handler, StringTokens *tokens)
{
   String server = tokens->nextToken();
   
   // Server not specified, or it is us?
   if (!server.length()) {
      doMAP(handler, handler->user);
   } else {
      Server *s = Daemon::getServer(StringMask(server));
      
      // Check
      if (!s) {
	 // Ok.. well try and look up a user to grab a server..
	 User *u = Daemon::getUser(server);
	 
	 // Check
	 if (!u) {
	    handler->sendNumeric(Numerics::ERR_NOSUCHSERVER,
				 server + " :" +
				 handler->lang(Language::L_ERR_NOSUCHSERVER));
	    return;
	 }
	 
	 // Copy the server pointer across
	 s = u->getServer();
      }
   
      // Is this US??
      if (s == Daemon::myServer()) {
	 doMAP(handler, handler->user);
	 return;
      }
   
      // Poll the remote server
      Daemon::routeTo(s)->callMAP(s, handler->user);
  }
}


/* parseMODE
 * Original 25/08/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::parseMODE(irc2userHandler *handler, StringTokens *tokens)
{
   // Check we have at least one parameter
   if (tokens->countTokens() < 2) {
      handler->sendNumeric(Numerics::ERR_NEEDMOREPARAMS,
			   String("MODE :") + 
			   handler->lang(Language::L_ERR_NEEDMOREPARAMS));
      return;
   }

   // Grab the target and the modes string
   String target = tokens->nextToken();
   String modes = tokens->nextToken();
   
   // Is the target a user or a channel
   if (Channel::isChannel(target)) {
      // Grab the channel
      Channel *c = Daemon::getChannel(target);
      
      // Make sure we got that channel
      if (!c) {
	 handler->sendNumeric(Numerics::ERR_NOSUCHCHANNEL,
			      target + " :" +
			      handler->lang(Language::L_ERR_NOSUCHCHANNEL));
	 return;
      }
      
      // Check if this user can see this channel
      if (c->isHidden() &&
	  !c->getMember(handler->user)) {
	 handler->sendNumeric(Numerics::ERR_NOTONCHANNEL,
			      c->name + " :" + 
			      handler->lang(Language::L_ERR_NOTONCHANNEL));
	 return;
      }
      
      // Are we setting modes or just getting the modes
      if (modes.length()) {
	 // Changing modes/Receiving specific mode information. Fob it off..
	 c->processModes(handler, handler->user, modes, tokens);
      } else {
	 // Send the channel modes for this channel
	 handler->sendNumeric(Numerics::RPL_CHANNELMODEIS,
			      c->name + " " + Channel::makeModes(c));
	 
	 // Also, send the creation time for the channel
	 handler->sendNumeric(Numerics::RPL_CREATIONTIME,
			      String::printf("%s %lu",
					     (char const *)c->name,
					     c->creationTime));
      }
      
      return;
   } 
   
   // Ok; Perhaps this is this user?
   if (target.IRCtoLower() == handler->user->nickname.IRCtoLower()) {
      // Are we setting modes or just getting the modes
      if (modes.length()) {
	 // Call the processing function
	 String modeString = handler->processUserModes(modes, tokens);
	 
	 // If the mode string has anything in it, send the user the changes
	 if (modeString.length()) {
	    handler->sendUserMode(0, modeString);
	 }
	 
   	 return;
      }
      
      // Show the modes (with the parameters)
      handler->sendNumeric(Numerics::RPL_UMODEIS, User::makeModes(handler->user));
      return;
   }

#ifdef HELPERS_CAN_SEE_USER_MODES   
   // If this user is a helper, we can continue
   if (!User::isHelper(handler->user)) {
      handler->
	sendNumeric(Numerics::ERR_USERSDONTMATCH, String(':') +
		    handler->lang(Language::L_ERR_USERSDONTMATCH_MODE));
      return;
   }
   
   // Look the user up
   User *u = Daemon::getUser(target);
   
   // Check, show the modes if possible
   if (u) {
      // Show the modes (with the parameters)
      handler->sendNumeric(Numerics::RPL_OTHERUMODEIS, 
			   String::printf("%s %s",
					  (char const *)u->nickname,
					  (char const *)User::makeModes(u)));
      return;
   }
#endif
   
   // Ok perhaps not. If this user is not an oper, skip the last check
   if (!User::isOper(handler->user)) {
      handler->sendNumeric(Numerics::ERR_NOSUCHNICK,
			   target + " :" +
			   handler->lang(Language::L_ERR_NOSUCHNICK));
      return;
   }
   
   // Woah, must be a server! Try and find this server
   Server *server = Daemon::getServer(StringMask(target));
   
   // Check
   if (!server) {
      handler->
	sendNumeric(Numerics::ERR_NOSUCHNICK,
		    target + " :" +
		    handler->lang(Language::L_ERR_NOSUCHNICK_OR_SERVER));
      return;
   }

   // Is this server unlocked for mode changes?
   if (server->isModeSet(Server::M_MODELOCK)) {
      handler->sendNumeric(Numerics::ERR_SERVERMODELOCK,
			   server->getHostname() + " :" +
			   handler->lang(Language::L_ERR_SERVERMODELOCK));
      return;
   }
   
   // OK! Are we setting modes or just getting the modes
   if (modes.length()) {
      Daemon::changeServerMode(server, handler, handler->user, modes,
				  tokens);
      return;
   }

   // Just show them. PHEW!
   handler->sendNumeric(Numerics::RPL_SERVMODEIS, 
			String::printf("%s %s",
				       (char const *)server->getHostname(),
				       (char const *)Server::makeModes(server)));
}


/* parseMOTD
 * Original 13/08/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::parseMOTD(irc2userHandler *handler, StringTokens *tokens)
{
   String server = tokens->nextToken();
   
   // Server not specified, or it is us?
   if (!server.length()) {
      doMOTD(handler, handler->user);
   } else {
      Server *s = Daemon::getServer(StringMask(server));
      
      // Check
      if (!s) {
	 // Ok.. well try and look up a user to grab a server..
	 User *u = Daemon::getUser(server);
	 
	 // Check
	 if (!u) {
	    handler->sendNumeric(Numerics::ERR_NOSUCHSERVER,
				 server + " :" +
				 handler->lang(Language::L_ERR_NOSUCHSERVER));
	    return;
	 }
	 
	 // Copy the server pointer across
	 s = u->getServer();
      }
   
      // Is this US??
      if (s == Daemon::myServer()) {
	 doMOTD(handler, handler->user);
	 return;
      }
   
      // Poll the remote server
      Daemon::routeTo(s)->callMOTD(s, handler->user); 
  }
}


/* parseNAMES
 * Original 15/08/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::parseNAMES(irc2userHandler *handler, StringTokens *tokens)
{
   // Check we have at least one parameter
   if (tokens->countTokens() < 2) {
      handler->sendNumeric(Numerics::ERR_NEEDMOREPARAMS,
			   String("NAMES :") + 
			   handler->lang(Language::L_ERR_NEEDMOREPARAMS));
      return;
   }
   
   // Call our helper to do the dirty work..
   String param = tokens->nextToken();
   handler->doNAMES(param);
}


/* parseNICK
 * Original 24/08/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::parseNICK(irc2userHandler *handler, StringTokens *tokens)
{
   // Grab the nickname
   String nick = StringTokens(tokens->nextColonToken()).nextToken();
   
   // Make sure we got a nickname..
   if (!nick.length()) {
      handler->sendNumeric(Numerics::ERR_NONICKNAMEGIVEN, String(':') +
			   handler->lang(Language::L_ERR_NONICKNAMEGIVEN));
      return;
   }
   
   // Check the nickname for an acceptable length and ok characters
   if (!User::okName(nick)) {
      handler->sendNumeric(Numerics::ERR_ERRONEUSNICKNAME,
			   nick + " :" +
			   handler->lang(Language::L_ERR_ERRONEUSNICKNAME));
      return;
   }
   
   // Checks that IRC operators can bypass
   if (!User::isOper(handler->user)) {
      // Check if this nickname is marked on the fail mask list
      String reason = Daemon::failedNickname(nick);
      if (reason.length()) {
	 // Tell the user they cannot join, giving them the reaso
	 handler->
	   sendNumeric(Numerics::ERR_ERRONEUSNICKNAME,
		       String::printf("%s :%s (%s)",
				      (char const *)nick,
				      (char const *)handler->lang(Language::L_ERR_ERRONEUSNICKNAME),
				      (char const *)reason));
	 return;
      }
      
      // Check if this user is already at this nickname
      if (handler->user->nickname == nick) {
	 // Fake a nickname change, since none happened (stupid client)
	 handler->sendNickChange(handler->user, handler->user->nickname);
	 return;
      }
      
      /* Make sure this user is not on a channel marked with the event flag.
       * This only effects users that are not opped or voiced in that channel
       */
      for (User::channel_map_t::iterator it = handler->user->channels.begin();
	   it != handler->user->channels.end(); it++) {
	 ChannelMember *cm = (*it).second->getMember(handler->user);

	 // Sanity check
	 if (cm) {
	    if (!cm->isPrivileged() &&
		((*it).second->modes & Channel::M_EVENT)) {
	       handler->
		 sendNumeric(Numerics::ERR_EVENTNICKCHANGE,
			     (*it).second->name + " :" +
			     handler->lang(Language::L_ERR_EVENTNICKCHANGE));
	       return;
	    }
	 }
      }
   }
   
   /* Check that the nickname is not already in use by someone else.
    * Note this check should be last as it is considered the most 'expensive'
    * since it uses the master user list for a lookup.
    */
   if (!(handler->user->nickname.IRCtoLower() == nick.IRCtoLower()) &&
       Daemon::getUser(nick)) {
      handler->sendNumeric(Numerics::ERR_NICKNAMEINUSE,
			   nick + " :" +
			   handler->lang(Language::L_ERR_NICKNAMEINUSE));
      return;
   }

   // Tell the user we are changing their nickname
   handler->sendNickChange(handler->user, nick);

   // Do the change (this also broadcasts the nick change)
   Daemon::changeUserNick(handler->user, nick);
}


/* parseNOTICE
 * Original 01/09/01, Simon Butcher <pickle@austnet.org>
 * Note: Exactly the same as PRIVMSG, except we are not allowed to reply
 *       to an error at all. This is wrong in many servers which just block
 *       the RPL_AWAY. Read the RFC you lazy ircd programmers!!! :)
 */
void irc2userHandler::parseNOTICE(irc2userHandler *handler, StringTokens *tokens)
{
   String targetStr(tokens->nextToken());
   
   // Check the targets
   if (!targetStr.length()) {
      return;
   }
   
   // Grab our fields
   StringTokens targets(targetStr);
   String message = tokens->nextColonToken();

   // Check the length...
   if (!message.length()) {
      return;
   }
   
   // Run through the list of recipients
   for (String iter = targets.nextToken(','); iter.length();
	iter = targets.nextToken(',')) {
      // Is this a channel or a user?
      if (Channel::isChannel(iter)) {
	 Channel *c = 0;
	 
	 // Most of the time the user will be on the channel. Check their list
	 c = handler->user->getChannel(iter);
	 
	 // Check.
	 if (!c) {
	    // Try and get the channel from the main channel list
	    c = Daemon::getChannel(iter);
	 }
	 
	 // Check.
	 if (!c) {
	    if (!User::isOper(handler->user)) {
	       continue;
	    }
	 } else {
	 // Grab the member record for this user
	    ChannelMember *cm = c->getMember(handler->user);
	    
	    // Check that it worked, most of the time it will
	    if (cm) {
	       /* Check if this user is not +o/+v. Chan operators/voiced ppl 
		* can bypass these checks since they are obviously given the 
		* right to talk
		*/
	       if (!cm->isPrivileged()) {
		  // Check if this channel is moderated
		  if (c->modes & Channel::M_MODERATED) {
		     continue;
		  }
		  
		  // Check if this is a registered user only channel
		  if ((c->modes & Channel::M_REGNICKSONLY) && 
		      !(handler->user->modes & User::M_IDENTIFIED)) {
		     continue;
		  }
		  
		  // Check if this user is banned
		  if (c->onBan(handler->user) && 
		      !c->onBanExcept(handler->user)) {
		     continue;
		  }
	       }
	    } else {
	       // Check if the user can still send while not on the channel
	       if (c->modes & Channel::M_NOOUTSIDEMSG) {
		  continue;
	       }
	       
	       // Check if this is a registered user only channel
	       if ((c->modes & Channel::M_REGNICKSONLY) && 
		   !(handler->user->modes & User::M_IDENTIFIED)) {
		  continue;
	       }
	       
	       // Check if this user is banned from this channel
	       if (c->onBan(handler->user) && !c->onBanExcept(handler->user)) {
		  continue;
	       }
	    }
	    
	    // Send this message to the channel, finally!!
	    c->sendNotice(handler->user, message);
	    continue;
	 }
      } else {
	 // We might be able to save some time here, is this user themselves?
	 if (iter.IRCtoLower() != handler->user->nickname.IRCtoLower()) {
	    // Look for this user in the userlist
	    User *u = Daemon::getUser(iter);
	    
	    // Did we not get a user record and this user is not an oper?
	    if (!u) {
	       if (!User::isOper(handler->user)) {
		  continue;
	       }
	    } else {
	       // Check if this user is allowed to send to this user
	       if ((u->modes & User::M_REGNICKSMSG) &&
		   (!(handler->user->modes & User::M_IDENTIFIED) ||
		    !User::isOper(handler->user))) {
		  continue;
	       }
	       
	       // Check if this user has not been silenced
	       if (u->isSilencing(handler->user)) {
		  continue;
	       }
	       
	       /* Check if this user is doing the caller-id thing (+g) and
		* we are on their ACCEPT list to be able to send to them...
		*/
	       if (u->isModeSet(User::M_IGNORING) &&
		   !u->isAccepting(handler->user)) {
		  continue;
	       }

	       // Send this message to the user
	       Daemon::routeTo(u)->sendNotice(handler->user, u, message);
	       continue;
	    }
	 } else {
	    // Send the message right back at them without doing anything fancy
	    handler->sendNotice(handler->user, handler->user, message);
	    continue;
	 }
      }
      
      // If we got down here, the user must be an operator, perhaps its a mask?
      handler->sendNumeric(Numerics::RPL_DUMMY_NUMERIC,
			   ":Oops, sorry, havn't coded this yet :(");
      
      // Check it is a mask
      
#ifdef TRADITIONAL_IRCOP_MESSAGES
      // Check that the target contains at least one decimal point and stuff
#endif
      
      // Work out what kind of mask this is
      switch ((iter)[0]) {
       case '$': // server mask
	 continue;
       case '#': // host mask
	 continue;
      } 
   }
}


/* parseOPER
 * Original 08/09/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::parseOPER(irc2userHandler *handler, StringTokens *tokens)
{
   // Check we have at least one parameter
   if (tokens->countTokens() < 3) {
      handler->sendNumeric(Numerics::ERR_NEEDMOREPARAMS,
			   String("OPER :") + 
			   handler->lang(Language::L_ERR_NEEDMOREPARAMS));
      return;
   }

   // Check that this user is not already an oper
   if (User::isOper(handler->user)) {
      handler->sendNumeric(Numerics::RPL_YOUREOPER, String(':') +
			   handler->lang(Language::L_RPL_YOUREOPER_ALREADY));
      return;
   }
   
   // Check if the server is in NOOPER mode
   if (Daemon::myServer()->isModeSet(Server::M_NOOP)) {
      handler->sendNumeric(Numerics::ERR_NOOPERHOST, String(':') +
			   handler->lang(Language::L_ERR_NOOPERHOST_NOOP));
      return;
   }

   // Grab the nickname and password pair
   String nickname = tokens->nextToken();
   String password = tokens->nextToken();
   
   // Get the operator
   Operator *oper = Daemon::getOperator(nickname);
   
   // Check.
   if (!oper) {
      handler->sendNumeric(Numerics::ERR_NOOPERHOST, String(':') +
			   handler->lang(Language::L_ERR_NOOPERHOST));
      return;
   }
   
   // Try this password against this operator
   if (oper->getPassword() != 
       Operator::makeOperPassword(nickname, password)) {
      handler->sendNumeric(Numerics::ERR_PASSWDMISMATCH, String(':') +
			   handler->lang(Language::L_ERR_PASSWDMISMATCH));
      return;
   }

   // Work out what log messages to send, and what modes to set this user
   User::modes_t modes = 0;
   if (oper->isGlobal()) {
      // Log this action
      Daemon::logger(handler->user->nickname + " is now a global operator",
		      LOGPRI_NOTICE);

      // Count this oper in on the number of global opers/helpers on-line
      Daemon::numOpers++;
      Daemon::numHelpers++;
      
      // Fix the modes
#ifdef GIVE_MODES_GLOBALOP
      modes = User::M_GLOBALOPER | User::M_HELPER | GIVE_MODES_GLOBALOP;
#else
      modes = User::M_GLOBALOPER | User::M_HELPER;
#endif
   } else {
      // Log this action
      Daemon::logger(handler->user->nickname + " is now a local operator",
		      LOGPRI_NOTICE);
      
      // Fix the modes
#ifdef GIVE_MODES_LOCALOP
      modes = User::M_LOCALOPER | GIVE_MODES_LOCALOP;
#else
      modes = User::M_LOCALOPER;
#endif
   }
   
   /* Send the user their new modes. This messy looking slab here makes sure
    * only the appropriate modes are went to the user to avoid client
    * confusion.
    */
   handler->sendUserMode(0, User::makeModes(handler->user->modes ^ 
					    (handler->user->modes | modes)));
   handler->user->modes |= modes;
   
   // Tell the user they are now an IRC Operator, finally!
   handler->sendNumeric(Numerics::RPL_YOUREOPER, String(':') +
			handler->lang(Language::L_RPL_YOUREOPER));
}


/* parsePART
 * Original 18/08/01, Simon Butcher <pickle@austnet.org>
 * Note: We have no need for 'ERR_NOSUCHCHANNEL' here, since we look up
 *       the channel differently to how the ircd used in the RFC does
 */
void irc2userHandler::parsePART(irc2userHandler *handler, StringTokens *tokens)
{
   // Check we have at least one parameter
   if (tokens->countTokens() < 2) {
      handler->sendNumeric(Numerics::ERR_NEEDMOREPARAMS,
			   String("PART :") + 
			   handler->lang(Language::L_ERR_NEEDMOREPARAMS));
      return;
   }
   
   // This command effects the user idle time, so we must change it here
   handler->getConnection()->touch();
   
   // Grab the variables we need
   StringTokens channels(tokens->nextToken());
   String reason = tokens->nextColonToken();
   
   // Run through the parameter tokens
   for (String chan = channels.nextToken(','); chan.length(); 
	chan = channels.nextToken(',')) {
      // Try to grab the channel
      Channel *c = handler->user->getChannel(chan);

      if (!c) {
	 handler->sendNumeric(Numerics::ERR_NOTONCHANNEL,
			      chan + " :" +
			      handler->lang(Language::L_ERR_NOTONCHANNEL));
	 continue;
      } 
      
      // Do the part
      Daemon::partChannel(c, handler->user, reason);
   }
}


/* parsePING
 * Original 25/08/01, Simon Butcher <pickle@austnet.org>
 * Note: This obviously does not conform to standards. We simple take
 *       whatever it is this user sent us and send it right on back at
 *       them. Users do not have access to throw ping packets all over
 *       the network, and if they need to ping users can can always use
 *       CTCP ping.
 */
void irc2userHandler::parsePING(irc2userHandler *handler, StringTokens *tokens)
{
   handler->sendPong(tokens->rest());
}


/* parsePONG
 * Original , Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::parsePONG(irc2userHandler *handler, StringTokens *tokens)
{
   handler->sendNumeric(Numerics::RPL_DUMMY_NUMERIC, ":Oops, sorry, havn't coded this yet :(");
}


/* parsePRIVMSG
 * Original 01/09/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::parsePRIVMSG(irc2userHandler *handler, StringTokens *tokens)
{
   String targetStr(tokens->nextToken());
   
   // Check the targets
   if (!targetStr.length()) {
      handler->sendNumeric(Numerics::ERR_NORECIPIENT, String(':') +
			   handler->lang(Language::L_ERR_NORECIPIENT));
      return;
   }
   
   // Grab our fields
   StringTokens targets(targetStr);
   String message = tokens->nextColonToken();

   // This command effects the user idle time, so we must change it here
   handler->getConnection()->touch();
   
   // Check the length...
   if (!message.length()) {
      handler->sendNumeric(Numerics::ERR_NOTEXTTOSEND, String(':') +
			   handler->lang(Language::L_ERR_NOTEXTTOSEND));
      return;
   }
   
   // Check the message here - ctcp dcc ??
   
   // Run through the list of recipients
   for (String iter = targets.nextToken(','); iter.length();
	iter = targets.nextToken(',')) {
      // Is this a channel or a user?
      if (Channel::isChannel(iter)) {
	 Channel *c = 0;
	 
	 // Most of the time the user will be on the channel. Check their list
	 c = handler->user->getChannel(iter);
	 
	 // Check.
	 if (!c) {
	    // Try and get the channel from the main channel list
	    c = Daemon::getChannel(iter);
	 }
	 
	 // Check.
	 if (!c) {
	    if (!User::isOper(handler->user)) {
	       handler->
		 sendNumeric(Numerics::ERR_NOSUCHCHANNEL,
			     iter + " :" +
			     handler->lang(Language::L_ERR_NOSUCHCHANNEL));
	       continue;
	    }
	 } else {
	    // Grab the member record for this user
	    ChannelMember *cm = c->getMember(handler->user);
	    
	    // Check that it worked, most of the time it will
	    if (cm) {
	       /* Check if this user is not +o/+v. Chan operators/voiced ppl 
		* can bypass these checks since they are obviously given the 
		* right to talk
		*/
	       if (!cm->isPrivileged()) {
		  // Check if this channel is moderated
		  if (c->modes & Channel::M_MODERATED) {
		     handler->
		       sendNumeric(Numerics::ERR_CANNOTSENDTOCHAN,
				   iter + " :" +
				   handler->lang(Language::L_ERR_CANNOTSENDTOCHAN_MODERATED));
		     continue;
		  }
		  
		  // Check if this is a registered user only channel
		  if ((c->modes & Channel::M_REGNICKSONLY) && 
		      !(handler->user->modes & User::M_IDENTIFIED)) {
		     handler->
		       sendNumeric(Numerics::ERR_CANNOTSENDTOCHAN,
				   iter + " :" +
				   handler->lang(Language::L_ERR_CANNOTSENDTOCHAN_REGNICKSONLY));
		     continue;
		  }
		  
		  // Check if this user is banned
		  if (c->onBan(handler->user) && 
		      !c->onBanExcept(handler->user)) {
		     handler->
		       sendNumeric(Numerics::ERR_CANNOTSENDTOCHAN,
				   iter + " :" +
				   handler->lang(Language::L_ERR_CANNOTSENDTOCHAN_BANNED));
		     continue;
		  }
	       }
	    } else {
	       // Check if the user can still send while not on the channel
	       if (c->modes & Channel::M_NOOUTSIDEMSG) {
		  handler->
		    sendNumeric(Numerics::ERR_CANNOTSENDTOCHAN,
				iter + " :" +
				handler->lang(Language::L_ERR_CANNOTSENDTOCHAN_NOOUTSIDEMSG));
		  continue;
	       }
	       
	       // Check if this is a registered user only channel
	       if ((c->modes & Channel::M_REGNICKSONLY) && 
		   !(handler->user->modes & User::M_IDENTIFIED)) {
		  handler->
		    sendNumeric(Numerics::ERR_CANNOTSENDTOCHAN,
				iter + " :" +
				handler->lang(Language::L_ERR_CANNOTSENDTOCHAN_REGNICKSONLY));
		  continue;
	       }
	       
	       // Check if this user is banned from this channel
	       if (c->onBan(handler->user) && !c->onBanExcept(handler->user)) {
		  handler->
		    sendNumeric(Numerics::ERR_CANNOTSENDTOCHAN,
				iter + " :" +
				handler->lang(Language::L_ERR_CANNOTSENDTOCHAN_BANNED));
		  continue;
	       }
	    }
	    
	    // Send this message to the channel, finally!!
	    c->sendPrivmsg(handler->user, message);
	    continue;
	 }
      } else {
	 // We might be able to save some time here, is this user themselves?
	 if (iter.IRCtoLower() != handler->user->nickname.IRCtoLower()) {
	    // Look for this user in the userlist
	    User *u = Daemon::getUser(iter);
	    
	    // Did we not get a user record and this user is not an oper?
	    if (!u) {
	       if (!User::isOper(handler->user)) {
		  handler->
		    sendNumeric(Numerics::ERR_NOSUCHNICK,
				iter + " :" +
				handler->lang(Language::L_ERR_NOSUCHNICK_NICK));
		  continue;
	       }
	    } else {
	       // Check if this user is allowed to send to this user
	       if ((u->modes & User::M_REGNICKSMSG) &&
		   (!(handler->user->modes & User::M_IDENTIFIED) ||
		    !User::isOper(handler->user))) {
		  handler->
		    sendNumeric(Numerics::ERR_CANNOTSENDTONICK,
				iter + " :" +
				handler->lang(Language::L_ERR_CANNOTSENDTONICK_REGNICKSONLY));
		  continue;
	       }
	       
	       // Check if this user has not been silenced
	       if (u->isSilencing(handler->user)) {
		  continue;
	       }
	       
	       /* Check if this user is doing the caller-id thing (+g) and
		* we are on their ACCEPT list to be able to send to them...
		*/
	       if (u->isModeSet(User::M_IGNORING) &&
		   !u->isAccepting(handler->user)) {
		  // Tell the user that their message was not delivered
		  handler->
		    sendNumeric(Numerics::RPL_NOTACCEPTED, 
				u->getNickname() + " :" +
				handler->lang(Language::E_RPL_NOTACCEPTED));
		  
		  // Send a request to the remote user
#ifdef FLOODLOCK_ACCEPT_MSG
		  if (!handler->user->local->onAcceptMessageFloodlock(u)) {
#endif
		     Daemon::routeTo(u)->
		       sendNumeric(Daemon::myServer(),
				   Numerics::RPL_ACCEPTNOTICE, handler->user,
				   String::printf("%s %s %s :%s",
						  (char const *)handler->user->getNickname(),
						  (char const *)handler->user->getUsername(),
						  (char const *)handler->user->getHost(u),
						  (char const *)u->lang(Language::E_RPL_ACCEPTNOTICE)));
#ifdef FLOODLOCK_ACCEPT_MSG
		  }
#endif
		  continue;
	       }

	       // Send this message to the user
	       Daemon::routeTo(u)->sendPrivmsg(handler->user, u, message);
	       
	       // If this user is 'away', tell the user
	       if (u->awayMessage.length()) {
#ifdef FLOODLOCK_AWAY_REPLY
		  if (!handler->user->local->onAwayReplyFloodlock(u)) {
#endif
		     handler->
		       sendNumeric(Numerics::RPL_AWAY,
				   String::printf("%s :%s",
						  (char const *)u->nickname,
						  (char const *)u->awayMessage));
#ifdef FLOODLOCK_AWAY_REPLY
		  }
#endif
	       }
	       continue;
	    }
	 } else {
	    // Send the message right back at them without doing anything fancy
	    handler->sendPrivmsg(handler->user, handler->user, message);
	    continue;
	 }
      }
      
      // If we got down here, the user must be an operator, perhaps its a mask?
      handler->sendNumeric(Numerics::RPL_DUMMY_NUMERIC,
			   ":Oops, sorry, havn't coded this yet :(");
      
      // Check it is a mask
      
#ifdef TRADITIONAL_IRCOP_MESSAGES
      // Check that the target contains at least one decimal point and stuff
#endif
      
      // Work out what kind of mask this is
      switch ((iter)[0]) {
       case '$': // server mask
	 continue;
       case '#': // host mask
	 continue;
      } 
      
      // If we got here, we did not find a match, complain to the user
      handler->sendNumeric(Numerics::ERR_NOSUCHNICK,
			   iter + " :" +
			   handler->lang(Language::L_ERR_NOSUCHNICK));
   }
}


/* parseQUIT
 * Original 14/08/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::parseQUIT(irc2userHandler *handler, StringTokens *tokens)
{
   String reason = tokens->nextColonToken();
   
   /* Most clients don't care if we tell them goodbye or not, but the RFCs
    * say we should be nice to our clients, so we may as well say bye bye
    * to them :)
    */
   if (reason.length()) {
      handler->getConnection()->
	goodbye(String::printf((char *)Language::L_BYE_BYE_USER,
			       (char const *)reason));
   } else {
      handler->getConnection()->goodbye();
   }
}


/* parseREHASH
 * Original 19/09/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::parseREHASH(irc2userHandler *handler, StringTokens *tokens)
{
   // This command effects the user idle time, so we must change it here
   handler->getConnection()->touch();
   
   // Rehash.. gee this is hard!
   Daemon::rehash(handler, handler->user);
}


/* parseRESTART
 * Original 28/10/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::parseRESTART(irc2userHandler *handler, StringTokens *tokens)
{
   // Restart.. eek
   Daemon::restart(handler, handler->user);
}


/* parseSERVLIST - List services visible and on-line
 * Original , Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::parseSERVLIST(irc2userHandler *handler, StringTokens *tokens)
{
   StringMask mask("*");
   String type = "0";
   
   handler->sendNumeric(Numerics::RPL_SERVLIST,
			"ChanOP services.austnet.org * 0 1 :ChanOP service (test)");
   handler->sendNumeric(Numerics::RPL_SERVLIST,
			"NickOP services.austnet.org * 0 1 :NickOP service (test)");
   
   // Send the end of services list numerics
   handler->sendNumeric(Numerics::RPL_SERVLISTEND,
			mask.getMask() + " " + type + " :" +
			handler->lang(Language::L_RPL_SERVLISTEND));
}


/* parseSILENCE - Block a user from sending a message (dalnet/undernet style)
 * Original 23/09/01, Simon Butcher <pickle@austnet.org>
 * Note: If I invented this command I would have at least put multiple target
 *       possibilities in it. Adding that would be kind of pointless now that
 *       this command features such widespread use.. :(
 */
void irc2userHandler::parseSILENCE(irc2userHandler *handler, StringTokens *tokens)
{
   String param = tokens->nextToken();
   StringMask mask;
   User *u = 0;
   
   // Are we listing or doing something else?
   if (param.length()) {
      // Check what we should do with this parameter
      switch (param[0]) {
       case '+': // Adding a mask
	   {
	      param = param.subString(1);
	      
	      // Check if their silence list is too big
	      if (handler->user->silences.size() > 
		  DEFAULT_MAX_SILENCES_PER_USER) {
		 handler->
		   sendNumeric(Numerics::ERR_SILELISTFULL,
			       param + " :" +
			       handler->lang(Language::L_ERR_SILELISTFULL));
		 return;
	      }
	      
	      // Grab the mask
	      mask = Utils::fixToIdentityMask(param);

	      // Check if the user is trying to ignore themselves (stupid)
	      if (mask.matches(handler->user->getAddress()) ||
		  mask.matches(handler->user->getVWAddress())) {
		 // Tell them why we did nothing??
		 return;
	      }
	      
	      // Add the mask
	      if (Daemon::addUserSilence(handler->user, mask)) {
		 // Send confirmation to the client
		 handler->sendSilence(handler->user, true, mask);
	      }
	   }
	 return;
       case '-': // Removing a mask
	   {
	      param = param.subString(1);
	      
	      // Grab the mask
	      mask = Utils::fixToIdentityMask(param);
	      
	      // Add the mask
	      if (Daemon::delUserSilence(handler->user, mask)) {
		 // Send confirmation to the client
		 handler->sendSilence(handler->user, false, mask);
	      }
	   }
	 return;
      }
      
      /* If we got here, we must be requesting a silence list of a user. We
       * need to be an operator or a helper to do this, however.
       */
      if (!(handler->user->modes & User::M_GLOBALOPER) &&
	  !(handler->user->modes & User::M_LOCALOPER) &&
	  !(handler->user->modes & User::M_HELPER)) {
	 /* We SHOULD tell them permission denied or something but *sigh* this
	  * appears to be how dalnet ppl do it. Anyone think we should just
	  * send a customised permission denied?
	  */
	 handler->sendNumeric(Numerics::RPL_ENDOFSILELIST, String(" :") +
			      handler->lang(Language::L_RPL_ENDOFSILELIST));
	 return;
      }
      
      // Look up the requested user, this will follow through to the list below
      u = Daemon::getUser(param);
      
      // Check
      if (!u) {
	 handler->sendNumeric(Numerics::ERR_NOSUCHNICK,
			      param + " :" +
			      handler->lang(Language::L_ERR_NOSUCHNICK_NICK));
	 return;
      }
   } else {
      // Set the user to this one, since we are just listing
      u = handler->user;
   }
   
   // If we got here we must be listing the masks
   for (User::silence_list_t::iterator it = u->silences.begin();
	it != u->silences.end(); it++) {
      // Send this entry
      handler->sendNumeric(Numerics::RPL_SILELIST,
			   String::printf("%s %s",
					  (char const *)u->nickname,
					  (char const *)(*it).getMask()));
   }
   
   // Send the end of the list numeric
   handler->sendNumeric(Numerics::RPL_ENDOFSILELIST, 
			Language::L_RPL_ENDOFSILELIST);
}


/* parseSQUIT
 * Original , Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::parseSQUIT(irc2userHandler *handler, StringTokens *tokens)
{
   handler->sendNumeric(Numerics::RPL_DUMMY_NUMERIC, ":Oops, sorry, havn't coded this yet :(");
}


/* parseSTATS
 * Original 14/08/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::parseSTATS(irc2userHandler *handler, StringTokens *tokens)
{
   String request = tokens->nextToken();
   String server = tokens->nextToken();

   // Make sure we have a request
   if (!request.length()) {
      handler->sendNumeric(Numerics::ERR_NEEDMOREPARAMS,
			   String("STATS :") + 
			   handler->lang(Language::L_ERR_NEEDMOREPARAMS));
   }
   
   // This command effects the user idle time, so we must change it here
   handler->getConnection()->touch();
   
   // Server not specified, or it is us?
   if (!server.length()) {
      doSTATS(handler, handler->user, request);
   } else {
      Server *s = Daemon::getServer(StringMask(server));
      
      // Check
      if (!s) {
	 // Ok.. well try and look up a user to grab a server..
	 User *u = Daemon::getUser(server);
	 
	 // Check
	 if (!u) {
	    handler->sendNumeric(Numerics::ERR_NOSUCHSERVER,
				 server + " :" +
				 handler->lang(Language::L_ERR_NOSUCHSERVER));
	    return;
	 }
	 
	 // Copy the server pointer across
	 s = u->getServer();
      }
   
      // Is this US??
      if (s == Daemon::myServer()) {
	 doSTATS(handler, handler->user, request);
	 return;
      }
   
      // Poll the remote server
      Daemon::routeTo(s)->callSTATS(s, handler->user, request);
   }
}


/* parseTIME
 * Original 27/08/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::parseTIME(irc2userHandler *handler, StringTokens *tokens)
{
   String server = tokens->nextToken();
   
   // Server not specified, or it is us?
   if (!server.length()) {
      doTIME(handler, handler->user);
   } else {
      Server *s = Daemon::getServer(StringMask(server));
      
      // Check
      if (!s) {
	 // Ok.. well try and look up a user to grab a server..
	 User *u = Daemon::getUser(server);
	 
	 // Check
	 if (!u) {
	    handler->sendNumeric(Numerics::ERR_NOSUCHSERVER,
				 server + " :" +
				 handler->lang(Language::L_ERR_NOSUCHSERVER));
	    return;
	 }
	 
	 // Copy the server pointer across
	 s = u->getServer();
      }
   
      // Is this US??
      if (s == Daemon::myServer()) {
	 doTIME(handler, handler->user);
	 return;
      }
   
      // Poll the remote server
      Daemon::routeTo(s)->callTIME(s, handler->user);
   }
}


/* parseTOPIC
 * Original 19/08/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::parseTOPIC(irc2userHandler *handler, StringTokens *tokens)
{
   // Check the parameter count
   if (tokens->countTokens() < 2) {
      handler->sendNumeric(Numerics::ERR_NEEDMOREPARAMS,
			   String("TOPIC :") + 
			   handler->lang(Language::L_ERR_NEEDMOREPARAMS));
      return;
   }
   
   // This command effects the user idle time, so we must change it here
   handler->getConnection()->touch();
   
   // Grab the variables
   String channel = tokens->nextToken();
   String topic = tokens->nextColonToken();
   
   // Find the channel!
   Channel *c = handler->user->getChannel(channel);

   // Check
   if (!c) {
      handler->sendNumeric(Numerics::ERR_NOSUCHCHANNEL,
			   channel + " :" +
			   handler->lang(Language::L_ERR_NOSUCHCHANNEL));
      return;
   } 

   // Are we grabbing or setting the topic?
   if (!topic.length()) {
      // Do we have a topic to tell?
      if (c->topic.length()) {
	 handler->sendNumeric(Numerics::RPL_TOPIC,
			      String::printf("%s :%s",
					     (char const *)c->name,
					     (char const *)c->topic));
	 handler->sendNumeric(Numerics::RPL_TOPICWHOTIME,
			      String::printf("%s %s :%lu",
					     (char const *)c->name,
					     (char const *)c->topicWho,
					     c->topicTime));
	 return;
      } 
	
      // No topic...
      handler->sendNumeric(Numerics::RPL_NOTOPIC,
			   c->name + " :" + 
			   handler->lang(Language::L_RPL_NOTOPIC));
      
      return;
   }
   
   // Grab the member record for this user
   ChannelMember *cm = c->getMember(handler->user);
   
   // Check
   if (!cm) {
      handler->sendNumeric(Numerics::ERR_NOTONCHANNEL,
			   channel + " :" +
			   handler->lang(Language::L_ERR_NOTONCHANNEL));
      return;
   }
   
   // Check the access, is the channel +t?
   if (c->modes & Channel::M_TOPICLOCK) {
      // Check if this user is an operator
      if (!cm->isChanOper()) {
	 handler->sendNumeric(Numerics::ERR_CHANOPRIVSNEEDED,
			      channel + " :" +
			      handler->lang(Language::L_ERR_CHANOPRIVSNEEDED));
	 return;
      }
   }

   // Change the topic, finally.
   Daemon::changeChannelTopic(c, handler->user, topic);
}


/* parseTRACE
 * Original , Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::parseTRACE(irc2userHandler *handler, StringTokens *tokens)
{
   handler->sendNumeric(Numerics::RPL_DUMMY_NUMERIC, ":Oops, sorry, havn't coded this yet :(");
}


#ifdef HAVE_CMD_TRACEROUTE
/* parseTRACEROUTE
 * Original , Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::parseTRACEROUTE(irc2userHandler *handler, StringTokens *tokens)
{
   handler->sendNumeric(Numerics::RPL_DUMMY_NUMERIC, ":Oops, sorry, havn't coded this yet :(");
}
#endif


/* parseUSERHOST
 * Original 21/08/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::parseUSERHOST(irc2userHandler *handler, StringTokens *tokens)
{
   // Check we have at least one parameter
   if (tokens->countTokens() < 2) {
      handler->sendNumeric(Numerics::ERR_NEEDMOREPARAMS,
			   String("USERHOST :") + 
			   handler->lang(Language::L_ERR_NEEDMOREPARAMS));
      return;
   }

   int count = 0;
   String reply = ":";

   // Run through the parameter tokens
   for (String iter = tokens->nextToken(); iter.length();
	iter = tokens->nextToken()) {
      User *u = Daemon::getUser(iter);
      count++;

      // Check we have not broken a limit (max 5 requests allowed)
      if (count > 4) {
	 break;
      }
      
      // Check if we have that nick in our userlist
      if (u) {
	 reply = reply + (u->nickname + "=" +
			  ((u->awayMessage.length()) ? "-" : "+") +
			  u->username + "@" +
			  (u->showVW(handler->user) ?
			   u->vwhostname :
			   u->hostname) + " ");
      }
   }
   
   
   // Send the reply
   handler->sendNumeric(Numerics::RPL_USERHOST, reply);
}


/* parseUSERIP
 * Original , Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::parseUSERIP(irc2userHandler *handler, StringTokens *tokens)
{
}


/* parseVERSION
 * Original 24/08/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::parseVERSION(irc2userHandler *handler, StringTokens *tokens)
{
   String server = tokens->nextToken();
   
   // Server not specified, or it is us?
   if (!server.length()) {
      doVERSION(handler, handler->user);
   } else {
      Server *s = Daemon::getServer(StringMask(server));
      
      // Check
      if (!s) {
	 // Ok.. well try and look up a user to grab a server..
	 User *u = Daemon::getUser(server);
	 
	 // Check
	 if (!u) {
	    handler->sendNumeric(Numerics::ERR_NOSUCHSERVER,
				 server + " :" +
				 handler->lang(Language::L_ERR_NOSUCHSERVER));
	    return;
	 }
	 
	 // Copy the server pointer across
	 s = u->getServer();
      }
   
      // Is this US??
      if (s == Daemon::myServer()) {
	 doVERSION(handler, handler->user);
	 return;
      }
   
      // Poll the remote server
      Daemon::routeTo(s)->callVERSION(s, handler->user);
   }
}


/* parseWALLCHOPS
 * Original , Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::parseWALLCHOPS(irc2userHandler *handler, StringTokens *tokens)
{
   handler->sendNumeric(Numerics::RPL_DUMMY_NUMERIC, ":Oops, sorry, havn't coded this yet :(");
}


#ifdef ALLOW_OPER_WALLOPS
/* parseWALLOPS
 * Original 18/09/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::parseWALLOPS(irc2userHandler *handler, StringTokens *tokens)
{
   String message = tokens->nextColonToken();
   
   // Check that we have been given something
   if (
# ifdef MINLEN_OP_BROADCAST
       message.length() >= MINLEN_OP_BROADCAST
# else
       message.length()
# endif
       ) {
      Daemon::broadcastWallops(handler->user, message);
      return;
   }
   
   // Complain bitterly!
# ifdef MINLEN_OP_BROADCAST
   handler->sendNumeric(Numerics::ERR_NOTEXTTOSEND, String(':') +
			handler->lang(Language::L_ERR_NOTEXTTOSEND_NEEDMORE));
# else
   handler->sendNumeric(Numerics::ERR_NOTEXTTOSEND, String(':') +
			handler->lang(Language::L_ERR_NOTEXTTOSEND));
# endif
}
#endif


/* parseWATCH
 * Original 24/09/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::parseWATCH(irc2userHandler *handler, StringTokens *tokens)
{
   bool noAdd = false;
   bool noList = false;
   
   // Run through the parameters
   for (String param = tokens->nextToken(); param.length();
	param = tokens->nextToken()) {
      // Work out what to do with this
      switch (toupper(param[0])) {
       case 'C': // Clear the list
	 handler->user->local->wipeWatches();
	 continue;
       case 'S': // Watch status
	   {
	      handler->
		sendNumeric(Numerics::RPL_WATCHSTAT,
			    String::printf(":%s %u %s",
					   (char const *)handler->lang(Language::B_RPL_WATCHSTAT),
					   handler->user->local->watches.size(),
					   (char const *)handler->lang(Language::E_RPL_WATCHSTAT)));

	      String reply = ":";
	      
	      for (LocalUser::watch_set_t::iterator it = 
		   handler->user->local->watches.begin();
		   it != handler->user->local->watches.end(); it++) {
		 // Add this watch item, neatly..
		 if (reply.length() != 1) {
		    reply = reply + " " + *it;
		 } else {
		    reply = reply + *it;
		 }
		 
		 // Check if we are close to breaking a limit here
		 if (reply.length() > 400) {
		    handler->sendNumeric(Numerics::RPL_WATCHLIST, reply);
		    
		    // reset the reply
		    reply = ":";
		 }
	      }
	      
	      // Anything left to send?
	      if (reply.length()) {
		 handler->sendNumeric(Numerics::RPL_WATCHLIST, reply);
	      }

	      // Send the end of list marker
	      handler->
		sendNumeric(Numerics::RPL_ENDOFWATCHLIST,
			    String::printf(":%s (%c)",
					   (char const *)handler->lang(Language::L_RPL_ENDOFWATCHLIST),
					   param[0]));
	   }
	 continue;
       case 'L': // Another something *sigh*
	 /* If they have already done this, skip it. They do not need to do it
	  * again this time
	  */
	 if (noList) {
	    continue;
	 }
	 
	 noList = true;
	 
	 // Run through the watch list (*sigh*)
	 for (LocalUser::watch_set_t::iterator it = 
	      handler->user->local->watches.begin();
	      it != handler->user->local->watches.end(); it++) {
	    String watch = *it;
	    
	    // Ok, maybe this is a channel? If not, it barely dints cpu..
	    if (Channel::isChannel(watch)) {
	       // Look for this channel
	       Channel *c = Daemon::getChannel(watch);
	       
	       // Check
	       if (c) {
		  handler->
		    sendNumeric(Numerics::RPL_NOWON,
				String::printf("%s * * %lu :%s",
					       ((char const *)
						c->name),
					       c->creationTime,
					       (char const *)handler->lang(Language::E_RPL_NOWON_CHANNEL)));
		  continue;
	       }
	    }
	    
	    // Look for a user, this is the most likely case though
	    User *u = Daemon::getUser(watch);
	    
	    // Check
	    if (u) {
	       handler->
		 sendNumeric(Numerics::RPL_NOWON,
			     String::printf("%s %s %s %lu :%s",
					    ((char const *)
					     u->nickname),
					    ((char const *)
					     u->username),
					    ((char const *)
					     u->getHost(handler->user)),
					    u->lastNickChange,
					    (char const *)handler->lang(Language::E_RPL_NOWON_USER)));
	       continue;
	    }
	    
	    // If this user is an oper, check for a server..
	    if ((handler->user->modes & User::M_GLOBALOPER) ||
		(handler->user->modes & User::M_LOCALOPER) ||
		(handler->user->modes & User::M_HELPER)) {
	       Server *s = Daemon::getServer(watch);
	       
	       // Check
	       if (s) {
		  handler->
		    sendNumeric(Numerics::RPL_NOWON,
				String::printf("%s * * 0 :%s",
					       ((char const *)
						u->nickname),
					       (char const *)handler->lang(Language::E_RPL_NOWON_SERVER)));
		  continue;
	       }
	    }
	    
	    // Only send this if they want it (uppercase L)
	    if (isupper(param[0])) {
	       // If we got here, whatever it is must be offline.
	       handler->
		 sendNumeric(Numerics::RPL_NOWOFF,
			     String::printf("%s * * 0 :%s",
					    (char const *)watch,
					    (char const *)handler->lang(Language::E_RPL_NOWOFF)));
	    }
	 }
	 
	 // Send the end of watch list message
	 handler->
	   sendNumeric(Numerics::RPL_ENDOFWATCHLIST, 
		       String::printf(":%s (%c",
				      (char const *)handler->lang(Language::L_RPL_ENDOFWATCHLIST),
				      param[0]));
	 
	 continue;
       case '+': // Adding a watch
	   {
	      // No more watches allowed to be added?
	      if (noAdd) {
		 // Just ignore it
		 continue;
	      }
	      
	      // Fix up the string
	      param = param.subString(1).IRCtoLower();
	      
	      // Make sure we got something other than just one char
	      if (!param.length()) {
		 continue;
	      }
	      
	      // Make sure the watch list is not too long
	      if (handler->user->local->watches.size() > 
		  DEFAULT_MAX_WATCHES_PER_USER) {
		 // Complain
		 handler->
		   sendNumeric(Numerics::ERR_TOOMANYWATCH,
			       String::printf("%s :%s %u %s",
					      (char const *)param,
					      (char const *)handler->lang(Language::B_ERR_TOOMANYWATCH),
					      DEFAULT_MAX_WATCHES_PER_USER,
					      (char const *)handler->lang(Language::E_ERR_TOOMANYWATCH)));
		 
		 // Stop any more warnings being sent (no flood)
		 noAdd = true;
		 continue;
	      }
	      
	      // Add it.
	      handler->user->local->addWatch(param);
	      
	      /* Ok, we have to confirm this by looking this object up and
	       * reporting its status (eek!).. Firstly, maybe this is a 
	       * channel? If not, it barely dints cpu since it's a basic 
	       * check. 
	       */
	      if (Channel::isChannel(param)) {
		 // Look for this channel
		 Channel *c = Daemon::getChannel(param);
		 
		 // Check
		 if (c) {
		    handler->
		      sendNumeric(Numerics::RPL_NOWON,
				  String::printf("%s * * %lu :%s",
						 ((char const *)
						  c->name),
						 c->creationTime,
						 (char const *)handler->lang(Language::E_RPL_NOWON_CHANNEL)));
		    continue;
		 }
	      }
	      
	      // Look for a user, this is the most likely case though
	      User *u = Daemon::getUser(param);
	      
	      // Check
	      if (u) {
		 handler->sendNumeric(Numerics::RPL_NOWON,
				      String::printf("%s %s %s %lu :%s",
						     ((char const *)
						      u->nickname),
						     ((char const *)
						      u->username),
						     ((char const *)
						      u->getAddress(handler->user)),
						     u->lastNickChange,
						     (char const *)handler->lang(Language::E_RPL_NOWON_USER)));
		 continue;
	      }
	      
	      // If this user is an oper, check for a server..
	      if ((handler->user->modes & User::M_GLOBALOPER) ||
		  (handler->user->modes & User::M_LOCALOPER) ||
		  (handler->user->modes & User::M_HELPER)) {
		 Server *s = Daemon::getServer(param);
		 
		 // Check
		 if (s) {
		    handler->
		      sendNumeric(Numerics::RPL_NOWON,
				  String::printf("%s * * 0 :%s",
						 ((char const *)
						  s->getHostname()),
						 (const char *)handler->lang(Language::E_RPL_NOWON_SERVER)));
		    continue;
		 }
	      }
	      
	      // If we got here, whatever it is must be offline.
	      handler->
		sendNumeric(Numerics::RPL_NOWOFF,
			    String::printf("%s * * 0 :%s",
					   (char const *)param,
					   (char const *)handler->lang(Language::E_RPL_NOWOFF)));
	   }
	 continue;
       case '-': // Removing a watch
	 // Fix up the string
	 param = param.subString(1).IRCtoLower();
	 
	 // Make sure we got something other than just one char
	 if (!param.length()) {
	    continue;
	 }
	 
	 // Delete it
	 handler->user->local->delWatch(param);
	 
	 // Tell the client we deleted it
	 handler->sendNumeric(Numerics::RPL_WATCHOFF,
			      String::printf("%s * * 0 :%s",
					     (char const *)param,
					     (char const *)handler->lang(Language::E_RPL_WATCHOFF)));
	 continue;
      }
   }
}


/* parseWHO
 * Original 22/08/01, Simon Butcher <pickle@austnet.org>
 * Note: In regards to grabbing the channel, perhaps we should loop to find
 *       a non-hidden channel before assuming channel is *? It's a minor
 *       thing; doesn't matter that much.
 */
void irc2userHandler::parseWHO(irc2userHandler *handler, StringTokens *tokens)
{
#ifdef HAVE_IRC2USER_MATCH_COUNTING
   unsigned int matches = 0;
#endif
   
   // Grab the mask the user is looking for
   String maskStr = tokens->nextToken();

   // Are we looking for a channel, or a mask/nickname?
   if (Channel::isChannel(maskStr)) {
      Channel *chan = Daemon::getChannel(maskStr);

      // Make sure we got that channel record, and this user is a member
      if (chan && chan->getMember(handler->user)) {
	 for (Channel::member_map_t::iterator it = chan->members.begin();
	      it != chan->members.end(); it++) {
	    // Send the who line
	    handler->sendWhoReply((*it).second->getUser(), chan, (*it).second);
	 }
      }
   } else {
      // Looking for a nickname or nickname mask
      if (!maskStr.length() || 
	  (maskStr == "0")) {
	 maskStr = "*";
      }
      
      StringMask mask(maskStr.IRCtoLower());
      char option = toupper(tokens->nextToken()[0]);
      
#ifdef MAX_WHO_LIST_LINES
      unsigned short lines = 0;
#endif
      
      for (Daemon::user_map_t::iterator it = Daemon::users.begin();
	   it != Daemon::users.end(); it++) {
	 // Check for a match (we could match a heap of things here)
	 if (mask.matches((*it).first) ||
	     mask.matches((*it).second->vwhostname) ||
	     mask.matches((*it).second->getVWAddress()) ||
	     mask.matches((*it).second->username) ||
	     ((User::isOper(handler->user) || (handler->user == (*it).second)) &&
	      (mask.matches((*it).second->hostname) ||
	       mask.matches((*it).second->getAddress())))) {
	    // Check if we are looking for a more specific match
	    switch (option) {
	     case 'O': // looking for an IRC OP that matches the mask
	       if (!User::isOper((*it).second)) {
		  continue; // Next record
	       }
	       break;
	       // more options?!
	    }

#ifdef HAVE_IRC2USER_MATCH_COUNTING
	    // Increase the match counter
	    matches++;
#endif
	    
	    // Grab the channel and the member record
	    Channel *chan = 0;
	    ChannelMember *chanmem = 0;
	    
	    // If this user is 'invisible', we should not show the channel...
	    if (!((*it).second->modes & User::M_INVISIBLE)) {
	       // Check that they are actually on a channel first
	       if (!(*it).second->channels.empty()) {
		  // Grab the first one on their list (fastest to grab, really)
		  chan = (*(*it).second->channels.begin()).second;
		  
		  // Check that this channel can be made visible
		  if (!User::isOper(handler->user) &&
		      chan->isHidden()) {
		     // go to next channel here?!
		     chan = 0;
		  }
	       }
	    }

	    // If we have a channel, grab the user channel member record
	    if (chan) {
	       chanmem = chan->getMember((*it).second);
	    }
	    
	    // Send the who line
	    handler->sendWhoReply((*it).second, chan, chanmem);

#ifdef MAX_WHO_LIST_LINES
	    // Increment and check our lines limit
	    if (!User::isOper(handler->user) &&
		(++lines >= MAX_WHO_LIST_LINES)) {
	       handler->sendNumeric(Numerics::ERR_WHOTRUNC,
				    String(':') + 
				    handler->lang(Language::L_ERR_WHOTRUNC));
	       break;
	    }
#endif
	 }
      }
   }
   
   // Send the end of help numeric
#ifdef HAVE_IRC2USER_MATCH_COUNTING
   handler->
     sendNumeric(Numerics::RPL_ENDOFWHO,
		 ((matches > 0) ?
		  ((matches == 1) ?
		   String::printf("%s :%s",
				  (char const *)maskStr,
				  (char const *)handler->lang(Language::L_RPL_ENDOFWHO)) :
		   String::printf("%s :%s (%u %s)",
				  (char const *)maskStr,
				  (char const *)handler->lang(Language::L_RPL_ENDOFWHO),
				  matches,
				  (char const *)handler->lang(Language::W_MATCH_PL))) :
		  String::printf("%s :%s (%s)",
				 (char const *)maskStr,
				 (char const *)handler->lang(Language::L_RPL_ENDOFWHO),
				 (char const *)handler->lang(Language::P_NO_MATCH))));
				 
#else
   handler->
     sendNumeric(Numerics::RPL_ENDOFWHO,
		 String::printf("%s :%s",
				(char const *)maskStr,
				(char *)handler->lang(Language::L_RPL_ENDOFWHO)));
#endif
}


/* parseWHOIS
 * Original 23/08/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::parseWHOIS(irc2userHandler *handler, StringTokens *tokens)
{
   String param1 = tokens->nextToken();
   String param2 = tokens->nextToken();
   
   // Server not specified, or it is us?
   if (!param2.length()) {
      doWHOIS(handler, handler->user, param1);
   } else {
      Server *s = Daemon::getServer(StringMask(param1));
      
      // Check
      if (!s) {
	 // Ok.. well try and look up a user to grab a server..
	 User *u = Daemon::getUser(param1);
	 
	 // Check
	 if (!u) {
	    handler->sendNumeric(Numerics::ERR_NOSUCHSERVER,
				 param1 + " :" +
				 handler->lang(Language::L_ERR_NOSUCHSERVER));
	    return;
	 }
	 
	 // Copy the server pointer across
	 s = u->getServer();
      }
   
      // Is this US??
      if (s == Daemon::myServer()) {
	 doWHOIS(handler, handler->user, param2);
	 return;
      }
   
      // Poll the remote server
      Daemon::routeTo(s)->callWHOIS(s, handler->user, param2);
   }
}


/* parseWHOWAS
 * Original 09/10/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::parseWHOWAS(irc2userHandler *handler, StringTokens *tokens)
{
#ifdef HAVE_IRC2USER_MATCH_COUNTING
   unsigned int matches = 0;
#endif
   
   // Check we have at least one parameter
   if (tokens->countTokens() < 2) {
      handler->sendNumeric(Numerics::ERR_NEEDMOREPARAMS,
			   String("WHOWAS :") + 
			   handler->lang(Language::L_ERR_NEEDMOREPARAMS));
      return;
   }
   
   // Grab the variables we need
   String param = tokens->nextToken();
   StringTokens nicks(param);
   String countStr = tokens->nextToken();
   int count = -1;
   
   // Check if we ARE counting or not
   if (countStr.length()) {
      // Try to convert the string to an integer
      count = countStr.toInt();
      
      // Check it is valid
      if (count <= 0) {
	 // Reset it
	 count = -1;
      }
   }
   
   // Run through the given nicks that are requested to be matched up
   for (String nick = nicks.nextToken(','); nick.length();
	nick = nicks.nextToken(',')) {
      // Fix the nick..
      nick = nick.IRCtoLower();
      
      // Run through the whowas list
      for (Daemon::whowas_deque_t::iterator it = Daemon::whowas.begin();
	   it != Daemon::whowas.end(); it++) {
	 // Check for a match
	 if ((*it).getNickname().IRCtoLower() == nick) {
	    // Check the counter
	    if (count-- == 0) {
	       break;
	    }
      
#ifdef HAVE_IRC2USER_MATCH_COUNTING
	    // Increase the match counter
	    matches++;
#endif
   
	    // Send the entry for this iteration
	    handler->sendNumeric(Numerics::RPL_WHOWASUSER,
				 String::printf("%s %s %s * :%s",
						(char const *)(*it).getNickname(),
						(char const *)(*it).getUsername(),
						(char const *)(*it).getHostname(handler->user),
						(char const *)(*it).getRealname()));
	    handler->sendNumeric(Numerics::RPL_WHOISSERVER,
				 String::printf("%s %s :%lu",
						(char const *)(*it).getNickname(),
						(char const *)(*it).getServer(handler->user),
						(*it).getSignoffTime()));
	    // If this user was away, send their old away message too (sigh)
	    if ((*it).hasAwayMessage()) {
	       handler->
		 sendNumeric(Numerics::RPL_AWAY,
			     String::printf(":%s",
					    (char const *)(*it).getAwayMessage()));
	    }
	 }
      }
   }
   
   // Send end of whowas list
#ifdef HAVE_IRC2USER_MATCH_COUNTING
   handler->
     sendNumeric(Numerics::RPL_ENDOFWHOWAS,
		 ((matches > 0) ?
		  ((matches == 1) ?
		   String::printf("%s :%s",
				  (char *)handler->lang(Language::L_RPL_ENDOFWHOWAS),
				  (char const *)param) :
		   String::printf("%s :%s (%u %s)",
				  (char const *)param,
				  (char const *)handler->lang(Language::L_RPL_ENDOFWHOWAS),
				  matches,
				  (char const *)handler->lang(Language::W_MATCH_PL))) :
		  String::printf("%s :%s (%s)",
				 (char const *)param,
				 (char const *)handler->lang(Language::L_RPL_ENDOFWHOWAS),
				 (char const *)handler->lang(Language::P_NO_MATCH))));
#else
   handler->
     sendNumeric(Numerics::RPL_ENDOFWHOWAS,
		 String::printf("%s :%s",
				(char const *)param,
				handler->lang(Language::L_RPL_ENDOFWHOWAS)));
#endif
}
