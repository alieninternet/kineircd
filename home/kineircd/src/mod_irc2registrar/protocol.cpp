/* ./src/register.cpp
 * Handle registration data (before handling a real handler)
 */

#include "config.h"

#include "handler.h"
#include "numerics.h"
#include "language.h"
#include "utils.h"


// Functions table
registerHandler::functionTableStruct registerHandler::functionsTable[] = {
     { "CAPAB",		parseCAPAB },
     { "NICK",		parseNICK },
     { "PASS",		parsePASS },
#ifdef USER_CONNECTION_PINGPONG
     { "PONG",		parsePONG },
#endif
     { "QUIT",		parseQUIT },
     { "SERVER",	parseSERVER },
     { "SQUIT",		parseQUIT }, // an alias, essentially
     { "USER",		parseUSER },
     { 0 }
};


/* registerHandler - Constructor for the register handler sub-class
 * Original 12/08/01, Simon Butcher <pickle@austnet.org>
 */
registerHandler::registerHandler(Connection *c)
: Handler(c),
  password(""),
  nickname(""),
  username(""),
  hostname(c->socket->getRemoteAddressStr()),
  realname(""),
#ifdef USER_CONNECTION_PINGPONG
  pingpong(""),
  gotPong(false),
#endif
  modes(""),
  protocol(0),
  startStamp(0),
  linkStamp(0),
#ifdef MAX_REGISTRATION_LINES
  numLines(0),
#endif
  regmode(IN_PROGRESS)
{
#ifdef DEBUG
   debug("New Handler: registerHandler");
#endif

   // gline, kline checking here
   // dns checking initiation here
   // class determination here
}


/* sendGeneric - Minature generic send routine
 * Original 16/08/01, Simon Butcher <pickle@austnet.org>
 * Note: Double-up? Sort of, this one works a little differently than the
 *       irc2userHandler:: one
 */ 
void registerHandler::sendGeneric(char const *command, String line)
{
   getConnection()->sendRaw(String::printf(":%s %s %s %s\r\n",
				      (char const *)getConnection()->getDaemon()->myServer()->hostname,
				      command,
				      (nickname.length() ?
				       (char const *)nickname :
				       "*"),
				      (char const *)line));
}


/* sendNumeric - Minature sendNumeric routine 'cause of no send handler
 * Original 12/08/01, Simon Butcher <pickle@austnet.org>
 * Note: We do a cheap thing here with the numeric because we know it will
 * 	 be three digits and suffer no 0-prepadding issues.
 */
void registerHandler::sendNumeric(short numeric, User *to, String line)
{
   getConnection()->sendRaw(String::printf(":%s %d %s %s\r\n",
				      (char const *)getConnection()->getDaemon()->myServer()->hostname,
				      numeric,
				      (nickname.length() ?
				       (char const *)nickname :
				       "*"),
				      (char const *)line));
}


/* parseLine - Parse an incoming line
 * Original 12/08/01, Simon Butcher <pickle@austnet.org>
 */
void registerHandler::parseLine(String *line)
{
#ifdef MAX_REGISTRATION_LINES
   // Increase and check the line counter
   if (++numLines > MAX_REGISTRATION_LINES) {
      // Boot them off
# ifdef DEBUG_EXTENDED
      debug("Registration overstepped MAX_REGISTRATION_LINES limit");
# endif
      getConnection()->goodbye();
      return;
   }
#endif
   
   bool found = false;
   StringTokens st(*line);
   String command = st.nextToken().toUpper();
   
   // Run through the list and find a function..
   for (int i = 0; functionsTable[i].command != 0; i++) {
      if (command == functionsTable[i].command) {
	 functionsTable[i].function(this, &st);
	 found = true;
	 break;
      }
   }
   
#ifdef WARN_UNKNOWN_COMMANDS_IN_REGISTRATION
   if (!found) {
# ifdef PASSIVE_REGISTRATION      	 
#  ifdef DEBUG_EXTENDED
      debug("Unknown command encountered during registration");
#  endif
      handler->getConnection()->goodbye();
# else
      sendNumeric(ERR_UNKNOWNCOMMAND, 0, 
		  command + LNG_ERR_UNKNOWNCOMMAND);
# endif
      return;
   }
#endif
   
   /* If we got a command, and we have at least a hostname and a username,
    * we can check if it is time to follow through with a registration
    */
   if (
#ifndef WARN_UNKNOWN_COMMANDS_IN_REGISTRATION
       found &&
#endif
       username.length() && hostname.length()) {
      Handler *newHandler = 0;
      
      switch (regmode) {
       case USER:
	 if (nickname.length() &&
#ifdef USER_CONNECTION_PINGPONG
	     gotPong &&
#endif
	     realname.length()) {
	    // Check if we need to find a password here!!!
	    if (password.length()) {
	       // something here.
#ifndef PASSIVE_REGISTRATION      	 
	       sendNumeric(ERR_PASSWDMISMATCH, 0, LNG_ERR_PASSWDMISMATCH);
#endif
#ifdef DEBUG_EXTENDED
	       debug("Invalid password, terminating!");
#endif

	       // wrong password, bye bye
	       getConnection()->goodbye();
	       return;
	    }
	    
	    // Check if we are not full up on clients at the moment
	    if (getConnection()->socket->getFD() > MAX_FD_NO_MORE_USERS) {
#ifndef PASSIVE_REGISTRATION      	 
	       sendNumeric(ERR_SERVERTOOFULL, 0, LNG_ERR_SERVERTOOFULL);
#endif
#ifdef DEBUG
	       debug("Server is too full, no more clients!");
#endif
	       getConnection()->goodbye();
	       return;
	    }
	    
	    // Create a new user
	    User *user = new User(nickname, username, "", hostname, 
				  generateVWorld(&hostname), realname,
				  getConnection()->getDaemon()->getTime(),
				  getConnection()->getDaemon()->myServer());

	    // Set up the connection name
	    getConnection()->name = &user->nickname;

	    // Create the new handler for this user
	    newHandler = new irc2userHandler(getConnection(), user, modes);
	 }
	 break;
       case SERVER:
	 // We MUST have a password before hitting here.
	 if (password.length()) {
	    // Check the password here.
	    if (false) {
#ifdef DEBUG
	       debug("Invalid password, terminating!");
#endif
	       getConnection()->goodbye();
	       return;
	    }
	    
	    // Try to find the server record
	    Server *server = 0;
	    server = getConnection()->getDaemon()->getServer(&username);
	    
	    // Check if we got it
	    if (!server) {
#ifdef DEBUG_EXTENDED
	       debug("Server not in list; Adding...");
#endif
	       // Create a new server
	       server = new Server(username, realname, 1);
	       
	       // Add this server to the server list
	       getConnection()->getDaemon()->addServer(server);
	    } else {
	       // Check if this server is already connected here
	       if (server->handler) {
#ifdef DEBUG
		  debug(String::printf("Server %s is already connected!",
				       (char const *)username));
#endif
		  getConnection()->goodbye();
	       }
#ifdef DEBUG
	       else {
		  debug("Server already in list, fixing local handler...");
	       }
#endif
	    }
	    
	    // Okay, try to find a direct link to the handler they need
	    switch (protocol) {
	     case 13: // P13
	       // Create the new handler for this user
	       newHandler = new p13serverHandler(getConnection(), server,
						 startStamp, linkStamp);
	       break;
	     default: // Unknown protocol, terminate connection
#ifdef DEBUG
	       debug(String::printf("Unsupported protocol (%d), terminating!",
				    protocol));
#endif
	       getConnection()->goodbye();
	       return;
	    }
	    
	    // Set up the connection name
	    getConnection()->name = &server->hostname;

	    // Fix server handler pointer
	    server->handler = newHandler;
	 } else {
	    // Dodgey connection, no password...
#ifdef DEBUG_EXTENDED
	    debug("No password, terminating!");
#endif
	    getConnection()->goodbye();
	    return;
	 }
	 break;
       default:
	 // Nothing to do here.
	 return;
      }
      
      // If we got here, we should cross over to the new handler, if we can
      if (newHandler) {
	 // Decrease the unknown connection counter
	 getConnection()->getDaemon()->numUnknown--;
	 
	 // Copy the new handler across
	 getConnection()->handler = newHandler;
	 
	 // Bye bye!
	 delete this;
      }
   }
}


/* parseCAPAB - Requested capabilities
 * Original 16/08/01, Simon Butcher <pickle@austnet.org>
 * Note: Capabilities are separated by a space, capability options are
 *       broken up by a comma, similar to most IRC command paramters
 */
void registerHandler::parseCAPAB(registerHandler *handler, StringTokens *tokens)
{
   // Make sure we were given at least one parameter
   if (!(tokens->countTokens() >= 2)) {
#ifdef PASSIVE_REGISTRATION      	 
      handler->getConnection()->goodbye();
#else
      handler->sendNumeric(ERR_NEEDMOREPARAMS, 0,
			   "CAPAB" LNG_ERR_NEEDMOREPARAMS);
#endif
      return;
   }

   /* Run through the requested capabilities list and look for stuff we can
    * recognise
    */
   for (String ability = tokens->nextToken(); ability.length();
	ability = tokens->nextToken()) {
#ifdef DEBUG_EXTENDED
      debug(String::printf(" -=>   Ability: %s",
			   (char const *)ability));
#endif
   }
}


/* parseNICK
 * Original 12/08/01, Simon Butcher <pickle@austnet.org>
 */
void registerHandler::parseNICK(registerHandler *handler, StringTokens *tokens)
{
   // Rip the nick out, ignoring anything after a space
   String nick = tokens->nextToken();

   // Check we got a nickname from that..
   if (!nick.length()) {
#ifdef PASSIVE_REGISTRATION      	 
      handler->getConnection()->goodbye();
#else
      handler->sendNumeric(ERR_NONICKNAMEGIVEN, 0, LNG_ERR_NONICKNAMEGIVEN);
#endif
      return;
   }
   
   // Firstly, make sure the nickname is within acceptable limits (size/chars)
   if (!okNickname(&nick)) {
#ifdef PASSIVE_REGISTRATION      	 
      handler->getConnection()->goodbye();
#else
      handler->sendNumeric(ERR_ERRONEUSNICKNAME, 0,
			   nick + LNG_ERR_ERRONEUSNICKNAME);
#endif
      return;
   }
 
   // Check for nicks that are not allowed here (from config)
   String reason = TO_DAEMON->failedNickname(&nick);
   if (reason.length()) {
#ifdef PASSIVE_REGISTRATION      	 
      handler->getConnection()->goodbye();
#else
      handler->sendNumeric(ERR_ERRONEUSNICKNAME, 0,
			   String::printf(LNG_ERR_ERRONEUSNICKNAME_W_REASON,
					  (char const *)nick,
					  (char const *)reason));
      return;
#endif
   }
   
   // Check that the nickname is not already in use
   if (TO_DAEMON->getUser(&nick)) {
#ifdef PASSIVE_REGISTRATION      	 
      handler->getConnection()->goodbye();
#else
      handler->sendNumeric(ERR_NICKNAMEINUSE, 0,
			   nick + LNG_ERR_NICKNAMEINUSE);
#endif
      return;
   }
   
   // If we got here, the nick was ok - allow it
   handler->nickname = nick;
#ifdef DEBUG_EXTENDED
   debug(String::printf(" -=>       Nick: %s",
			(char const *)nick));
#endif
   
#ifdef USER_CONNECTION_PINGPONG
   // Make up some dodgey random stuff with xor!
   handler->pingpong = 
     String::printf("%08lX",
		    ((unsigned int)TO_DAEMON->getTime() ^
		     getRandom(0xFFFFFFFE)));

   // Send a PING out to make sure we are dealing with a 'real' client
# ifndef PASSIVE_REGISTRATION      	 
   handler->sendGeneric("NOTICE",
			String::printf(LNG_PINGPONG_NOTICE,
				       (char const *)handler->pingpong, 
				       (char const *)handler->pingpong,
				       (char const *)TO_DAEMON->myServer()->hostname));
# endif
   handler->getConnection()->sendRaw(String("PING :") + handler->pingpong + "\r\n");
#endif
}


/* parsePASS
 * Original 31/08/01, Simon Butcher <pickle@austnet.org>
 */
void registerHandler::parsePASS(registerHandler *handler, StringTokens *tokens)
{
#ifdef STRICT_REGISTRATIONS
   // Have we already got the password?!
   if (handler->password.length()) {
      // Drop this connection..
      handler->getConnection()->goodbye();
      return;
   }
#endif
   
   // Grab the password from the line (we do not care if it is blank)
   handler->password = tokens->nextColonToken();
   
#ifdef DEBUG_EXTENDED
   debug(String::printf(" -=>   Password: %s",
			(char const *)handler->password));
#endif
}


#ifdef USER_CONNECTION_PINGPONG
/* parsePONG
 * Original 16/08/01, Simon Butcher <pickle@austnet.org>
 */
void registerHandler::parsePONG(registerHandler *handler, StringTokens *tokens)
{
   // Grab the pong reply
   String pongpong = tokens->nextColonToken();
   
   // Make sure this pong is valid
   if (pongpong == handler->pingpong) {
      handler->gotPong = true;
   }
}
#endif


/* parseQUIT
 * Original 12/08/01, Simon Butcher <pickle@austnet.org>
 */
void registerHandler::parseQUIT(registerHandler *handler, StringTokens *tokens)
{
   // Close the connection. No goodbye or anything
   handler->getConnection()->goodbye();
}


/* parseSERVER
 * Original 12/08/01, Simon Butcher <pickle@austnet.org>
 */
void registerHandler::parseSERVER(registerHandler *handler, StringTokens *tokens)
{
#ifdef STRICT_REGISTRATIONS
   // Make sure this connection has not already been given a registration mode
   if (handler->regmode > IN_PROGRESS) {
# ifdef PASSIVE_REGISTRATION      	 
      handler->getConnection()->goodbye();
# else
      handler->sendNumeric(ERR_ALREADYREGISTERED, 0,
			   LNG_ERR_ALREADYREGISTERED);
      handler->getConnection()->goodbye();
# endif
      return;
   }
#endif
   
   // Check there are enough tokens
   if (!(tokens->countTokens() >= 10)) {
#ifdef PASSIVE_REGISTRATION      	 
      handler->getConnection()->goodbye();
#else
      handler->sendNumeric(ERR_NEEDMOREPARAMS, 0,
			   "SERVER" LNG_ERR_NEEDMOREPARAMS);
#endif
      return;
   }

   // Grab the variables
   handler->username = tokens->nextToken();
   int hops = tokens->nextToken().toInt();
   handler->startStamp = tokens->nextToken().toLong();
   handler->linkStamp = tokens->nextToken().toLong();
   handler->protocol = tokens->nextToken().subString(1).toInt();
   handler->realname = tokens->nextColonToken().subString(0, 
							  MAXLEN_SERVERDESC);
   
#ifdef DEBUG_EXTENDED
   // Send what we got to the debugging output
   debug(String::printf(" -=>     Server: %s",
			(char const *)handler->username));
   debug(String::printf(" -=>       Hops: %d",
			hops));
   debug(String::printf(" -=> startStamp: %lu",
			handler->startStamp));
   debug(String::printf(" -=>  linkStamp: %lu (My time is %lu)",
			handler->linkStamp,
			TO_DAEMON->getTime()));
   debug(String::printf(" -=>   Protocol: %d",
			handler->protocol));
   debug(String::printf(" -=>       Name: %s",
			(char const *)handler->realname));
#endif
   
   // Check the integers variables are ok
   if ((hops != 1) ||
       (handler->startStamp == 0) ||
       (handler->linkStamp == 0)) {
#ifdef DEBUG_EXTENDED
      debug("Variables are invalid!");
#endif
      handler->getConnection()->goodbye();
      return;
   }
   
   // Set the registration mode
   handler->regmode = SERVER;
}


/* parseUSER
 * Original 12/08/01, Simon Butcher <pickle@austnet.org>
 */
void registerHandler::parseUSER(registerHandler *handler, StringTokens *tokens)
{
#ifdef STRICT_REGISTRATIONS
   // Make sure this connection has not already been given a registration mode
   if (handler->regmode > IN_PROGRESS) {
# ifdef PASSIVE_REGISTRATION      	 
      handler->getConnection()->goodbye();
# else
      handler->sendNumeric(ERR_ALREADYREGISTERED, 0,
			   LNG_ERR_ALREADYREGISTERED);
      handler->getConnection()->goodbye();
# endif
      return;
   }
#endif
   
   // Check there are enough tokens
   if (!(tokens->countTokens() >= 5)) {
#ifdef PASSIVE_REGISTRATION      	 
      handler->getConnection()->goodbye();
#else
      handler->sendNumeric(ERR_NEEDMOREPARAMS, 0,
			   "USER" LNG_ERR_NEEDMOREPARAMS);
#endif
      return;
   }
   
   // Rip the command apart - the new RFCs do not do things this way..
   handler->username = tokens->nextToken();
   handler->modes = tokens->nextToken();
   (void)tokens->nextToken(); // We ignore this one.
   handler->realname = tokens->nextColonToken().subString(0, MAXLEN_REALNAME);

#ifdef DEBUG_EXTENDED
   // Output what we got for debugging purposes
   debug(String::printf(" -=>       User: %s",
			(char const *)handler->username));
   debug(String::printf(" -=>      Modes: %s",
			(char const *)handler->modes));
   debug(String::printf(" -=>   Realname: %s",
			(char const *)handler->realname));
#endif
   
   // Set the registration mode
   handler->regmode = USER;
}
