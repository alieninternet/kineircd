/* irc2user.cpp
 * Handle USER connection data using the irc2 server<->user protocol
 */

#include "config.h"

#include <ctype.h>

#include "handler.h"
#include "numerics.h"
#include "language.h"
#include "version.h"
#include "modes.h"
#include "utils.h"
#include "user.h"
#include "channel.h"


/* Functions table. In the interests of efficiency should this table be 
 * broken down since a compiler would not be able to efficiently create a
 * structure multiplier for the CPU?
 */
irc2userHandler::functionTableStruct irc2userHandler::functionsTable[] = {
     { "ACCEPT",	parseACCEPT,		1,	0,
	  "{ <nickname> | -<nickname> | * } ( SPACE { nickname | -nickname "
	  "| * } )",
	  "This command is only useful with the user mode '+g' set. When set, "
	  "this command allows you to accept people, thus enabling them to "
	  "talk to you while you are ignoring everyone with user-mode '+g'."
     },
     { "ADMIN",		parseADMIN,		2,	0,
	  "[ <server> ]",
	  "Returns information about the administrators of the given server. "
	  "If no server is specified, this returns information regarding "
	  "the administrators of the server you are locally connected to."
     }, 
     { "AWAY",		parseAWAY,		1,	0,
	  "[ <message> ]",
	  "If a message is given, this will mark you as 'away', using the "
	  "given message as your reason. If you do not give a message, this "
	  "will mark you as 'unaway', or that you have returned, and will "
	  "remove the given away message."
     }, 
     { "CONNECT",	parseCONNECT,		0,	User::isOper,
	  "<target server>  [ <port>  [ <remote server> ] ]",
	  "Instruct a server to connect to the target server on the default "
	  "port found in the configuration file for that server. If a port is "
	  "given, the server will connect on that port instead. If a remote "
	  "server is given, that server will be instructed to connect to "
	  "the target server rather than the local server forfilling the "
	  "request."
     }, 
     { "DIE",		parseDIE,		0,	User::isOper,
	  "[ <reason> ]",
	  "This command shuts down the server you are currently connected to. "
	  "Optionally a reason can be specified which will be broadcast to "
	  "other operators on the network as the reason for the server's "
	  "disappearance."
     },
     { "GLOBOPS",	parseGLOBOPS,		0,	User::isGlobalOper,
	  "<message>",
	  "Sends the given message to all global operators online (provided "
	  "they have their server-notices usermode set to receive them)."
     },
     { "HELP",		parseHELP,		3,	0,
	  "[ '-' ] [ <command> | <command mask> ]",
	  "Returns command parameters for the given command or mask. If no "
	  "mask it specified, it is assuming you mean all commands (or a mask "
	  "of '*'). When the '-' prefix is added to the command or mask, "
	  "additional help will be returned, such as what you are reading now."
     },
     { "HELPME",	parseHELPME,		4,	User::isHelper,
	  "<message>",
	  "Sends the given message to operators on-line for requesting help "
	  "with something. This is usually used to ask for an operator's "
	  "assistance with something a 'helper' cannot acheive on their own, "
	  "such as a KILL or GLINE."
     }, 
     { "INFO",		parseINFO,		1,	0,
	  "[ <server> ]",
	  "Returns extended information about the version of the given "
	  "server. If no server is specified, this returns information "
	  "regarding the server you are locally connected to."
     }, 
     { "INVITE",	parseINVITE,		3,	0,
	  "<nickname>  <channel>",
	  "Invite the person under the given nickname to the specified "
	  "channel. For this to work, you must currently be on the given "
	  "channel and be of either 'Operator' or 'Half-Operator' status."
     }, 
     { "ISON",		parseISON,		2,	0,
	  "<nickname> ( SPACE <nickname> )",
	  "Check if the given nickname(s) are online. The server will "
	  "compile a list of nicknames who are online now in the exact "
	  "order of the list given."
     },
     { "JOIN",		parseJOIN,		3,	0,
	  "{ <channel> ( ',' <channel> )  [ <key> ( ',' <key> ) ] } | '0'",
	  "Join the given channels. If the channel(s) require a key, or "
	  "keys, then the keys list (if it exists) will be checked for a "
	  "valid key. If the special channel name '0' is used, it is "
	  "considered a 'partall' command, and will part you from any "
	  "channel you are currently on. Any channels specified before the "
	  "'0' on the list will not be joined."
     }, 
     { "KICK",		parseKICK,		1,	0,
	  "<channel> ( ',' <channel> )  <user> ( ',' <user> )  [ <reason> ]",
	  "Kick the given user(s) from the given channel(s), optionally "
	  "with a reason for your actions."
     },
     { "KILL",		parseKILL,		0,	User::isGlobalOper,
	  "<nickname>  <reason>",
	  "Kill the given nickname with the specified reason. The reason is "
	  "manditory as you must have a reason for KILLing someone, "
	  "naturally :)"
     },
     { "LINKS",		parseLINKS,		2,	0,
	  "[ [ <remote server> ]  <server mask> ]",
	  "Generate a list of server links. If a server mask is specified, "
	  "only servers matching the mask will be shown. If a remote server "
	  "is given, the request will be generated from the given server "
	  "instead of this server. Only helpers and operators will see "
	  "hidden servers."
     },
     { "LIST",		parseLIST,		5,	0,
	  "[ <channel> ( ',' <channel> ) ]",
	  "Return a list of channels currently created on the network. "
	  "The information returned also includes the number of users on "
	  "the channels, and the current topic."
     },
     { "LOCOPS",	parseLOCOPS,		0,	User::isOper,
	  "<message>",
	  "Sends a message to any operator connected to the same server as "
	  "you are. This is not to be confused with 'local operators', as "
	  "this command will send the given message to both local AND global "
	  "operators, limiting its reception to this server. Like GLOBOPS, "
	  "the message will only be received by an operator who has their "
	  "server-notices usermode set appropriately."
     },
     { "LUSERS",	parseLUSERS,		2,	0,
	  "[ <mask>  [ <server> ] ]",
	  "Show the current local-user status. This command has been "
	  "extended to also include global users, and peak user counts."
     },
     { "MODE",		parseMODE,		2,	0,
	  "{ <channel> | <nickname> | <server> }  ( [ '-' | '+' ] <mode> )  "
	  "[ <modeparams> ]",
	  0
     },
     { "MOTD",		parseMOTD,		3,	0,
	  "[ <server> ]",
	  "Display the MOTD (Message Of The Day). If a server is specified, "
	  "the MOTD displayed will come from that server, provided the given "
	  "server will allow you to receive it remotely."
     }, 
     { "NAMES",		parseNAMES,		1,	0,
	  "[ <channel> ( ',' <channel> ) ]",
	  0
     },
     { "NICK",		parseNICK,		3,	0,
	  "<new nickname>",
	  "Change your current nickname to the given nickname. This command "
	  "will fail if the given nickname is already in use, the nickname is "
	  "reserved for another purpose by the IRC Operators, you are in a "
	  "channel which is holding an event (channel mode +E), or your "
	  "connection is restricted."
     },
     { "NOTICE",	parseNOTICE,		4,	0,
	  "{ <nickname> | <channel> } ( ',' { <nickname> | <channel> } )  "
	  "<message>",
	  "Send the given NOTICE message to the given channel(s)/"
	  "nickname(s). Any combination of nicknames/channels can be "
	  "specified. Note that the NOTICE interface does not return any "
	  "responce, including erroneous target replies. This is done "
	  "intentionally to avoid never-ending automated replies bouncing "
	  "back and forth."
     },
     { "OPER",		parseOPER,		5,	0,
	  "<nickname>  <password>",
	  "This command allows you to become an IRC operator. You must give "
	  "your operator nickname and password for this command to be "
	  "successful."
     },
     { "PART",		parsePART,		3,	0,
	  "<channel> ( ',' <channel> )  [ <reason> ]",
	  "Leave the given channel(s), optionally giving a reason for your "
	  "departure."
     },
     { "PING",		parsePING,		1,	0,
	  "[ <text> ]",
	  "Ping the server, optionally giving text. The server will reply "
	  "with the given text, or just a PONG. Some clients use this "
	  "for 'lag checking', to measure the lag between the client and "
	  "the server."
     },
     { "PONG",		parsePONG,		1,	0,
	  "[ <text> ]",
	  "Used as a reply to a server ping, has no other purpose but for a "
	  "client to automatically respond to prove to the server the client "
	  "is still active."
     },
     { "PRIVMSG",	parsePRIVMSG,		4,	0,
	  "{ <nickname> | <channel> } ( ',' { <nickname> | <channel> } )  "
	  "<message>",
	  "Send the given PRIVMSG message to the given channel(s)/"
	  "nickname(s). Any combination of nicknames/channels can be "
	  "specified. Note that if you are being silenced by one of the "
	  "given nicks, the message may simply be ignored without warning. "
     },
     { "QUIT",		parseQUIT,		0,	0,
	  "[ <reason> ]",
	  "Leave the IRC network, optionally giving a reason for "
	  "disconnecting."
     },
     { "REHASH",	parseREHASH,		0,	User::isOper,
	  "",
	  "'Rehash' the server. This command (although incorrectly named) "
	  "will reload and parse the configuration file, as well as perform "
	  "some quick checks similar to start-up operation."
     },
     { "RESTART",	parseRESTART,		0,	User::isOper,
	  "",
	  "This will restart the server. This command will load an entirely "
	  "new process for the IRC Daemon."
     },
     { "SERVLIST",	parseSERVLIST,		2,	0,
	  "[ <mask>  [ <type> ] ]",
	  0
     },
     { "SILENCE",	parseSILENCE,		1,	0,
	  "{ { '+' | '-' } { <mask> | <nickname } } | [ <nickname > ]",
	  "Silence the given mask or nickname. This will stop any privage "
	  "messages or notices being sent to you from anyone matching the "
	  "mask or nickname (excluding services, servers, operators and "
	  "helpers). The mask or nickname must be specified with a '+' or '-' "
	  "prefix to denote adding the silence or removing the silence "
	  "respectively. If only a nickname is given, and you are a helper "
	  "or an operator, you will see that person's current silence list."
     }, // This help message may be too big?
     { "SQUIT",		parseSQUIT,		0,	User::isOper,
	  "<server>  <reason>",
	  "Force the given server to depart the network with the given "
	  "reason. Warning: this command may cause a netsplit."
     },
     { "STATS",		parseSTATS,		0,	User::isOper,
	  "[ <query>  [ <server> ] ]",
	  "Ask for a status/statistical report from the current server. If "
	  "another server is given, the report will be generated there. If "
	  "no query is given, a list of possible queries you can specify."
     },
     { "TIME",		parseTIME,		1,	0,
	  "[ <nickname> | <service> | <server> ]",
	  "Returns the current time on the given server, or the server the "
	  "given user or service is connected to. This is useful when talking "
	  "to people around the world, provided they are connected to a "
	  "server that is geographically local to where they are situated."
     },
     { "TOPIC",		parseTOPIC,		3,	0,
	  "<channel>  [ <topic> ]",
	  "If no topic is specified, this returns the current topic on the "
	  "given channel. If a topic is specified, this command will change "
	  "the topic to the one given, provided you have access to do so."
     },
     { "TRACE",		parseTRACE,		0,	User::isOper,
	  "[ <nickname> | <service> | <server> ]",
	  "Trace the path (internally on the network) if a given nickname, "
	  "service or server. This is useful for locating lagging "
	  "connections that need to be re-routed."
     },
     { "TRACEROUTE",	parseTRACEROUTE,	0,	User::isOper,
	  "<destination>  [ <server> ]",
	  "Trace the path of IP packets (externally to the network) of the "
	  "given server. This is useful for finding better routes and should "
	  "be used to confirm a better solution for a route."
     }, // this command is planned...
     { "USERHOST",	parseUSERHOST,		2,	0,
	  "<nickname> ( SPACE <nickname> )",
	  "Returns the hosts of the nicknames on the given list. Note that "
	  "the host may in fact be a virtual-world host rather than the "
	  "real host. Also see USERIP."
     },
     { "USERIP",	parseUSERIP,		2,	0,
	  "<nickname> ( SPACE <nickname> )",
	  "Returns the IP addresses of the nicknames on the given list. Note "
	  "that the IP may in fact be a virtual-world IP rather than the "
	  "real IP. Also see USERHOST."
     },
     { "VERSION",	parseVERSION,		1,	0,
	  "[ <nickname> | <service> | <server> ]",
	  "Returns the version of the given server, or the server the given "
	  "user or service is connected to."
     },
     { "WATCH",		parseWATCH,		3,	0,
	  "'C' | 'S' | 'L' | { '+' | '-' } { <nickname> | <channel> | <server> } ( SPACE { '+' | '-' } { <nickname> | <channel> | "
	  "<server> } )",
	  "Used as an alternative to ISON, WATCH sets up a list of "
	  "nicknames, channels or servers you can 'watch'. You will be "
	  "notified of their appearance and disappearances on the network. "
	  "Note that only helpers and operators can watch a server."
     },
     { "WALLCHOPS",	parseWALLCHOPS,		4,	0,
	  "*tba*",
	  0
     },
#ifdef ALLOW_OPER_WALLOPS
     { "WALLOPS",	parseWALLOPS,		0,	User::isOper,
	  "<message>",
	  "Broadcasts the given message to all users on the network with the "
	  "user mode '+w' set."
     },
#endif
     { "WHO",		parseWHO,		2,	0,
	  "[ <channel> | <nickname mask> | '0' ]  "
	  "[ 'H' / 'I' / 'N' / 'O' / 'R' / 'S' / 'U' / 'X' ] "
	  "[ '%' { 'C' / 'D' / 'F' / 'H' / 'I' / 'N' / 'R' / 'S' / 'T' / "
	  "'U' } [ ',' <query type> ] ]",
	  0
     },
     { "WHOIS",		parseWHOIS,		2,	0,
	  "[ <server> ]  <nickname> ( ',' <nickname> )",
	  "Return information on who the given nickname(s) is/are. If a "
	  "server is specified, that server will generate the reply. This is "
	  "an extended version of the simplistic WHO command."
     },
     { "WHOWAS",	parseWHOWAS,		2,	0,
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
  user(u)
{
#ifdef DEBUG
   debug("New Handler: irc2userHandler");
#endif

   // Create a LocalUser class for this user since they are obviouslly local
   user->local = new LocalUser(user, this);
   
   // Add this user to the connection list
   getConnection()->getDaemon()->addUser(user);
	    
   // Increase the client connection count, and the peak too if we need to
   getConnection()->getDaemon()->numClientConns++;
   if (getConnection()->getDaemon()->numClientConns > 
       getConnection()->getDaemon()->numClientConnsPeak) {
      getConnection()->getDaemon()->numClientConnsPeak = 
	getConnection()->getDaemon()->numClientConns;
   }

   // Send the greeting lines
   sendNumeric(RPL_WELCOME,
	       String::printf(LNG_RPL_WELCOME,
			      (char const *)user->getAddress()));
   sendNumeric(RPL_YOURHOST,
	       String::printf(LNG_RPL_YOURHOST,
			      (char const *)getConnection()->getDaemon()->myServer()->hostname,
			      (char const *)getVersion));
   
#ifdef BLOODY_IRCII_KLUGE
   /* I hate this. It makes the signon look ugly, and us like a pack of idiots.
    * This is essentially THE SAME as RPL_YOURHOST, except the ircII client
    * desparately needs it.. or so they say - First person to confirm that to
    * be false gets the honours of removing this crap :)
    */
   getConnection()->sendRaw(String::printf(LNG_RPL_YOURHOST_IRCII_KLUGE_NOTICE
					   IRC2USER_EOL_CHARS,
					   (char const *)user->nickname,
					   (char const *)getConnection()->getDaemon()->myServer()->hostname,
					   (char const *)getVersion));
#endif
   
   sendNumeric(RPL_CREATED,
	       String::printf(LNG_RPL_CREATED,
			      getBuild));
   sendNumeric(RPL_MYINFO,
	       String::printf("%s %s %s %s %s %s %s %s %s",
			      (char const *)getConnection()->getDaemon()->myServer()->hostname,
			      getVersion,
			      userModeStr,
			      channelModeStr,
			      channelModeParamStr,
			      userModeParamStr,
			      serverModeStr,
			      serverModeParamStr,
			      getVersionChars));
   sendNumeric(RPL_ISUPPORT,
	       String::printf("NICKLEN=%d "		// (~11chrs)
			      "TOPICLEN=%d "		// (~13chrs)
			      "KICKLEN=%d "		// (~12chrs)
			      "%s"			// NETWORK= (~30chrs)
//			      "WHOX "			// (5chrs)
//			      "WALLCHOPS "		// (10chrs)
//			      "USERIP "			// (7chrs)
			      "WATCH=%d "		// (~10chrs)
			      "SILENCE=%d "		// (~11chrs)
			      "CHANTYPES=%s "		// (~15chrs)
			      "MAXCHANNELS=%d " 	// (~16chrs)
			      "MAXBANS=%d "     	// (~11chrs)
			      "CHARSET=rfc1459 " 	// (16chrs)
			      "MODES=%d "		// (~9chrs)
			      "PREFIX=%s "		// (~20chrs)
			      LNG_RPL_ISUPPORT_TAG, 	// (=~196chrs + this)
			      MAXLEN_NICKNAME,
			      MAXLEN_TOPIC,
			      MAXLEN_KICK_REASON,
			      (getConnection()->getDaemon()->haveNetworkName() ?
			       ((char const *)
				(String("NETWORK=") +
				 getConnection()->getDaemon()->getNetworkName() +
				 " ")) :
			       ""),
			      MAX_WATCHES_PER_USER,
			      MAX_SILENCES_PER_USER,
			      CHANNEL_TYPES,
			      MAX_CHANNELS_PER_USER, 
			      MAX_BANS_PER_CHANNEL,
			      MAX_MODES_PER_COMMAND,
			      channelPrefixes));
   
   // Assemble and send the current time on the server to the client
   TYPE_RPL_TIMEONSERVERIS_FLAGS tosiFlags = 0;
   bool GMTdiffEast = true; // We working east or west of GMT?
   unsigned short GMTdiffHours = 0;
   unsigned short GMTdiffMins = 0;
   
#ifdef HAVE_STRUCT_TM_TM_ZONE
   time_t timeNow = getConnection()->getDaemon()->getTime();
   struct tm *timeinfo;
   timeinfo = localtime(&timeNow);
   
   // Daylight savings mode?
   if(timeinfo->tm_isdst) {
      tosiFlags |= TIMEONSERVERFLAG_DST;
   }
   
   // Work out the GMT offset
   GMTdiffEast = (timeinfo->tm_gmtoff > 0);
   GMTdiffHours = (unsigned short)(timeinfo->tm_gmtoff / 3600);
   GMTdiffMins = (unsigned short)((timeinfo->tm_gmtoff % 3600) / 60);
#else
   // Are we *POSSIBLY* in DST mode? This posix variable is half-accurate..
   if (daylight) {
      tosiFlags |= TIMEONSERVERFLAG_DST;
   }
   
   // Work out the timezone (this does not seem to work too well)
   GMTdiffEast = (timezone > 0);
   GMTdiffHours = (unsigned short)(timezone / 3600);
   GMTdiffMins = (unsigned short)((timezone % 3600) / 60);
#endif
   
   sendNumeric(RPL_TIMEONSERVERIS,
	       String::printf(LNG_RPL_TIMEONSERVERIS,
			      getConnection()->getDaemon()->getTime(),
			      getConnection()->getDaemon()->getTimeUsecs(),
			      ((GMTdiffEast) ? '+' : '-'),
			      GMTdiffHours,
			      GMTdiffMins,
			      tosiFlags));

   // Send some stuff that clients seem to rely on being sent... (urgh)
   doLUSERS(this, user, 0);
   
#ifdef SHORT_MOTD
   // Send a fake three-line motd
   sendNumeric(RPL_MOTDSTART,
	       String::printf(LNG_RPL_MOTDSTART, 
			      (char const *)getConnection()->getDaemon()->myServer()->hostname));
   sendNumeric(RPL_MOTD, LNG_RPL_MOTD_SHORT1);
   sendNumeric(RPL_MOTD, LNG_RPL_MOTD_SHORT2);
   sendNumeric(RPL_MOTD, LNG_RPL_MOTD_SHORT3);
   sendNumeric(RPL_ENDOFMOTD, LNG_RPL_ENDOFMOTD);
#else
   doMOTD(this, user);
#endif
   
   // Send the 6-line spam notice thingy
   sendNumeric(RPL_SPAM, LNG_RPL_SPAM_LINE1);
   sendNumeric(RPL_SPAM, LNG_RPL_SPAM_LINE2);
   sendNumeric(RPL_SPAM, LNG_RPL_SPAM_LINE3);
   sendNumeric(RPL_SPAM, LNG_RPL_SPAM_LINE4);
   sendNumeric(RPL_SPAM, LNG_RPL_SPAM_LINE5);
   sendNumeric(RPL_SPAM, LNG_RPL_SPAM_LINE6);
   
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
      processUserModes(&modes, &dummy, true);
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
	 if (modeBitMask & USERMODE_WALLOPS) {
	    user->modes |= USERMODE_WALLOPS;
	 }
	 if (modeBitMask & USERMODE_INVISIBLE) {
	    user->modes |= USERMODE_INVISIBLE;
	 }
      }
   }
   
   // Send the modes if there are any set. Note we ignore any parameters...
   if (user->modes != 0) {
      sendUserMode(0, makeUserModes(user->modes));
   }
}


/* ~irc2userHandler - Class destructor
 * Original 13/08/01, Simon Butcher <pickle@austnet.org>
 */
irc2userHandler::~irc2userHandler(void)
{
   // Lower the connection count
   getConnection()->getDaemon()->numClientConns--;
}


/* goodbye - Handle a connection closure
 * Original 02/09/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::goodbye(String const &reason)
{
   // Tell the user why the link is being closed if we need to
   sendGoodbye(reason);

   // Deregister us from the network
   getConnection()->getDaemon()->quitUser(user, reason, true);
}


/* kill - Handle a connection closure (killed)
 * Original 22/09/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::kill(String *caller, String *reason)
{
   // Tell the user why the link is being closed
   sendKill(user, caller, reason);
   
   // Shut down the connection
   getConnection()->kill();
}


/* processUserModes - Process a user mode string
 * Original 24/08/01, Simon Butcher <pickle@austnet.org>
 */
String irc2userHandler::processUserModes(String *modes, StringTokens *tokens, 
					 bool silent /* = false */)
{
   bool toggle = true;
   int numModes = 0;
   String param = "";
   String toggleOnStr = "";
   String toggleOffStr = "";
   String toggleParamsOn = "";
   String toggleParamsOff = "";

   for (String::length_t i = 0; 
	((i < modes->length()) && (numModes < MAX_MODES_PER_COMMAND));
	i++) {
      switch ((*modes)[i]) {
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
	 for (int ii = 0; userModeTable[ii].letter != 0; ii++) {
	    if (userModeTable[ii].letter == (*modes)[i]) {
	       // Check if this mode can be toggled by the user
	       if (userModeTable[ii].userToggle) {
		  // If this mode needs a parameter, grab the next token...
		  if ((toggle && userModeTable[ii].hasParamOn) ||
		      (!toggle && userModeTable[ii].hasParamOff)) {
		     param = tokens->nextToken();
		  } else {
		     param = "";
		  }
		  
		  // If this runs, we will need to add the output char
		  if (userModeTable[ii].toggler(toggle, 
						(silent ? 0 : this),
						getConnection()->getDaemon(), user,
						&(*modes)[i], &param) &&
		      !silent) {

		     // Check which toggle string to add this to
		     if (toggle) {
			toggleOnStr = toggleOnStr + String((*modes)[i]);
			
			// If this mode had a paramter, add it to the list
			if (param.length()) {
			   toggleParamsOn = toggleParamsOn + " " + param;
			}
		     } else {
			toggleOffStr = toggleOffStr + String((*modes)[i]);

			// If this mode had a paramter, add it to the list
			if (param.length()) {
			   toggleParamsOff = toggleParamsOff + " " + param;
			}
		     }
		  }
	       } else {
		  // Tell the user they cannot change that mode, if we can
		  if (!silent) {
		     sendNumeric(ERR_CANNOTCHANGEUMODE,
				 String::printf(LNG_ERR_CANNOTCHANGEUMODE,
						(*modes)[i]));
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
	    sendNumeric(ERR_UMODEUNKNOWNFLAG,
			String((*modes)[i]) + LNG_ERR_UMODEUNKNOWNFLAG);
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
void irc2userHandler::sendChannelMode(Channel *c, User *u, String *modes)
{
   getConnection()->sendRaw(String::printf(":%s MODE %s %s" IRC2USER_EOL_CHARS,
				      (char const *)u->getAddress(user),
				      (char const *)c->name,
				      (char const *)*modes));
}

void irc2userHandler::sendChannelMode(Channel *c, User *u, String modes)
{
   getConnection()->sendRaw(String::printf(":%s MODE %s %s" IRC2USER_EOL_CHARS,
				      (char const *)u->getAddress(user),
				      (char const *)c->name,
				      (char const *)modes));
}


/* sendGoodbye - Send an ERROR message to tell the client we are saying bye
 * Original 14/08/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::sendGoodbye(String const &reason)
{
   getConnection()->sendRaw(String::printf(LNG_ERROR_CLOSING_LINK 
				      IRC2USER_EOL_CHARS,
				      (reason.length() ?
				       (char const *)reason :
				       LNG_ERROR_CLOSING_LINK_DEFAULT_REASON)));
}


/* sendJoin - Send a channel join message
 * Original 14/08/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::sendJoin(Channel *c, User *u)
{
   getConnection()->sendRaw(String::printf(":%s JOIN %s" IRC2USER_EOL_CHARS,
				      (char const *)u->getAddress(user),
				      (char const *)c->name));
}


/* sendKick - Send a channel kick message
 * Original 20/09/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::sendKick(Channel *c, User *kicker, User *kickee,
			       String *reason)
{
   getConnection()->sendRaw(String::printf(":%s KICK %s %s%s" IRC2USER_EOL_CHARS,
				      (char const *)kicker->getAddress(user),
				      (char const *)c->name,
				      (char const *)kickee->nickname,
				      (((reason && reason->length()) ?
					(char const *)(String(" :") + 
						       *reason) :
					""))));
}


/* sendKill - Send a user kill message
 * Original 22/09/01, Simon Butcher <pickle@austnet.org>
 * Note: The user field is ignored since any kill MUST be to us anyway
 */
void irc2userHandler::sendKill(User *u, String *caller, String *reason)
{
   getConnection()->sendRaw(String::printf(":%s KILL %s :%s (%s)" 
				      IRC2USER_EOL_CHARS,
				      (char const *)(*caller),
				      (char const *)user->nickname,
				      (char const *)(*caller),
				      (char const *)(*reason)));
}


/* sendNick - Send a nickname change event
 * Original 24/08/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::sendNick(User *u, String *nick)
{
   getConnection()->sendRaw(String::printf(":%s NICK :%s" IRC2USER_EOL_CHARS,
				      (char const *)u->getAddress(user),
				      (char const *)(*nick)));
}


/* sendNotice - [Various Forms] Send a NOTICE message
 * Original 01/08/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::sendNotice(String *from, String *destination, 
				 String const &message)
{
   getConnection()->sendRaw(String::printf(":%s NOTICE %s :%s" IRC2USER_EOL_CHARS,
				      (char const *)*from,
				      (char const *)*destination,
				      (char const *)message));
}

void irc2userHandler::sendNotice(String from, String *destination,
				 String const &message)
{
   getConnection()->sendRaw(String::printf(":%s NOTICE %s :%s" IRC2USER_EOL_CHARS,
				      (char const *)from,
				      (char const *)*destination,
				      (char const *)message));
}

void irc2userHandler::sendNotice(User *from, String *destination, 
				 String const &message)
{
   getConnection()->sendRaw(String::printf(":%s NOTICE %s :%s" IRC2USER_EOL_CHARS,
				      (char const *)from->getAddress(user),
				      (char const *)*destination,
				      (char const *)message));
}


/* sendNumeric - [Various forms] Send a numeric to the connection
 * Original 12/08/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::sendNumeric(short numeric, String line)
{
   getConnection()->sendRaw(String::printf(":%s %03d %s %s" IRC2USER_EOL_CHARS,
				      (char const *)getConnection()->getDaemon()->myServer()->hostname,
				      numeric,
				      (char const *)user->nickname,
				      (char const *)line));
}

void irc2userHandler::sendNumeric(Server *from, short numeric, User *to, 
				  String line)
{
   getConnection()->sendRaw(String::printf(":%s %03d %s %s" IRC2USER_EOL_CHARS,
				      (char const *)from->hostname,
				      numeric,
				      (char const *)user->nickname,
				      (char const *)line));
}


/* sendPart - Send a channel part message
 * Original 14/08/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::sendPart(Channel *c, User *u, String const &reason)
{
   getConnection()->sendRaw(String::printf(":%s PART %s%s" IRC2USER_EOL_CHARS,
				      (char const *)u->getAddress(user),
				      (char const *)c->name,
				      (char const *)(reason.length() ?
						     (String(" ") + reason) :
						     String(""))));
}


/* sendPing - Send a PING request to this connection
 * Original 24/08/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::sendPing(void)
{
   getConnection()->sendRaw(String::printf("PING :%s" IRC2USER_EOL_CHARS,
				      (char const *)getConnection()->getDaemon()->myServer()->hostname));
}


/* sendPong - Send a PING reply (PONG)
 * Original 20/08/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::sendPong(String reply)
{
   getConnection()->sendRaw(String::printf(":%s PONG %s %s" IRC2USER_EOL_CHARS,
				      (char const *)getConnection()->getDaemon()->myServer()->hostname,
				      (char const *)getConnection()->getDaemon()->myServer()->hostname,
				      (char const *)reply));
}


/* sendPrivmsg - [Various Forms] Send a PRIVMSG message
 * Original 01/08/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::sendPrivmsg(String *from, String *destination, 
				  String const &message)
{
   getConnection()->sendRaw(String::printf(":%s PRIVMSG %s :%s" IRC2USER_EOL_CHARS,
				      (char const *)*from,
				      (char const *)*destination,
				      (char const *)message));
}

void irc2userHandler::sendPrivmsg(String from, String *destination, 
				  String const &message)
{
   getConnection()->sendRaw(String::printf(":%s PRIVMSG %s :%s" IRC2USER_EOL_CHARS,
				      (char const *)from,
				      (char const *)*destination,
				      (char const *)message));
}

void irc2userHandler::sendPrivmsg(User *from, String *destination,
				  String const &message)
{
   getConnection()->sendRaw(String::printf(":%s PRIVMSG %s :%s" IRC2USER_EOL_CHARS,
				      (char const *)from->getAddress(user),
				      (char const *)*destination,
				      (char const *)message));
}


/* sendQuit - Send a network quit message
 * Original 22/08/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::sendQuit(User *u, String const &reason)
{
   getConnection()->sendRaw(String::printf(":%s QUIT :%s" IRC2USER_EOL_CHARS,
				      (char const *)u->getAddress(user),
				      (reason.length() ?
				       (char const *)reason :
				       LNG_DEFAULT_QUIT_MESSAGE)));
}


/* sendServerMode - [Various forms] Send a server mode change
 * Original 21/09/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::sendServerMode(Server *s, String *setter, String *modes)
{
   getConnection()->sendRaw(String::printf(":%s MODE %s %s" IRC2USER_EOL_CHARS,
				      (char const *)(*setter),
				      (char const *)s->hostname,
				      (char const *)(*modes)));
}

void irc2userHandler::sendServerMode(Server *s, User *setter, String *modes)
{
   getConnection()->sendRaw(String::printf(":%s MODE %s %s" IRC2USER_EOL_CHARS,
				      (char const *)setter->getAddress(user),
				      (char const *)s->hostname,
				      (char const *)(*modes)));
}


/* sendSilence - Send a SILENCE reply
 * Original 23/09/01, Simon Butcher <pickle@austnet.org>
 * Note: The 'silencer' is ignored here, since this is always going to be
 *       the current user.
 */
void irc2userHandler::sendSilence(User *silencer, bool setting, 
				  StringMask *mask)
{
   getConnection()->sendRaw(String::printf(":%s SILENCE %c%s" IRC2USER_EOL_CHARS,
				      (char const *)user->getAddress(),
				      (setting ? '+' : '-'),
				      (char const *)mask->getMask()));
}


/* sendTopic - [Various Forms] Send a channel topic change
 * Original 19/09/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::sendTopic(Channel *chan, String *from, String *topic)
{
   getConnection()->sendRaw(String::printf(":%s TOPIC %s :%s" IRC2USER_EOL_CHARS,
				      (char const *)*from,
				      (char const *)chan->name,
				      (char const *)*topic));
}

void irc2userHandler::sendTopic(Channel *chan, User *from, String *topic)
{
   getConnection()->sendRaw(String::printf(":%s TOPIC %s :%s" IRC2USER_EOL_CHARS,
				      (char const *)from->getAddress(user),
				      (char const *)chan->name,
				      (char const *)*topic));
}


/* sendUserMode - Send a user mode change
 * Original 25/08/01, Simon Butcher <pickle@austnet.org>
 * Note: This is a strange one, but this is how it's supposed to work
 *       according to the implementations (not the RFC). Here the given user
 *       is ignored
 */
void irc2userHandler::sendUserMode(User *u, String modes)
{
   getConnection()->sendRaw(String::printf(":%s MODE %s :%s" IRC2USER_EOL_CHARS,
				      (char const *)user->nickname,
				      (char const *)user->nickname,
				      (char const *)modes));
}


/* sendWhoReply - Send a WHO reply
 * Original 22/08/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::sendWhoReply(User *u, Channel *c, ChannelMember *cm)
{
   sendNumeric(RPL_WHOREPLY,
	       String::printf("%s %s %s %s %s %c%s :%d %s" IRC2USER_EOL_CHARS,
			      (c ? 
			       (char const *)c->name : 
			       "*"),
			      (char const *)u->username,
			      (u->showVW(user) ?
			       (char const *)u->vwhostname :
			       (char const *)u->hostname),
			      (((u->server->modes & SERVERMODE_HIDDEN) &&
				!User::isOper(user)) ?
			       (char const *)u->server->hostname :
			       SERVER_NAME_MASK),
			      (char const *)u->nickname,
			      (u->awayMessage.length() ?
			       'G' : 'H'),
			      ((char const *)
			       (String(User::isOper(u) ?
				       "*" : "") +
				(cm ?
				 ((cm->modes & CHANMEMMODE_OPPED) ?
				  "@" :
				  ((cm->modes & CHANMEMMODE_HALFOPPED) ?
				   "%" :
				   ((cm->modes & CHANMEMMODE_VOICED) ?
				    "+" : ""))) : ""))),
			      u->server->numHops,
			      (char const *)u->realname));
}


/* sendWallops - [Various Forms] Send a WALLOPS message
 * Original 18/09/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::sendWallops(String *from, String *message)
{
   getConnection()->sendRaw(String::printf(":%s WALLOPS :%s" IRC2USER_EOL_CHARS,
				      (char const *)*from,
				      (char const *)*message));
}

void irc2userHandler::sendWallops(User *from, String *message)
{
   getConnection()->sendRaw(String::printf(":%s WALLOPS :%s" IRC2USER_EOL_CHARS,
				      (char const *)from->getAddress(user),
				      (char const *)*message));
}


/* sendWatchOff - [Various Forms] Send an object sign-off notification message
 * Original 24/09/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::sendWatchOff(Server *target)
{
   getConnection()->sendRaw(String::printf(":%s %d %s %s * * 0 "
				      LNG_RPL_LOGOFF_SERVER
				      IRC2USER_EOL_CHARS,
				      (char const *)getConnection()->getDaemon()->myServer()->hostname,
				      RPL_LOGOFF,
				      (char const *)user->nickname,
				      (char const *)target->hostname));
}

void irc2userHandler::sendWatchOff(Channel *target)
{
   getConnection()->sendRaw(String::printf(":%s %d %s %s * * %lu "
				      LNG_RPL_LOGOFF_CHANNEL
				      IRC2USER_EOL_CHARS,
				      (char const *)getConnection()->getDaemon()->myServer()->hostname,
				      RPL_LOGOFF,
				      (char const *)user->nickname,
				      (char const *)target->name,
				      target->creationTime));
}

void irc2userHandler::sendWatchOff(User *target)
{
   getConnection()->sendRaw(String::printf(":%s %d %s %s %s %s %lu "
				      LNG_RPL_LOGOFF_USER
				      IRC2USER_EOL_CHARS,
				      (char const *)getConnection()->getDaemon()->myServer()->hostname,
				      RPL_LOGOFF,
				      (char const *)user->nickname,
				      (char const *)target->nickname,
				      (char const *)target->username,
				      (char const *)target->getAddress(user),
				      target->lastNickChange));
}


/* sendWatchOn - [Various Forms] Send an object sign-on notification message
 * Original 24/09/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::sendWatchOn(Server *target)
{
   getConnection()->sendRaw(String::printf(":%s %d %s %s * * 0 "
					   LNG_RPL_LOGON_SERVER
					   IRC2USER_EOL_CHARS,
					   (char const *)getConnection()->getDaemon()->myServer()->hostname,
					   RPL_LOGON,
					   (char const *)user->nickname,
					   (char const *)target->hostname));
}

void irc2userHandler::sendWatchOn(Channel *target)
{
   getConnection()->sendRaw(String::printf(":%s %d %s %s * * %lu "
					   LNG_RPL_LOGON_CHANNEL
					   IRC2USER_EOL_CHARS,
					   (char const *)getConnection()->getDaemon()->myServer()->hostname,
					   RPL_LOGON,
					   (char const *)user->nickname,
					   (char const *)target->name,
					   target->creationTime));
}

void irc2userHandler::sendWatchOn(User *target, String *newNick /* = 0 */)
{
   getConnection()->sendRaw(String::printf(":%s %d %s %s %s %s %lu "
					   LNG_RPL_LOGON_USER
					   IRC2USER_EOL_CHARS,
					   (char const *)getConnection()->getDaemon()->myServer()->hostname,
					   RPL_LOGON,
					   (char const *)user->nickname,
					   (newNick ?
					    (char const *)(*newNick) :
					    (char const *)target->nickname),
					   (char const *)target->username,
					   (char const *)target->getHost(user),
					   target->lastNickChange));
}


/* doNAMES - Process and reply to a NAMES command
 * Original 15/08/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::doNAMES(String &param)
{
#ifdef DO_MATCH_COUNTING
   int matches = 0;
#endif
   
   // Grab the channels list
   StringTokens channels(param);
   
   for (String chan = channels.nextToken(','); chan.length(); 
	chan = channels.nextToken(',')) {
      Channel *c = getConnection()->getDaemon()->getChannel(chan);
      
      // Make sure we got a channel, otherwise we just ignore it
      if (c) {
#ifdef DO_MATCH_COUNTING
	 // Increase the match counter
	 matches++;
#endif
	 
	 String reply = "";

	 for (Channel::member_map_t::iterator it = c->members.begin();
	      it != c->members.end(); it++) {
	    // Do we need to write a new prefix?
	    if (!reply.length()) {
	       reply = String(((c->modes & CHANMODE_PRIVATE) ?
			       "*" :
			       ((c->modes & CHANMODE_SECRET) ?
				"@" : "="))) + " " + c->name + " :";
	    }
	    
	    reply = reply +
	      (((*it).second->modes & CHANMEMMODE_OPPED) ?
	       "@" : 
	       (((*it).second->modes & CHANMEMMODE_HALFOPPED) ?
		"%" : 
		(((*it).second->modes & CHANMEMMODE_VOICED) ?
		 "+" : ""))) +
	      (*it).second->user->nickname + " ";
	    
	    // Check if we are close to breaking a limit here
	    if (reply.length() > 400) {
	       sendNumeric(RPL_NAMREPLY, reply);
	       
	       // reset the reply
	       reply = "";
	    }
	 }
	 
	 sendNumeric(RPL_NAMREPLY, reply);
      }
   }
   
   // Send end of names list
#ifdef DO_MATCH_COUNTING
   sendNumeric(RPL_ENDOFNAMES,
	       ((matches > 0) ?
		((matches == 1) ?
		 String::printf(LNG_RPL_ENDOFNAMES,
				(char const *)param) :
		 String::printf(LNG_RPL_ENDOFNAMES_MATCHES,
				(char const *)param,
				matches)) :
		String::printf(LNG_RPL_ENDOFNAMES_NOMATCH,
			       (char const *)param)));
#else
   sendNumeric(RPL_ENDOFNAMES,
	       String::printf(LNG_RPL_ENDOFNAMES,
			      (char const *)param));
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
   sendNumeric(ERR_UNKNOWNCOMMAND,
	       command + LNG_ERR_UNKNOWNCOMMAND);
}


/* parseACCEPT - Accept a user for receiving messages (efnet +g style)
 * Original , Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::parseACCEPT(irc2userHandler *handler, StringTokens *tokens)
{
   handler->sendNumeric(999, ":Oops, sorry, havn't coded this yet :(");
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
      StringMask serverMask(server);
      Server *s = TO_DAEMON->getServer(&serverMask);
      
      // Check
      if (!s) {
	 handler->sendNumeric(ERR_NOSUCHSERVER,
			      server + LNG_ERR_NOSUCHSERVER);
	 return;
      }
      
      // Is this US??
      if (s == TO_DAEMON->myServer()) {
	 doADMIN(handler, handler->user);
	 return;
      }
   
      // poll server
   }
}


/* parseAWAY
 * Original 13/08/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::parseAWAY(irc2userHandler *handler, StringTokens *tokens)
{
   // Grab the away message
   String message = tokens->nextColonToken();
   
   // Do the change
   handler->user->markAway(message);
   
   // If we have a message, the user is going away, otherwise they are 'unaway'
   if (message.length()) {
      handler->sendNumeric(RPL_NOWAWAY,
			   LNG_RPL_NOWAWAY);
   } else {
      handler->sendNumeric(RPL_UNAWAY,
			   LNG_RPL_UNAWAY);
   }
}


/* parseCONNECT
 * Original , Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::parseCONNECT(irc2userHandler *handler, StringTokens *tokens)
{
   handler->sendNumeric(999, ":Oops, sorry, havn't coded this yet :(");
}


/* parseDIE
 * Original 14/08/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::parseDIE(irc2userHandler *handler, StringTokens *tokens)
{
   String reason = tokens->rest();

   // Send out a server notice
   TO_DAEMON->
     broadcastServerNotice(SERVERNOTICE_OPER,
			   String::printf(LNG_SERVNOTICE_CMD_DIE,
					  ((char const *)
					   handler->user->nickname)));
   
   // Check if we have a reason
   if (reason.length()) {
      TO_DAEMON->shutdown(reason + " (" + 
					  handler->user->nickname + ")");
   } else {
      TO_DAEMON->shutdown(handler->user->nickname + 
					  LNG_REQUESTED_SHUTDOWN);
   }
}


/* parseGLOBOPS
 * Original 19/09/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::parseGLOBOPS(irc2userHandler *handler, StringTokens *tokens)
{
   String message = tokens->rest();
   
   // Check that we have been given something
   if (message.length()) {
      // Strip the colon if it is there (smart client if it is :)
      if (message[0] == ':') {
	 message = message.subString(1);
      }
   
      TO_DAEMON->broadcastServerNotice(SERVERNOTICE_GLOBOPS,
				     String::printf(LNG_SERVNOTICE_GLOBOPS,
						    ((char const *)
						     handler->user->nickname), 
						    (char const *)message));
      return;
   }
   
   // Complain bitterly!
   handler->sendNumeric(ERR_NOTEXTTOSEND, 
			LNG_ERR_NOTEXTTOSEND " (GLOBOPS)");
}


/* parseHELP
 * Original 13/08/01, Simon Butcher <pickle@austnet.org>
 * Note: Not the best use on CPU since it's checking a match no matter how
 *       this is called, but since this is very rarely called I decided the 
 *       code should be small rather than faster :)
 */
void irc2userHandler::parseHELP(irc2userHandler *handler, StringTokens *tokens)
{
#ifdef DO_MATCH_COUNTING
   int matches = 0;
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
#ifdef DO_MATCH_COUNTING
	    // Increase the match counter
	    matches++;
#endif
	    
	    // Send the user the help for this function
	    handler->sendNumeric(RPL_HELP,
				 String::printf("%s :%s",
						functionsTable[i].command,
						functionsTable[i].parameters));
	    
	    // If we are doing extended help, send the extended lines
	    if (extended && functionsTable[i].help) {
	       handler->sendNumeric(RPL_MOREHELP,
				    String::printf("%s :%s",
						   functionsTable[i].command,
						   functionsTable[i].help));
	    }
	 }
      }
   }

   // Send the end of help numeric
#ifdef DO_MATCH_COUNTING
   handler->sendNumeric(RPL_ENDOFHELP,
			((matches > 0) ?
			 ((matches == 1) ?
			  String::printf(LNG_RPL_ENDOFHELP,
					 (char const *)maskStr) :
			  String::printf(LNG_RPL_ENDOFHELP_MATCHES,
					 (char const *)maskStr,
					 matches)) :
			 String::printf(LNG_RPL_ENDOFHELP_NOMATCH,
					(char const *)maskStr)));
#else
   handler->sendNumeric(RPL_ENDOFHELP,
			String::printf(LNG_RPL_ENDOFHELP,
				       (char const *)maskStr));
#endif
}


/* parseHELPME
 * Original 19/09/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::parseHELPME(irc2userHandler *handler, StringTokens *tokens)
{
   String message = tokens->rest();

   // Check that we have been given something
   if (message.length()) {
      // Strip the colon if it is there (smart client if it is :)
      if (message[0] == ':') {
	 message = message.subString(1);
      }
   
      TO_DAEMON->broadcastServerNotice(SERVERNOTICE_HELPME,
				     String::printf(LNG_SERVNOTICE_HELPME,
						    ((char const *)
						     handler->user->nickname),
						    (char const *)message));
      return;
   }
   
   // Complain bitterly!
   handler->sendNumeric(ERR_NEEDMOREPARAMS,
			"HELPME" LNG_ERR_NEEDMOREPARAMS);
}


/* parseINFO
 * Original 14/08/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::parseINFO(irc2userHandler *handler, StringTokens *tokens)
{
   String server = tokens->nextToken();
   
   // Server not specified, or it is us?
   if (!server.length()) {
      doINFO(handler, handler->user);
   } else {
      StringMask serverMask(server);
      Server *s = TO_DAEMON->getServer(&serverMask);
      
      // Check
      if (!s) {
	 handler->sendNumeric(ERR_NOSUCHSERVER,
			      server + LNG_ERR_NOSUCHSERVER);
	 return;
      }
   
      // Is this US??
      if (s == TO_DAEMON->myServer()) {
	 doINFO(handler, handler->user);
	 return;
      }
   
      // poll server
   }
}


/* parseINVITE
 * Original , Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::parseINVITE(irc2userHandler *handler, StringTokens *tokens)
{
   handler->sendNumeric(999, ":Oops, sorry, havn't coded this yet :(");
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
      handler->sendNumeric(ERR_NEEDMOREPARAMS,
			   "ISON" LNG_ERR_NEEDMOREPARAMS);
      return;
   }

   String reply = ":";

   // Run through the parameter tokens
   for (String iter = tokens->nextToken(); iter.length(); 
	iter = tokens->nextToken()) {
      // Check if we have that nick in our userlist
      if (TO_DAEMON->getUser(iter)) {
	 reply = reply + iter + " ";
      }
   }
   
   // Send the reply
   handler->sendNumeric(RPL_ISON, reply);
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
      handler->sendNumeric(ERR_NEEDMOREPARAMS,
			   "JOIN" LNG_ERR_NEEDMOREPARAMS);
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
	 String redir = TO_DAEMON->redirectedChannel(&chan);
	 if (redir.length()) {
	    // Notify a savvy client
	    handler->sendNumeric(RPL_CHANREDIR,
				 String::printf(LNG_RPL_CHANREDIR,
						(char const *)chan,
						(char const *)redir));
	    // Do the swap
	    chan = redir;
	 }
      }
	    
      // Make sure joining this channel does not go over our limit
      if (handler->user->channels.size() >= MAX_CHANNELS_PER_USER) {
	 handler->sendNumeric(ERR_TOOMANYCHANNELS,
			      chan + LNG_ERR_TOOMANYCHANNELS);
	 return; // no point going through other channels.
      }
      
      // Make sure the channel name is acceptable
      if (!okChannel(&chan)) {
	 handler->sendNumeric(ERR_BADCHANNAME,
			      chan + LNG_ERR_BADCHANNAME);
	 continue;
      }

      // Find this channel in the channel list
      Channel *c = TO_DAEMON->getChannel(chan);
      
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
	 if ((c->modes & CHANMODE_REGNICKSONLY) &&
	     !(handler->user->modes & USERMODE_IDENTIFIED)) {
	    handler->sendNumeric(ERR_NONONREG,
				 chan + LNG_ERR_NONONREG_CHANNEL);
	    continue;
	 }
	 
	 // Check if this user is banned
	 if (c->onBan(handler->user)) {
	    // if this user does not have an exemption, they cannot be let in
	    if (!c->onBanExcept(handler->user)) {
	       handler->sendNumeric(ERR_BANNEDFROMCHAN,
				    chan + LNG_ERR_BANNEDFROMCHAN);
	       continue;
	    }
	 }
	 
	 // Check if this channel is set invite only
	 if (c->modes & CHANMODE_INVITE) {
	    // check if this user has an auto-invite mask set
	    if (!c->onInvite(handler->user)) {
	       handler->sendNumeric(ERR_INVITEONLYCHAN,
				    chan + LNG_ERR_INVITEONLYCHAN);
	       continue;
	    }
	 }

	 // check if this user joining the channel would break a +l (limit)
	 if ((c->limit > 0) && 
	     (c->members.size() >= c->limit)) {
	    handler->sendNumeric(ERR_CHANNELISFULL,
				 chan + LNG_ERR_CHANNELISFULL);
	    continue;
	 }
	 
	 // Check if this channel needs a key, and it is correct
	 if (c->key.length() && 
	     (c->key != keys.nextToken(','))) {
	    handler->sendNumeric(ERR_BADCHANNELKEY,
				 chan + LNG_ERR_BADCHANNELKEY);
	    continue;
	 }
	 
	 // Join the user to their new channel
	 TO_DAEMON->joinChannel(c, handler->user);
      } else {
	 // This channel did not exist. Is it a safe channel?
	 if (chan[0] == '!') {
	    // Reject their request outright. We should fix this one day?
	    handler->sendNumeric(ERR_BADCHANNAME,
				 chan + LNG_ERR_BADCHANNAME_SAFECHAN);
	    continue;
	 }

	 // Check for stuff that helpers/operators skip
	 if (!User::isHelper(handler->user)) {
	    // Check if this channel is not marked on the fail mask list
	    String reason = TO_DAEMON->failedChannel(&chan);
	    if (reason.length()) {
	       // Tell the user they cannot join, giving them the reaso
	       handler->sendNumeric(ERR_BADCHANNAME,
				    String::printf(LNG_ERR_BADCHANNAME_FAILMASK,
						   (char const *)chan,
						   (char const *)reason));
	       continue;
	    }
	 }
	 
	 // Create this channel with this user as the channel creator
	 c = new Channel(chan, TO_DAEMON->getTime(), TO_DAEMON);
	 TO_DAEMON->addChannel(c);
      
	 // Join the user to their new channel
	 TO_DAEMON->joinChannel(c, handler->user);

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
	      CHANMEMMODE_CREATOR | CHANMEMMODE_OPPED;
	 }
      }
      
      // Send topic information (if there is a topic set)
      if (c->topic.length()) {
	 handler->sendNumeric(RPL_TOPIC,
			      String::printf("%s :%s",
					     (char const *)c->name,
					     (char const *)c->topic));
	 handler->sendNumeric(RPL_TOPICWHOTIME,
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
      handler->sendNumeric(ERR_NEEDMOREPARAMS,
			   "KICK" LNG_ERR_NEEDMOREPARAMS);
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
	 handler->sendNumeric(ERR_NOTONCHANNEL,
			      chan + LNG_ERR_NOTONCHANNEL);
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
	 handler->sendNumeric(ERR_NOTONCHANNEL,
			      chan + LNG_ERR_NOTONCHANNEL);
	 continue;
      }
      
      // Does this fella have the rights to even contemplate kicking someone?
      if (!(kicker->modes & CHANMEMMODE_OPPED) &&
	  !(kicker->modes & CHANMEMMODE_HALFOPPED)) {
	 handler->sendNumeric(ERR_CHANOPRIVSNEEDED,
			      chan + LNG_ERR_CHANOPRIVSNEEDED);
	 continue;
      }
      
      // Try and get the target user record
      User *u = TO_DAEMON->getUser(target);
      
      // Check
      if (!u) {
	 handler->sendNumeric(ERR_NOSUCHNICK,
			      target + LNG_ERR_NOSUCHNICK_NICK);
			      
	 continue;
      }

      // Try and get the channel member record for this user
      ChannelMember *kickee = c->getMember(u);
      
      // Check
      if (!kickee) {
	 handler->sendNumeric(ERR_USERNOTINCHANNEL,
			      String::printf(LNG_ERR_USERNOTINCHANNEL,
					     (char const *)u->nickname,
					     (char const *)chan));
	 continue;
      }
      
      // Check if this user is immune from kicks
      if (u->modes & USERMODE_NONKICKABLE) {
	 handler->sendNumeric(ERR_ISCHANSERVICE,
			      String::printf(LNG_ERR_ISCHANSERVICE,
					     (char const *)u->nickname,
					     (char const *)c->name));
	 continue;
      }
      
      // Check that the kickee is not an op while this guy is just a half op
      if ((kickee->modes & CHANMEMMODE_OPPED) &&
	  (kicker->modes & CHANMEMMODE_HALFOPPED) &&
	  !(kicker->modes & CHANMEMMODE_OPPED)) {
	 handler->sendNumeric(ERR_CHANOPRIVSNEEDED,
			      chan + LNG_ERR_CHANOPRIVSNEEDED_HALFOPERVSOPER);
	 continue;
      }
      
      // Finally, all things say we can kick this bloke out!
      TO_DAEMON->kickChannelMember(c, handler->user, u, &reason);
   }
}


/* parseKILL
 * Original 22/09/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::parseKILL(irc2userHandler *handler, StringTokens *tokens)
{
   // Check we have at least one parameter
   if (tokens->countTokens() < 3) {
      handler->sendNumeric(ERR_NEEDMOREPARAMS,
			   "KILL" LNG_ERR_NEEDMOREPARAMS);
      return;
   }

   // Grab the varables
   String target = tokens->nextToken();
   String reason = tokens->nextColonToken();
   
   // Find the target
   User *u = TO_DAEMON->getUser(target);
   
   // Check
   if (!u) {
      handler->sendNumeric(ERR_NOSUCHNICK,
			   target + LNG_ERR_NOSUCHNICK_NICK);
      return;
   }
   
   // Check if this user is immune from kills
   if (u->modes & USERMODE_NONKICKABLE) {
      handler->sendNumeric(ERR_CANTKILLSERVICES, LNG_ERR_CANTKILLSERVICES);
      // Broadcast this blunder
      return;
   }
   
   // Check permissions here
   
   // Do the kill
   TO_DAEMON->killUser(u, &handler->user->nickname, &reason);

   /* This is 'obsolete' but it is a good idea if the oper has server notices
    * turned off just to confirm the kill.
    */
   handler->sendNumeric(RPL_KILLDONE,
			target + LNG_RPL_KILLDONE);
}


/* parseLINKS
 * Original 27/08/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::parseLINKS(irc2userHandler *handler, StringTokens *tokens)
{
   String server = tokens->nextToken();
   String mask = tokens->nextToken();
   
   // Server not specified, or it is us?
   if (!server.length()) {
      doLINKS(handler, handler->user, &mask);
   } else {
      StringMask serverMask(server);
      Server *s = TO_DAEMON->getServer(&serverMask);
      
      // Check
      if (!s) {
	 handler->sendNumeric(ERR_NOSUCHSERVER,
			      server + LNG_ERR_NOSUCHSERVER);
	 return;
      }
   
      // Is this US??
      if (s == TO_DAEMON->myServer()) {
	 doLINKS(handler, handler->user, &mask);
	 return;
      }
   
      // poll server
   }
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
   for (Daemon::channel_map_t::iterator it = 
	TO_DAEMON->channels.begin();
	it != TO_DAEMON->channels.end(); 
	it++) {
      handler->sendNumeric(RPL_LIST,
			   String::printf("%s %d :%s",
					  (char const *)(*it).second->name,
					  (*it).second->members.size(),
					  (char const *)(*it).second->topic));
   }

   // Then local channels
   for (Daemon::channel_map_t::iterator it = 
	TO_DAEMON->localChannels.begin();
	it != TO_DAEMON->localChannels.end(); 
	it++) {
      handler->sendNumeric(RPL_LIST,
			   String::printf("%s %d :%s",
					  (char const *)(*it).second->name,
					  (*it).second->members.size(),
					  (char const *)(*it).second->topic));
   }
   
   // Send the end of the list message
   handler->sendNumeric(RPL_LISTEND, LNG_RPL_LISTEND);
}


/* parseLOCOPS
 * Original 19/09/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::parseLOCOPS(irc2userHandler *handler, StringTokens *tokens)
{
   String message = tokens->rest();
   
   // Check that we have been given something
   if (message.length()) {
      // Strip the colon if it is there (smart client if it is :)
      if (message[0] == ':') {
	 message = message.subString(1);
      }
   
      TO_DAEMON->broadcastServerNotice(SERVERNOTICE_LOCOPS,
				     String::printf(LNG_SERVNOTICE_LOCOPS,
						    ((char const *)
						     handler->user->nickname), 
						    (char const *)message));
      return;
   }
   
   // Complain bitterly!
   handler->sendNumeric(ERR_NOTEXTTOSEND, 
			LNG_ERR_NOTEXTTOSEND " (LOCOPS)");
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
      doLUSERS(handler, handler->user, &mask);
   } else {
      StringMask serverMask(server);
      Server *s = TO_DAEMON->getServer(&serverMask);
      
      // Check
      if (!s) {
	 handler->sendNumeric(ERR_NOSUCHSERVER,
			      server + LNG_ERR_NOSUCHSERVER);
	 return;
      }
   
      // Is this US??
      if (s == TO_DAEMON->myServer()) {
	 doLUSERS(handler, handler->user, &mask);
	 return;
      }
   
      // poll server
   }
}


/* parseMODE
 * Original 25/08/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::parseMODE(irc2userHandler *handler, StringTokens *tokens)
{
   // Check we have at least one parameter
   if (tokens->countTokens() < 2) {
      handler->sendNumeric(ERR_NEEDMOREPARAMS,
			   "MODE" LNG_ERR_NEEDMOREPARAMS);
      return;
   }

   // Grab the target and the modes string
   String target = tokens->nextToken();
   String modes = tokens->nextToken();
   
   // Is the target a user or a channel
   if (isChannel(&target)) {
      // Grab the channel
      Channel *c = TO_DAEMON->getChannel(target);
      
      // Make sure we got that channel
      if (!c) {
	 handler->sendNumeric(ERR_NOSUCHCHANNEL,
			      target + LNG_ERR_NOSUCHCHANNEL);
	 return;
      }
      
      // Check if this user can see this channel
      if (Channel::isHidden(c) && 
	  !c->getMember(handler->user)) {
	 handler->sendNumeric(ERR_NOTONCHANNEL,
			      c->name + LNG_ERR_NOTONCHANNEL);
	 return;
      }
      
      // Are we setting modes or just getting the modes
      if (modes.length()) {
	 // Changing modes/Receiving specific mode information. Fob it off..
	 c->processModes(handler, handler->user, &modes, tokens);
      } else {
	 // Send the channel modes for this channel
	 handler->sendNumeric(RPL_CHANNELMODEIS,
			      c->name + " " + makeChannelModes(c));
	 
	 // Also, send the creation time for the channel
	 handler->sendNumeric(RPL_CREATIONTIME,
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
	 String modeString = handler->processUserModes(&modes, tokens);
	 
	 // If the mode string has anything in it, send the user the changes
	 if (modeString.length()) {
	    handler->sendUserMode(0, modeString);
	 }
	 
   	 return;
      }
      
      // Show the modes (with the parameters)
      handler->sendNumeric(RPL_UMODEIS, makeUserModes(handler->user));
      return;
   }

#ifdef HELPERS_CAN_SEE_USER_MODES   
   // If this user is a helper, we can continue
   if (!User::isHelper(handler->user)) {
      handler->sendNumeric(ERR_USERSDONTMATCH,
			   LNG_ERR_USERSDONTMATCH_MODE);
      return;
   }
   
   // Look the user up
   User *u = TO_DAEMON->getUser(target);
   
   // Check, show the modes if possible
   if (u) {
      // Show the modes (with the parameters)
      handler->sendNumeric(RPL_OTHERUMODEIS, 
			   String::printf("%s %s",
					  (char const *)u->nickname,
					  (char const *)makeUserModes(u)));
      return;
   }
#endif
   
   // Ok perhaps not. If this user is not an oper, skip the last check
   if (!User::isOper(handler->user)) {
      handler->sendNumeric(ERR_NOSUCHNICK,
			   target + LNG_ERR_NOSUCHNICK);
      return;
   }
   
   // Woah, must be a server! Try and find this server
   StringMask serverMask(target);
   Server *server = TO_DAEMON->getServer(&serverMask);
   
   // Check
   if (!server) {
      handler->sendNumeric(ERR_NOSUCHNICK,
			   target + LNG_ERR_NOSUCHNICK_OR_SERVER);
      return;
   }

   // Is this server unlocked for mode changes?
   if (server->modes & SERVERMODE_MODELOCK) {
      handler->sendNumeric(ERR_SERVERMODELOCK,
			   server->hostname + LNG_ERR_SERVERMODELOCK);
      return;
   }
   
   // OK! Are we setting modes or just getting the modes
   if (modes.length()) {
      TO_DAEMON->changeServerMode(server, handler, handler->user, &modes,
				tokens);
      return;
   }

   // Just show them. PHEW!
   handler->sendNumeric(RPL_SERVMODEIS, 
			String::printf("%s %s",
				       (char const *)server->hostname,
				       (char const *)makeServerModes(server)));
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
      StringMask serverMask(server);
      Server *s = TO_DAEMON->getServer(&serverMask);
      
      // Check
      if (!s) {
	 handler->sendNumeric(ERR_NOSUCHSERVER,
			      server + LNG_ERR_NOSUCHSERVER);
	 return;
      }
   
      // Is this US??
      if (s == TO_DAEMON->myServer()) {
	 doMOTD(handler, handler->user);
	 return;
      }
   
      // poll server
   }
}


/* parseNAMES
 * Original 15/08/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::parseNAMES(irc2userHandler *handler, StringTokens *tokens)
{
   // Check we have at least one parameter
   if (tokens->countTokens() < 2) {
      handler->sendNumeric(ERR_NEEDMOREPARAMS,
			   "NAMES" LNG_ERR_NEEDMOREPARAMS);
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
      handler->sendNumeric(ERR_NONICKNAMEGIVEN,
			   LNG_ERR_NONICKNAMEGIVEN);
      return;
   }
   
   // Check the nickname for an acceptable length and ok characters
   if (!okNickname(&nick)) {
      handler->sendNumeric(ERR_ERRONEUSNICKNAME,
			   nick + LNG_ERR_ERRONEUSNICKNAME);
      return;
   }
   
   // Checks that IRC operators can bypass
   if (!User::isOper(handler->user)) {
      // Check if this nickname is marked on the fail mask list
      String reason = TO_DAEMON->failedNickname(&nick);
      if (reason.length()) {
	 // Tell the user they cannot join, giving them the reaso
	 handler->sendNumeric(ERR_ERRONEUSNICKNAME,
			      String::printf(LNG_ERR_ERRONEUSNICKNAME_W_REASON,
					     (char const *)nick,
					     (char const *)reason));
	 return;
      }
      
      // Check if this user is already at this nickname
      if (handler->user->nickname == nick) {
	 // Fake a nickname change, since none happened (stupid client)
	 handler->sendNick(handler->user, &handler->user->nickname);
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
	    if (!((cm->modes & CHANMEMMODE_OPPED) ||
		  (cm->modes & CHANMEMMODE_HALFOPPED) ||
		  (cm->modes & CHANMEMMODE_VOICED)) &&
		((*it).second->modes & CHANMODE_EVENT)) {
	       handler->sendNumeric(ERR_EVENTNICKCHANGE,
				    (*it).second->name + 
				    LNG_ERR_EVENTNICKCHANGE);
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
       TO_DAEMON->getUser(nick)) {
      handler->sendNumeric(ERR_NICKNAMEINUSE,
			   nick + LNG_ERR_NICKNAMEINUSE);
      return;
   }

   // Tell the user we are changing their nickname
   handler->sendNick(handler->user, &nick);

   // Do the change (this also broadcasts the nick change)
   TO_DAEMON->changeUserNick(handler->user, &nick);
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
      if (isChannel(&iter)) {
	 Channel *c = 0;
	 
	 // Most of the time the user will be on the channel. Check their list
	 c = handler->user->getChannel(iter);
	 
	 // Check.
	 if (!c) {
	    // Try and get the channel from the main channel list
	    c = TO_DAEMON->getChannel(iter);
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
	       if (!(cm->modes & CHANMEMMODE_OPPED) &&
		   !(cm->modes & CHANMEMMODE_HALFOPPED) &&
		   !(cm->modes & CHANMEMMODE_VOICED)) {
		  // Check if this channel is moderated
		  if (c->modes & CHANMODE_MODERATED) {
		     continue;
		  }
		  
		  // Check if this is a registered user only channel
		  if ((c->modes & CHANMODE_REGNICKSONLY) && 
		      !(handler->user->modes & USERMODE_IDENTIFIED)) {
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
	       if (c->modes & CHANMODE_NOOUTSIDEMSG) {
		  continue;
	       }
	       
	       // Check if this is a registered user only channel
	       if ((c->modes & CHANMODE_REGNICKSONLY) && 
		   !(handler->user->modes & USERMODE_IDENTIFIED)) {
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
	    User *u = TO_DAEMON->getUser(iter);
	    
	    // Did we not get a user record and this user is not an oper?
	    if (!u) {
	       if (!User::isOper(handler->user)) {
		  continue;
	       }
	    } else {
	       // Check if this user is allowed to send to this user
	       if ((u->modes & USERMODE_REGNICKSMSG) &&
		   (!(handler->user->modes & USERMODE_IDENTIFIED) ||
		    !User::isOper(handler->user))) {
		  continue;
	       }
	       
	       // Check if this user has not been silenced
	       if (u->isSilencing(handler->user)) {
		  continue;
	       }
	       
	       // Check if this user is doing the caller-id thing (+g)
	       if (u->modes & USERMODE_IGNORING) {
		  // This really needs to be completed.
		  continue;
	       }

	       // Send this message to the user
	       TO_DAEMON->routeTo(u)->sendNotice(handler->user, &u->nickname, 
						 message);
	       continue;
	    }
	 } else {
	    // Send the message right back at them without doing anything fancy
	    handler->sendNotice(handler->user, &handler->user->nickname, 
				message);
	    continue;
	 }
      }
      
      // If we got down here, the user must be an operator, perhaps its a mask?
      handler->sendNumeric(999,
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
      handler->sendNumeric(ERR_NEEDMOREPARAMS,
			   "OPER" LNG_ERR_NEEDMOREPARAMS);
      return;
   }

   // Check that this user is not already an oper
   if (User::isOper(handler->user)) {
      handler->sendNumeric(RPL_YOUREOPER, LNG_RPL_YOUREOPER_ALREADY);
      return;
   }
   
   // Check if the server is in NOOPER mode
   if (TO_DAEMON->myServer()->modes & SERVERMODE_NOOP) {
      handler->sendNumeric(ERR_NOOPERHOST, LNG_ERR_NOOPERHOST_NOOP);
      return;
   }

   // Grab the nickname and password pair
   String nickname = tokens->nextToken();
   String password = tokens->nextToken();
   
   // Get the operator
   Operator *oper = TO_DAEMON->getOperator(&nickname);
   
   // Check.
   if (!oper) {
      handler->sendNumeric(ERR_NOOPERHOST, LNG_ERR_NOOPERHOST);
      return;
   }
   
   // Try this password against this operator
   if (oper->getPassword() != makeOperPassword(&nickname, &password)) {
      handler->sendNumeric(ERR_PASSWDMISMATCH, LNG_ERR_PASSWDMISMATCH);
      return;
   }

   // Work out what log messages to send, and what modes to set this user
   TYPE_USERMODES modes = 0;
   if (oper->isGlobal()) {
      // Log this action
      TO_DAEMON->logger(handler->user->nickname + " is now a global operator",
		      LOGPRI_NOTICE);

      // Count this oper in on the number of global opers/helpers on-line
      TO_DAEMON->numOpers++;
      TO_DAEMON->numHelpers++;
      
      // Fix the modes
#ifdef GIVE_MODES_GLOBALOP
      modes = USERMODE_GLOBALOPER | USERMODE_HELPER | GIVE_MODES_GLOBALOP;
#else
      modes = USERMODE_GLOBALOPER | USERMODE_HELPER;
#endif
   } else {
      // Log this action
      TO_DAEMON->logger(handler->user->nickname + " is now a local operator",
		      LOGPRI_NOTICE);
      
      // Fix the modes
#ifdef GIVE_MODES_LOCALOP
      modes = USERMODE_LOCALOPER | GIVE_MODES_LOCALOP;
#else
      modes = USERMODE_LOCALOPER;
#endif
   }
   
   /* Send the user their new modes. This messy looking slab here makes sure
    * only the appropriate modes are went to the user to avoid client
    * confusion.
    */
   handler->sendUserMode(0, makeUserModes(handler->user->modes ^ 
					  (handler->user->modes | modes)));
   handler->user->modes |= modes;
   
   // Tell the user they are now an IRC Operator, finally!
   handler->sendNumeric(RPL_YOUREOPER, LNG_RPL_YOUREOPER);
}


/* parsePART
 * Original , Simon Butcher <pickle@austnet.org>
 * Note: We have no need for 'ERR_NOSUCHCHANNEL' here, since we look up
 *       the channel differently to how the ircd used in the RFC does
 */
void irc2userHandler::parsePART(irc2userHandler *handler, StringTokens *tokens)
{
   // Check we have at least one parameter
   if (tokens->countTokens() < 2) {
      handler->sendNumeric(ERR_NEEDMOREPARAMS,
			   "PART" LNG_ERR_NEEDMOREPARAMS);
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
	 handler->sendNumeric(ERR_NOTONCHANNEL,
			      chan + LNG_ERR_NOTONCHANNEL);
	 continue;
      } 
      
      // Do the part
      TO_DAEMON->partChannel(c, handler->user, &reason);
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
   handler->sendNumeric(999, ":Oops, sorry, havn't coded this yet :(");
}


/* parsePRIVMSG
 * Original 01/09/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::parsePRIVMSG(irc2userHandler *handler, StringTokens *tokens)
{
   String targetStr(tokens->nextToken());
   
   // Check the targets
   if (!targetStr.length()) {
      handler->sendNumeric(ERR_NORECIPIENT, LNG_ERR_NORECIPIENT);
      return;
   }
   
   // Grab our fields
   StringTokens targets(targetStr);
   String message = tokens->nextColonToken();

   // This command effects the user idle time, so we must change it here
   handler->getConnection()->touch();
   
   // Check the length...
   if (!message.length()) {
      handler->sendNumeric(ERR_NOTEXTTOSEND, 
			   LNG_ERR_NOTEXTTOSEND "(PRIVMSG)");
      return;
   }
   
   // Check the message here - ctcp dcc ??
   
   // Run through the list of recipients
   for (String iter = targets.nextToken(','); iter.length();
	iter = targets.nextToken(',')) {
      // Is this a channel or a user?
      if (isChannel(&iter)) {
	 Channel *c = 0;
	 
	 // Most of the time the user will be on the channel. Check their list
	 c = handler->user->getChannel(iter);
	 
	 // Check.
	 if (!c) {
	    // Try and get the channel from the main channel list
	    c = TO_DAEMON->getChannel(iter);
	 }
	 
	 // Check.
	 if (!c) {
	    if (!User::isOper(handler->user)) {
	       handler->sendNumeric(ERR_NOSUCHNICK,
				    iter + LNG_ERR_NOSUCHNICK_CHANNEL);
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
	       if (!(cm->modes & CHANMEMMODE_OPPED) &&
		   !(cm->modes & CHANMEMMODE_HALFOPPED) &&
		   !(cm->modes & CHANMEMMODE_VOICED)) {
		  // Check if this channel is moderated
		  if (c->modes & CHANMODE_MODERATED) {
		     handler->sendNumeric(ERR_CANNOTSENDTOCHAN,
					  iter + 
					  LNG_ERR_CANNOTSENDTOCHAN_MODERATED);
		     continue;
		  }
		  
		  // Check if this is a registered user only channel
		  if ((c->modes & CHANMODE_REGNICKSONLY) && 
		      !(handler->user->modes & USERMODE_IDENTIFIED)) {
		     handler->sendNumeric(ERR_CANNOTSENDTOCHAN,
					  iter + 
					  LNG_ERR_CANNOTSENDTOCHAN_REGNICKSONLY);
		     continue;
		  }
		  
		  // Check if this user is banned
		  if (c->onBan(handler->user) && 
		      !c->onBanExcept(handler->user)) {
		     handler->sendNumeric(ERR_CANNOTSENDTOCHAN,
					  iter + 
					  LNG_ERR_CANNOTSENDTOCHAN_BANNED);
		     continue;
		  }
	       }
	    } else {
	       // Check if the user can still send while not on the channel
	       if (c->modes & CHANMODE_NOOUTSIDEMSG) {
		  handler->sendNumeric(ERR_CANNOTSENDTOCHAN,
				       iter + 
				       LNG_ERR_CANNOTSENDTOCHAN_NOOUTSIDEMSG);
		  continue;
	       }
	       
	       // Check if this is a registered user only channel
	       if ((c->modes & CHANMODE_REGNICKSONLY) && 
		   !(handler->user->modes & USERMODE_IDENTIFIED)) {
		  handler->sendNumeric(ERR_CANNOTSENDTOCHAN,
				       iter + 
				       LNG_ERR_CANNOTSENDTOCHAN_REGNICKSONLY);
		  continue;
	       }
	       
	       // Check if this user is banned from this channel
	       if (c->onBan(handler->user) && !c->onBanExcept(handler->user)) {
		  handler->sendNumeric(ERR_CANNOTSENDTOCHAN,
				       iter + 
				       LNG_ERR_CANNOTSENDTOCHAN_BANNED);
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
	    User *u = TO_DAEMON->getUser(iter);
	    
	    // Did we not get a user record and this user is not an oper?
	    if (!u) {
	       if (!User::isOper(handler->user)) {
		  handler->sendNumeric(ERR_NOSUCHNICK,
				       iter + 
				       LNG_ERR_NOSUCHNICK_NICK);
		  continue;
	       }
	    } else {
	       // Check if this user is allowed to send to this user
	       if ((u->modes & USERMODE_REGNICKSMSG) &&
		   (!(handler->user->modes & USERMODE_IDENTIFIED) ||
		    !User::isOper(handler->user))) {
		  handler->sendNumeric(ERR_CANNOTSENDTONICK,
				       iter + LNG_ERR_CANNOTSENDTONICK_MUSTID);
		  continue;
	       }
	       
	       // Check if this user has not been silenced
	       if (u->isSilencing(handler->user)) {
		  continue;
	       }
	       
	       // Check if this user is doing the caller-id thing (+g)
	       if (u->modes & USERMODE_IGNORING) {
		  // This really needs to be completed.
		  continue;
	       }

	       // Send this message to the user
	       TO_DAEMON->routeTo(u)->sendPrivmsg(handler->user, &u->nickname, 
						  message);
	       
	       // If this user is 'away', tell the user
	       if (u->awayMessage.length()) {
		  handler->sendNumeric(RPL_AWAY,
				       String::printf("%s :%s",
						      (char const *)u->nickname,
						      (char const *)u->awayMessage));
	       }
	       continue;
	    }
	 } else {
	    // Send the message right back at them without doing anything fancy
	    handler->sendPrivmsg(handler->user, &handler->user->nickname, 
				 message);
	    continue;
	 }
      }
      
      // If we got down here, the user must be an operator, perhaps its a mask?
      handler->sendNumeric(999,
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
      handler->sendNumeric(ERR_NOSUCHNICK,
			   iter + LNG_ERR_NOSUCHNICK);
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
      handler->getConnection()->goodbye(String::printf(LNG_BYE_BYE_USER,
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
   TO_DAEMON->rehash(handler, handler->user);
}


/* parseRESTART
 * Original , Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::parseRESTART(irc2userHandler *handler, StringTokens *tokens)
{
   handler->sendNumeric(999, ":Oops, sorry, havn't coded this yet :(");
}


/* parseSERVLIST
 * Original , Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::parseSERVLIST(irc2userHandler *handler, StringTokens *tokens)
{
   handler->sendNumeric(999, ":Oops, sorry, havn't coded this yet :(");
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
	      if (handler->user->silences.size() > MAX_SILENCES_PER_USER) {
		 handler->sendNumeric(ERR_SILELISTFULL,
				      param + LNG_ERR_SILELISTFULL);
		 return;
	      }
	      
	      // Grab the mask
	      mask = fixToIdentityMask(&param);

	      // Check if the user is trying to ignore themselves (stupid)
	      if (mask.matches(handler->user->getAddress()) ||
		  mask.matches(handler->user->getVWAddress())) {
		 // Tell them why we did nothing??
		 return;
	      }
	      
	      // Add the mask
	      if (TO_DAEMON->addUserSilence(handler->user, &mask)) {
		 // Send confirmation to the client
		 handler->sendSilence(handler->user, true, &mask);
	      }
	   }
	 return;
       case '-': // Removing a mask
	   {
	      param = param.subString(1);
	      
	      // Grab the mask
	      mask = fixToIdentityMask(&param);
	      
	      // Add the mask
	      if (TO_DAEMON->delUserSilence(handler->user, &mask)) {
		 // Send confirmation to the client
		 handler->sendSilence(handler->user, false, &mask);
	      }
	   }
	 return;
      }
      
      /* If we got here, we must be requesting a silence list of a user. We
       * need to be an operator or a helper to do this, however.
       */
      if (!(handler->user->modes & USERMODE_GLOBALOPER) &&
	  !(handler->user->modes & USERMODE_LOCALOPER) &&
	  !(handler->user->modes & USERMODE_HELPER)) {
	 /* We SHOULD tell them permission denied or something but *sigh* this
	  * appears to be how dalnet ppl do it. Anyone think we should just
	  * send a customised permission denied?
	  */
	 handler->sendNumeric(RPL_ENDOFSILELIST, LNG_RPL_ENDOFSILELIST);
	 return;
      }
      
      // Look up the requested user, this will follow through to the list below
      u = TO_DAEMON->getUser(param);
      
      // Check
      if (!u) {
	 handler->sendNumeric(ERR_NOSUCHNICK, LNG_ERR_NOSUCHNICK_NICK);
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
      handler->sendNumeric(RPL_SILELIST,
			   String::printf("%s %s",
					  (char const *)u->nickname,
					  (char const *)(*it).getMask()));
   }
   
   // Send the end of the list numeric
   handler->sendNumeric(RPL_ENDOFSILELIST, LNG_RPL_ENDOFSILELIST);
}


/* parseSQUIT
 * Original , Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::parseSQUIT(irc2userHandler *handler, StringTokens *tokens)
{
   handler->sendNumeric(999, ":Oops, sorry, havn't coded this yet :(");
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
      handler->sendNumeric(ERR_NEEDMOREPARAMS,
			   "STATS" LNG_ERR_NEEDMOREPARAMS);
   }
   
   // This command effects the user idle time, so we must change it here
   handler->getConnection()->touch();
   
   // Server not specified, or it is us?
   if (!server.length()) {
      doSTATS(handler, handler->user, &request);
   } else {
      StringMask serverMask(server);
      Server *s = TO_DAEMON->getServer(&serverMask);
      
      // Check
      if (!s) {
	 handler->sendNumeric(ERR_NOSUCHSERVER,
			      server + LNG_ERR_NOSUCHSERVER);
	 return;
      }
   
      // Is this US??
      if (s == TO_DAEMON->myServer()) {
	 doSTATS(handler, handler->user, &request);
	 return;
      }
   
      // poll server
   }
}


/* parseTIME
 * Original 27/08/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::parseTIME(irc2userHandler *handler, StringTokens *tokens)
{
   String target = tokens->nextToken();
   
   // Server not specified, or it is us?
   if (!target.length()) {
      doTIME(handler, handler->user);
   } else {
      // poll server
   }
}


/* parseTOPIC
 * Original 19/08/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::parseTOPIC(irc2userHandler *handler, StringTokens *tokens)
{
   // Check the parameter count
   if (tokens->countTokens() < 2) {
      handler->sendNumeric(ERR_NEEDMOREPARAMS,
			   "TOPIC" LNG_ERR_NEEDMOREPARAMS);
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
      handler->sendNumeric(ERR_NOSUCHCHANNEL,
			   channel + LNG_ERR_NOSUCHCHANNEL);
      return;
   } 

   // Are we grabbing or setting the topic?
   if (!topic.length()) {
      // Do we have a topic to tell?
      if (c->topic.length()) {
	 handler->sendNumeric(RPL_TOPIC,
			      String::printf("%s :%s",
					     (char const *)c->name,
					     (char const *)c->topic));
	 handler->sendNumeric(RPL_TOPICWHOTIME,
			      String::printf("%s %s :%lu",
					     (char const *)c->name,
					     (char const *)c->topicWho,
					     c->topicTime));
	 return;
      } 
	
      // No topic...
      handler->sendNumeric(RPL_NOTOPIC,
			   c->name + LNG_RPL_NOTOPIC);
      
      return;
   }
   
   // Grab the member record for this user
   ChannelMember *cm = c->getMember(handler->user);
   
   // Check
   if (!cm) {
      handler->sendNumeric(ERR_NOTONCHANNEL,
			   channel + LNG_ERR_NOTONCHANNEL);
      return;
   }
   
   // Check the access, is the channel +t?
   if (c->modes & CHANMODE_TOPICLOCK) {
      // Check if this user is an operator
      if (!(cm->modes & CHANMEMMODE_OPPED) &&
	  !(cm->modes & CHANMEMMODE_HALFOPPED)) {
	 handler->sendNumeric(ERR_CHANOPRIVSNEEDED,
			      channel + LNG_ERR_CHANOPRIVSNEEDED);
	 return;
      }
   }

   // Change the topic, finally.
   TO_DAEMON->changeChannelTopic(c, handler->user, &topic);
}


/* parseTRACE
 * Original , Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::parseTRACE(irc2userHandler *handler, StringTokens *tokens)
{
   handler->sendNumeric(999, ":Oops, sorry, havn't coded this yet :(");
}


/* parseTRACEROUTE
 * Original , Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::parseTRACEROUTE(irc2userHandler *handler, StringTokens *tokens)
{
   handler->sendNumeric(999, ":Oops, sorry, havn't coded this yet :(");
}


/* parseUSERHOST
 * Original 21/08/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::parseUSERHOST(irc2userHandler *handler, StringTokens *tokens)
{
   // Check we have at least one parameter
   if (tokens->countTokens() < 2) {
      handler->sendNumeric(ERR_NEEDMOREPARAMS,
			   "USERHOST" LNG_ERR_NEEDMOREPARAMS);
      return;
   }

   int count = 0;
   String reply = ":";

   // Run through the parameter tokens
   for (String iter = tokens->nextToken(); iter.length();
	iter = tokens->nextToken()) {
      User *u = TO_DAEMON->getUser(iter);
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
   handler->sendNumeric(RPL_USERHOST, reply);
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
      // poll server
   }
}


/* parseWALLCHOPS
 * Original , Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::parseWALLCHOPS(irc2userHandler *handler, StringTokens *tokens)
{
   handler->sendNumeric(999, ":Oops, sorry, havn't coded this yet :(");
}


#ifdef ALLOW_OPER_WALLOPS
/* parseWALLOPS
 * Original 18/09/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::parseWALLOPS(irc2userHandler *handler, StringTokens *tokens)
{
   String message = tokens->nextColonToken();
   
   // Check that we have been given something
   if (message.length()) {
      TO_DAEMON->broadcastWallops(handler->user, &message);
      return;
   }
   
   // Complain bitterly!
   handler->sendNumeric(ERR_NOTEXTTOSEND, 
			LNG_ERR_NOTEXTTOSEND " (WALLOPS)");
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
       case 'S': // Something.
	   {
	      handler->sendNumeric(RPL_WATCHSTAT,
				   String::printf(LNG_RPL_WATCHSTAT,
						  handler->user->local->watches.size(),
						  (int)0));

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
		    handler->sendNumeric(RPL_WATCHLIST, reply);
		    
		    // reset the reply
		    reply = ":";
		 }
	      }
	      
	      // Anything left to send?
	      if (reply.length()) {
		 handler->sendNumeric(RPL_WATCHLIST, reply);
	      }

	      // Send the end of list marker
	      handler->sendNumeric(RPL_ENDOFWATCHLIST,
				   String::printf(LNG_RPL_ENDOFWATCHLIST,
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
	    if (isChannel(&watch)) {
	       // Look for this channel
	       Channel *c = TO_DAEMON->getChannel(watch);
	       
	       // Check
	       if (c) {
		  handler->
		    sendNumeric(RPL_NOWON,
				String::printf("%s * * %lu "
					       LNG_RPL_NOWON_CHANNEL,
					       ((char const *)
						c->name),
					       c->creationTime));
		  continue;
	       }
	    }
	    
	    // Look for a user, this is the most likely case though
	    User *u = TO_DAEMON->getUser(watch);
	    
	    // Check
	    if (u) {
	       handler->sendNumeric(RPL_NOWON,
				    String::printf("%s %s %s %lu "
						   LNG_RPL_NOWON_USER,
						   ((char const *)
						    u->nickname),
						   ((char const *)
						    u->username),
						   ((char const *)
						    u->getHost(handler->user)),
						   u->lastNickChange));
	       continue;
	    }
	    
	    // If this user is an oper, check for a server..
	    if ((handler->user->modes & USERMODE_GLOBALOPER) ||
		(handler->user->modes & USERMODE_LOCALOPER) ||
		(handler->user->modes & USERMODE_HELPER)) {
	       Server *s = TO_DAEMON->getServer(watch);
	       
	       // Check
	       if (s) {
		  handler->
		    sendNumeric(RPL_NOWON,
				String::printf("%s * * 0 "
					       LNG_RPL_NOWON_SERVER,
					       ((char const *)
						u->nickname)));
		  continue;
	       }
	    }
	    
	    // Only send this if they want it (uppercase L)
	    if (isupper(param[0])) {
	       // If we got here, whatever it is must be offline.
	       handler->sendNumeric(RPL_NOWOFF,
				    String::printf("%s * * 0 "
						   LNG_RPL_NOWOFF,
						   (char const *)watch));
	    }
	 }
	 
	 // Send the end of watch list message
	 handler->sendNumeric(RPL_ENDOFWATCHLIST, 
			      String::printf(LNG_RPL_ENDOFWATCHLIST,
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
		  MAX_WATCHES_PER_USER) {
		 // Complain
		 handler->sendNumeric(ERR_TOOMANYWATCH,
				      String::printf(LNG_ERR_TOOMANYWATCH,
						     (char const *)param,
						     MAX_WATCHES_PER_USER));
		 
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
	      if (isChannel(&param)) {
		 // Look for this channel
		 Channel *c = TO_DAEMON->getChannel(param);
		 
		 // Check
		 if (c) {
		    handler->
		      sendNumeric(RPL_NOWON,
				  String::printf("%s * * %lu "
						 LNG_RPL_NOWON_CHANNEL,
						 ((char const *)
						  c->name),
						 c->creationTime));
		    continue;
		 }
	      }
	      
	      // Look for a user, this is the most likely case though
	      User *u = TO_DAEMON->getUser(param);
	      
	      // Check
	      if (u) {
		 handler->sendNumeric(RPL_NOWON,
				      String::printf("%s %s %s %lu "
						     LNG_RPL_NOWON_USER,
						     ((char const *)
						      u->nickname),
						     ((char const *)
						      u->username),
						     ((char const *)
						      u->getAddress(handler->user)),
						     u->lastNickChange));
		 continue;
	      }
	      
	      // If this user is an oper, check for a server..
	      if ((handler->user->modes & USERMODE_GLOBALOPER) ||
		  (handler->user->modes & USERMODE_LOCALOPER) ||
		  (handler->user->modes & USERMODE_HELPER)) {
		 Server *s = TO_DAEMON->getServer(param);
		 
		 // Check
		 if (s) {
		    handler->
		      sendNumeric(RPL_NOWON,
				  String::printf("%s * * 0 "
						 LNG_RPL_NOWON_SERVER,
						 ((char const *)
						  s->hostname)));
		    continue;
		 }
	      }
	      
	      // If we got here, whatever it is must be offline.
	      handler->sendNumeric(RPL_NOWOFF,
				   String::printf("%s * * 0 "
						  LNG_RPL_NOWOFF,
						  (char const *)param));
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
	 handler->sendNumeric(RPL_WATCHOFF,
			      String::printf("%s * * 0 " 
					     LNG_RPL_WATCHOFF,
					     (char const *)param));
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
#ifdef DO_MATCH_COUNTING
   int matches = 0;
#endif
   
   // Grab the mask the user is looking for
   String maskStr = tokens->nextToken();

   // Are we looking for a channel, or a mask/nickname?
   if (isChannel(&maskStr)) {
      Channel *chan = TO_DAEMON->getChannel(maskStr);

      // Make sure we got that channel record, and this user is a member
      if (chan && chan->getMember(handler->user)) {
	 for (Channel::member_map_t::iterator it = chan->members.begin();
	      it != chan->members.end(); it++) {
	    // Send the who line
	    handler->sendWhoReply((*it).second->user, chan, (*it).second);
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
      
      for (Daemon::user_map_t::iterator it = TO_DAEMON->users.begin();
	   it != TO_DAEMON->users.end(); it++) {
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

#ifdef DO_MATCH_COUNTING
	    // Increase the match counter
	    matches++;
#endif
	    
	    // Grab the channel and the member record
	    Channel *chan = 0;
	    ChannelMember *chanmem = 0;
	    
	    // If this user is 'invisible', we should not show the channel...
	    if (!((*it).second->modes & USERMODE_INVISIBLE)) {
	       // Check that they are actually on a channel first
	       if (!(*it).second->channels.empty()) {
		  // Grab the first one on their list (fastest to grab, really)
		  chan = (*(*it).second->channels.begin()).second;
		  
		  // Check that this channel can be made visible
		  if (!User::isOper(handler->user) &&
		      Channel::isHidden(chan)) {
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
	       handler->sendNumeric(ERR_WHOTRUNC,
				    LNG_ERR_WHOTRUNC);
	       break;
	    }
#endif
	 }
      }
   }
   
   // Send the end of help numeric
#ifdef DO_MATCH_COUNTING
   handler->sendNumeric(RPL_ENDOFWHO,
			((matches > 0) ?
			 ((matches == 1) ?
			  String::printf(LNG_RPL_ENDOFWHO,
					 (char const *)maskStr) :
			  String::printf(LNG_RPL_ENDOFWHO_MATCHES,
					 (char const *)maskStr,
					 matches)) :
			 String::printf(LNG_RPL_ENDOFWHO_NOMATCH,
					(char const *)maskStr)));
#else
   handler->sendNumeric(RPL_ENDOFWHO,
			String::printf(LNG_RPL_ENDOFWHO,
				       (char const *)maskStr));
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
      doWHOIS(handler, handler->user, &param1);
   } else {
      StringMask serverMask(param1);
      Server *s = TO_DAEMON->getServer(&serverMask);
      
      // Check
      if (!s) {
	 handler->sendNumeric(ERR_NOSUCHSERVER,
			      param1 + LNG_ERR_NOSUCHSERVER);
	 return;
      }
   
      // Is this US??
      if (s == TO_DAEMON->myServer()) {
	 doWHOIS(handler, handler->user, &param2);
      }
   
      // poll server
   }
}


/* parseWHOWAS
 * Original 09/10/01, Simon Butcher <pickle@austnet.org>
 */
void irc2userHandler::parseWHOWAS(irc2userHandler *handler, StringTokens *tokens)
{
#ifdef DO_MATCH_COUNTING
   int matches = 0;
#endif
   
   // Check we have at least one parameter
   if (tokens->countTokens() < 2) {
      handler->sendNumeric(ERR_NEEDMOREPARAMS,
			   "WHOWAS" LNG_ERR_NEEDMOREPARAMS);
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
      for (Daemon::whowas_deque_t::iterator it = TO_DAEMON->whowas.begin();
	   it != TO_DAEMON->whowas.end(); it++) {
	 // Check for a match
	 if ((*it).getNickname().IRCtoLower() == nick) {
	    // Check the counter
	    if (count-- == 0) {
	       break;
	    }
      
#ifdef DO_MATCH_COUNTING
	    // Increase the match counter
	    matches++;
#endif
   
	    // Send the entry for this iteration
	    handler->sendNumeric(RPL_WHOWASUSER,
				 String::printf("%s %s %s * :%s",
						(const char *)(*it).getNickname(),
						(const char *)(*it).getUsername(),
						(const char *)(*it).getHostname(handler->user),
						(const char *)(*it).getRealname()));
	    handler->sendNumeric(RPL_WHOISSERVER,
				 String::printf("%s %s :%lu",
						(const char *)(*it).getNickname(),
						(const char *)(*it).getServer(handler->user),
						(*it).getSignoffTime()));
	    // If this user was away, send their old away message too (sigh)
	    if ((*it).hasAwayMessage()) {
	       handler->sendNumeric(RPL_AWAY,
				    String::printf(":%s",
						   (const char *)(*it).getAwayMessage()));
	    }
	 }
      }
   }
   
   // Send end of whowas list
#ifdef DO_MATCH_COUNTING
   handler->sendNumeric(RPL_ENDOFWHOWAS,
			((matches > 0) ?
			 ((matches == 1) ?
			  String::printf(LNG_RPL_ENDOFWHOWAS,
					 (char const *)param) :
			  String::printf(LNG_RPL_ENDOFWHOWAS_MATCHES,
					 (char const *)param,
					 matches)) :
			 String::printf(LNG_RPL_ENDOFWHOWAS_NOMATCH,
					(char const *)param)));
#else
   handler->sendNumeric(RPL_ENDOFWHOWAS,
			String::printf(LNG_RPL_ENDOFWHOWAS,
				       (char const *)param));
#endif
}
