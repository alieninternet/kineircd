/* daemon.cpp
 * The main class (IRC daemon) constructors and destructors
 */

#include "config.h"

#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#ifdef HAVE_OPENSSL
# include <openssl/ssl.h>
#endif

#include "daemon.h"
#include "version.h"
#include "language.h"
#include "numerics.h"


// Variables
namespace Daemon {
   String configFile = DEFAULT_CONFIG_FILE;
   
   String adminName = DEFAULT_CONFIG_ADMIN_NAME;
   String adminEmail = DEFAULT_CONFIG_ADMIN_EMAIL;
   String adminLocation = DEFAULT_CONFIG_ADMIN_LOCATION;
   
   unsigned short confMaxAcceptsPerUser = DEFAULT_MAX_ACCEPTS_PER_USER;
   unsigned short confMaxBansPerChannel = DEFAULT_MAX_BANS_PER_CHANNEL;
   unsigned char confMaxLangsPerUser = DEFAULT_MAX_LANGS_PER_USER;
   unsigned short confMaxSilencesPerUser = DEFAULT_MAX_SILENCES_PER_USER;
   unsigned short confMaxWatchesPerUser = DEFAULT_MAX_WATCHES_PER_USER;
   
   Daemon::relationmask_list_t failNicknames;
   Daemon::relationmask_list_t failChannels;
   
   Daemon::relationmask_list_t redirectChannels;
   
   Daemon::operator_map_t operators;
   
   Daemon::listen_list_t listens;
   Daemon::connection_list_t connections;
   
   int maxDescriptors = 0;
   fd_set inFDSET;
   fd_set outFDSET;
   
   time_t lastGarboRun = 0;
   unsigned long long sentBytes = 0;
   unsigned long long receivedBytes = 0;
   
   unsigned long long lastDataMark = 0;
   time_t lastDataMarkTime = 0;
   unsigned long dataRate = 0;
   
   Daemon::stages stage = Daemon::STAGE_INIT;
   
   String networkName = "";
   
   struct timeval currentTime;
   String timeZone = "+0000"; // Assume GMT to start with..
   String timeFlags = "";
   time_t startTime = 0;
   
#ifdef HAVE_OPENSSL
   SSL_CTX *sslContext = 0;
#endif
   
   Server *server = 0;
   
   Daemon::server_map_t servers;
   
   Daemon::channel_map_t channels;
   Daemon::channel_map_t localChannels;
   
   Daemon::user_map_t users;
   Daemon::localuser_map_t localUsers;
   
   Daemon::whowas_deque_t whowas;
   time_t whowasDecay = DEFAULT_CONFIG_WHOWAS_DECAY;
   unsigned short whowasMaxEntries = DEFAULT_CONFIG_WHOWAS_MAX_ENTRIES;
   
   unsigned int numConns = 0;
   unsigned int numConnsPeak = 0;
   unsigned int numClientConns = 0;
   unsigned int numClientConnsPeak = 0;
   unsigned int numServerConns = 0;
   unsigned int numServices = 0;
   unsigned int numTotalUsers = 0;
   unsigned int numTotalUsersPeak = 0;
   unsigned int numServers = 0;
   unsigned int numOpers = 0;
   unsigned int numUnknown = 0;
   unsigned int numHelpers = 0;
   
   Daemon::motd_t motd;
};


/* ~Daemon - Deinit the server
 * Original 11/08/01, Simon Butcher <pickle@austnet.org>
 */
Daemon::~Daemon(void)
{
#ifdef DEBUG
   debug("Shutting down server");
#endif

#ifdef HAVE_OPENSSL
   // Clean up the SSL stuff, cautiously
   if (sslContext) {
      SSL_CTX_free(sslContext);
   }
#endif
   
   // Wipe up stuff
   wipeFails();
   wipeListens();
   wipeOpers();

   // Run through any connections that might be lingering
   Connection *conn;
   while (!connections.empty()) {
      conn = *connections.begin();
      connections.erase(connections.begin());
      delete conn;
   }

   // Clean out the channel list since we are going down
   Channel *chan;
   while (!channels.empty()) {
      chan = (*channels.begin()).second;
      channels.erase(channels.begin());
      delete chan;
   }
   while (!localChannels.empty()) {
      chan = (*localChannels.begin()).second;
      localChannels.erase(localChannels.begin());
      delete chan;
   }
   
   // Clean out the user list
   User *u;
   while (!users.empty()) {
      u = (*users.begin()).second;
      users.erase(users.begin());
      delete u;
   }

   // .. and the local user list
   LocalUser *lu;
   while (!localUsers.empty()) {
      lu = (*localUsers.begin()).second;
      localUsers.erase(localUsers.begin());
      delete lu;
   }
   
   // Clean up the whowas list
   whowas.clear();

#ifdef LOG_TO_SYSLOG
   // Kill the syslog link
   closelog();
#endif
   
   // Other stuff to delete
   delete server;
}


/* init - Init the server
 * Original 11/08/01, Simon Butcher <pickle@austnet.org>
 */
bool Daemon::init(String const &conf)
{
#ifdef DEBUG
   cerr << "Initialising new server" << endl;
#endif

   // Reset lists
#ifdef DEBUG_EXTENDED
   cerr << "Resetting lists" << endl;
#endif
   failNicknames.clear();
   failChannels.clear();
   redirectChannels.clear();
   operators.clear();
   listens.clear();
   connections.clear();
   FD_ZERO(&inFDSET);
   FD_ZERO(&outFDSET);
   servers.clear();
   channels.clear();
   localChannels.clear();
   users.clear();
   localUsers.clear();
   whowas.clear();
   motd.clear();

   
   // Reset variables
#ifdef DEBUG_EXTENDED
   cerr << "Setting up variables" << endl;
#endif
   configFile = conf;
   gettimeofday(&currentTime, NULL);
   startTime = getTime();
   server = new Server();
   
   /* Seed the random number thingy.. this is kinda dodgey :( */
   srand((unsigned int)getTime());
   
#ifdef LOG_TO_SYSLOG
   /* Open up logging session with syslog - The reason why I chose syslog
    * is that syslog is an easy to read format, easy to log to, and there are
    * heaps of utilities out there to help format, strip, grep syslogs etc.
    * Plus it also allows system administrators to centralise their logging,
    * or do whatever the hell they want. With syslog.conf you can always
    * control ircd messages to go to a separate file, so it's the same thing
    * in the end really.
    */
# ifdef DEBUG_EXTENDED
   cerr << "Calling syslog's openlog()" << endl;
# endif
   openlog(SYSLOG_IDENT, LOG_PID, LOG_DAEMON);
   // log out first line
# ifdef DEBUG
   logger(String::printf("Starting %s (debugging)", Version::version),
	  LOGPRI_INFO);
# else
   logger(String::printf("Starting %s", Version::version),
	  LOGPRI_INFO);
# endif
#endif
   
#ifdef HAVE_OPENSSL
   // Initialise the SSL stuff
   SSL_load_error_strings();
   SSL_library_init();
# ifdef MUST_INIT_PRNG
   // Init the bloody PRNG here
#  error "Oops, cannot init the PRNG, refusing to compile a potentially insecure ircd"
# endif
   
   // Fire up the method, here we allow both SSLv2 and SSLv 3, plus TLSv1
   sslContext = SSL_CTX_new(SSLv23_method());
   
   // Check.
   if (!sslContext) {
      String reason = "Warning: SSL will not be operational: ";
      // get openssl error?
      logger(&reason, LOGPRI_WARNING);
# ifndef DEBUG
      cout << reason << endl;
# endif
      
      // For our sanity, just in case.
      sslContext = 0;
      
						      exit(0);
   }
#endif

   // Fire up the extended clock information
   checkClock();
   
   // Load config!
   if (!configure(true)) {
      logger("IRCd not started: Configuration file error", 
	     LOGPRI_ERROR);
      return false;
   }

   /* Make sure we have actually bound to SOMETHING and are listening,
    * else we aren't going to be very useful at all
    */
   if (listens.empty()) {
      logger("IRCd not started: No listening sockets - Nobody can connect!", 
	     LOGPRI_ERROR);
      return false;
   }

   // Add ourselves to the server list which should be completely blank
   servers[server->hostname.toLower()] = server;

   // Last message, we are obviously going to run here. Tell the caller
   cout << "Running..." << endl;
   
   // Close some descriptors we will not be using
   close(0); // stdin
#ifndef DEBUG
   close(1); // stdout
   close(2); // stderr
#endif
   
   // We are ready to go, go into normal running stage
   stage = STAGE_NORMAL;
   return true;
}


/* logger - [Various forms] Log a line to whatever logging facility we use
 * Original 11/08/01, Simon Butcher <pickle@austnet.org>
 * Note: Should this routine be in another file perhaps?
 */
void Daemon::logger(String const &line, int priority)
{
   /* Notice how we do not log if we are in debug mode. This is because I
    * wasn't too happy about my syslog getting filled with junk while I was
    * starting, stopping, starting, stopping etc etc..
    */
#ifndef DEBUG
# ifdef LOG_TO_SYSLOG
   if (priority >= 0) {
      syslog(priority, "%s", (char const *)line);
   }
# endif
#else
   // Pipe to stderr if we are debugging
   clog << "logger() <- [" << priority << "] " << line << endl;
#endif
   
   // Broadcast it to +s people
}


/* garbo - Look for 'rubbish' to 'clean up' :)
 * Original 17/08/01, Simon Butcher <pickle@austnet.org>
 * Note: the goto's are more efficient
 */
void Daemon::garbo(bool called)
{
#ifdef MAX_GARBO_RUN_ITEMS
   int items = 0;
#endif
   
   // Mark down the garbo run time
   lastGarboRun = getTime();
   
check_connections:
   /* Run through the connections first, they have the highest priority
    * since ppl cannot get onto the network if there aren't any descriptors
    * left, and pings also have a fairly high priority to keep control over
    * timeouts.
    */
   Connection *conn;
   for (connection_list_t::iterator it = connections.begin();
	it != connections.end(); it++) {
#ifdef MAX_GARBO_RUN_ITEMS
      // One loop is worth an 'item'; check the items count
      if (called && 
	  (items > MAX_GARBO_RUN_ITEMS)) {
	 return;
      }
#endif
      
      // Check the status flag and delete if this connection is dead
      if ((!((*it)->status & CONFLAG_CONNECTED) &&
	   (*it)->outQueue.empty()) || 
	  (!(*it)->socket->isConnected())) {
#ifdef MAX_GARBO_RUN_ITEMS
	 items++;
#endif
	 
	 // Rip it out and delete it
	 conn = *it;
	 connections.erase(it);
	 delete conn;
	 
	 // The sequence is broken, we need to start again
	 goto check_connections;
      }
      
      // Are we waiting for a ping to be replied to on this connection?
      if ((*it)->status & CONFLAG_PING_SENT) {
	 // Check if the ping time-out has been reached
	    
      } else {
	 // Check if it is time to send a ping out
	    
      }
   }

   // Low priority, check the whowas list for old items to 'retire'
   Whowas *ww;
   while (!whowas.empty()) {
      ww = &whowas.front();

      // Check the timestamp on this item
      if ((getTime() - ww->getSignoffTime()) < whowasDecay) {
	 /* If we got here, the rest of the queue will also be young enough
	  * to keep within the list, so we can safely skip the rest of the
	  * list (remember: it IS a queue)
	  */
	 break;
      }
      
      // Otherwise, delete it
      whowas.pop_front();
      delete ww;
   }
   
   /* If we are being called, return a message saying we were called and how
    * many items we counted
    */
   if (called) {
#ifdef MAX_GARBO_RUN_ITEMS
      serverNotice(LocalUser::SN_HOUSEKEEPING,
		   String::printf((char *)Language::L_SERVNOTICE_GARBO_ITEMS,
				  items));
#else
      serverNotice(LocalUser::SN_HOUSEKEEPING, 
		   Language::L_SERVNOTICE_GARBO);
#endif
   }
}


/* checkClock - Check the extended clock information (for RPL_TIMEONSERVERIS)
 * Original 25/10/01, Simon Butcher <pickle@austnet.org>
 */
void Daemon::checkClock(void)
{
#ifdef HAVE_TZSET_AND_FRIENDS
   // For the timezone information to be initialised (or re-initialised)
   tzset();
   
   // Grab the timezone string (yes, this is kind of 'inversed')
   timeZone = String::printf("%c%02u%02u",
			     ((timezone > 0) ? '-' : '+'),
			     (unsigned short)(timezone / -3600),
			     (unsigned short)((timezone % -3600) / 60));

   // Work out the time flags...
   timeFlags = "";
   
   // If we are in day light savings mode, toggle on the DST flag
   if (daylight) {
      timeFlags = "d"; // first flag, we do not need to add it
   }
   
   // Problems occured during time collection?
//   if (false) {
//      timeFlags = timeFlags + "e";
//   }
#endif
}


/* rehash - Rehash the server
 * Original 19/09/01, Simon Butcher <pickle@austnet.org>
 */
void Daemon::rehash(Handler *handler, User *user)
{
   // If we got a user, we can assume this was called by someone
   if (user) {
      // Tell the user, if there is a handler we are rehashing
      if (handler) {
	 handler->sendNumeric(server, RPL_REHASHING, user,
			      configFile +
			      Language::L_RPL_REHASHING);
      }
      
      // Send out a server broadcast notifying of the rehash
      serverNotice(LocalUser::SN_HOUSEKEEPING,
		   String::printf((char *)Language::L_SERVNOTICE_CMD_REHASH,
				  ((char const *)
				   user->nickname)));
   } else {
      // Send out a server broadcast notifying of the rehash
      serverNotice(LocalUser::SN_HOUSEKEEPING,
		   String::printf((char *)Language::L_SERVNOTICE_CMD_REHASH,
				  "Signal SIGHUP"));
   }

   // Check the extended clock information (hey, you never know!)
   checkClock();
   
   // Clean things up
   garbo(true);
   
   // Reload the configuration file(s)
   configure();
}


/* restart - Rehash the server
 * Original , Simon Butcher <pickle@austnet.org>
 */
void Daemon::restart(Handler *handler, User *user)
{
}


/* makeISUPPORT - Generate an ISUPPORT line using current variables
 * Original 24/10/01, Simon Butcher <pickle@austnet.org>
 */
String Daemon::makeISUPPORT(void)
{
   return String::printf("NICKLEN=%d"				// (~11chrs)
			 " TOPICLEN=%d"				// (~13chrs)
			 " KICKLEN=%d"				// (~12chrs)
			 "%s"					// NETWORK= (~30chrs)
//			 " WHOX"				// (5chrs)
//			 " WALLCHOPS"				// (10chrs)
//			 " USERIP"				// (7chrs)
			 " LANGUAGE=%u,%s"			// (12chrs)
			 " KNOCK"				// (6chrs)
			 " WATCH=%u"				// (~10chrs)
			 " SILENCE=%u"				// (~11chrs)
			 " ACCEPT=%u"				// (~12chrs)
			 " CHANTYPES=%s"			// (~15chrs)
			 " MAXCHANNELS=%d" 			// (~16chrs)
			 " MAXBANS=%u"				// (~11chrs)
			 " CHARSET=rfc1459" 			// (16chrs)
			 " MODES=%u"				// (~9chrs)
			 " PREFIX=%s",				// (~20chrs)
			 	// (=~226chrs + tag)
			 MAXLEN_NICKNAME,
			 MAXLEN_TOPIC,
			 MAXLEN_KICK_REASON,
			 (haveNetworkName() ?
			  ((char const *)
			   (String(" NETWORK=") + getNetworkName())) :
			  ""),
			 confMaxLangsPerUser,
			 (char const *)Language::getISUPPORTcodes(),
			 confMaxWatchesPerUser,
			 confMaxSilencesPerUser,
			 confMaxAcceptsPerUser,
			 CHANNEL_TYPES,
			 MAX_CHANNELS_PER_USER,
			 confMaxBansPerChannel,
			 MAX_MODES_PER_COMMAND,
			 Channel::prefixStr);
}


/* serverNotice - [Various Forms] Broadcast a server notice to +s's
 * Original 18/09/01, Simon Butcher <pickle@austnet.org>
 */
void Daemon::serverNotice(LocalUser::servnotice_t type, String const &message)
{
   // Run through the local user list
   for (localuser_map_t::iterator it = localUsers.begin();
	it != localUsers.end(); it++) {
#ifdef DEBUG_EXTENDED
      // If we are debugging do a little sanity check. This should NEVER happen
      if (!(*it).second->user || !(*it).second->user->local) {
	 debug(String::printf("SANITY CHECK FAILED! User %s discovered in "
			      "Daemon::localUsers and is missing a local "
			      "record!",
			      (char const *)(*it).first));
	 continue;
      }
#endif
      
      // Check if this user is to receive server notices
      if ((*it).second->serverNotices & type) {
	 // Send the message via the notice interface
	 (*it).second->handler->sendNotice(server, (*it).second->user, 
					   message);
      }
   }
}


/* broadcastWallops - [Various Forms] Broadcast a WALLOPS to local +w's
 * Original 18/09/01, Simon Butcher <pickle@austnet.org>
 */
void Daemon::broadcastWallops(Server *from, String const &message)
{
#ifdef DEBUG_EXTENDED
   debug(String::printf("broadcastWallops() (%s) %s",
			(char const *)from->hostname,
			(char const *)message));
#endif
   
   // Run through the local user list
   for (localuser_map_t::iterator it = localUsers.begin();
	it != localUsers.end(); it++) {
#ifdef DEBUG_EXTENDED
      // If we are debugging do a little sanity check. This should NEVER happen
      if (!(*it).second->user || !(*it).second->user->local) {
	 debug(String::printf("SANITY CHECK FAILED! User %s discovered in "
			      "Daemon::localUsers and is missing a local "
			      "record!",
			      (char const *)(*it).first));
	 continue;
      }
#endif
      
      // Check if this user is to receive server notices
      if ((*it).second->user->modes & User::M_WALLOPS) {
	 // Send the message via the notice interface
	 (*it).second->handler->sendWallops(from, message);
      }
   }
}

void Daemon::broadcastWallops(User *from, String const &message)
{
#ifdef DEBUG_EXTENDED
   debug(String::printf("broadcastWallops() (%s) %s",
			(char const *)from->nickname,
			(char const *)message));
#endif
   
   // Run through the local user list
   for (localuser_map_t::iterator it = localUsers.begin();
	it != localUsers.end(); it++) {
#ifdef DEBUG_EXTENDED
      // If we are debugging do a little sanity check. This should NEVER happen
      if (!(*it).second->user || !(*it).second->user->local) {
	 debug(String::printf("SANITY CHECK FAILED! User %s discovered in "
			      "Daemon::localUsers and is missing a local "
			      "record!",
			      (char const *)(*it).first));
	 continue;
      }
#endif
      
      // Check if this user is to receive server notices
      if ((*it).second->user->modes & User::M_WALLOPS) {
	 // Send the message via the notice interface
	 (*it).second->handler->sendWallops(from, message);
      }
   }
}


/* addInputFD - Add a file descriptor to the input set
 * Original 11/08/01, Simon Butcher <pickle@austnet.org>
 */
void Daemon::addInputFD(int fd)
{
   FD_SET(fd, &inFDSET);
   
   if (fd >= maxDescriptors) {
      maxDescriptors = fd + 1;
#ifdef DEBUG_EXTENDED
      debug(String::printf("FD_SET input: maxDescriptors risen to %d",
			   maxDescriptors));
#endif
   }
}


/* delInputFD - Delete a file descriptor to the input set
 * Original 11/08/01, Simon Butcher <pickle@austnet.org>
 */
void Daemon::delInputFD(int fd)
{
   FD_CLR(fd, &inFDSET);
}


/* addOutputFD - Add a file descriptor to the output set
 * Original 11/08/01, Simon Butcher <pickle@austnet.org>
 */
void Daemon::addOutputFD(int fd)
{
   FD_SET(fd, &outFDSET);
}


/* delOutputFD - Delete a file descriptor to the output set
 * Original 11/08/01, Simon Butcher <pickle@austnet.org>
 */
void Daemon::delOutputFD(int fd)
{
   FD_CLR(fd, &outFDSET);
}


/* newConnection - Fire up a new connection from one of the listening sockets
 * Original 11/08/01, Simon Butcher <pickle@austnet.org>
 */
void Daemon::newConnection(Listen *l)
{
   // Accept the new connection
   Socket *newsock = l->socket->accept(l->secure);
   
   // Fall over if the pointer is broken or is not really connected
   if (!newsock || !newsock->isConnected()) {
#ifdef DEBUG
      debug(String::printf("Failed accept: %d (%s)",
			   l->socket->getFD(),
			   strerror(errno)));
#endif
      
      if (newsock) {
	 delete newsock;
      }
      
      return;
   }
   
#ifdef DEBUG
   debug(String::printf("Accepting from: fd %d (%s:%d) New: fd %d (%s:%d)",
			l->socket->getFD(),
			(char const *)newsock->getLocalAddressStr(),
			newsock->getLocalPort(),
			newsock->getFD(),
			(char const *)newsock->getRemoteAddressStr(),
			newsock->getRemotePort()));
#endif

   // Force this socket to be non blocking, dammit
   if (!newsock->setNonBlocking()) {
#ifdef DEBUG
      debug(String::printf("[%d] setNonBlocking() failed, aborting this "
			   "connection(%s) :(",
			   newsock->getFD(),
			   strerror(errno)));
#endif
      delete newsock;
      return;
   }
   

   // Make a new connection and tell the socket about it
   Connection *conn = new Connection(newsock, l->secure);
   newsock->connection = conn;
   
   // Add the connection to the list, and the FD SET for select()
   connections.push_front(conn);
}


/* addUser - Add a user to the user map, and broadcast the addition to the net
 * Original 12/08/01, Simon Butcher <pickle@austnet.org>
 */
void Daemon::addUser(User *user)
{
#ifdef DEBUG_EXTENDED
   debug(String::printf("addUser() <- %s (%s)",
			(char const *)user->nickname,
			(user->local ?
			 "local" : "remote")));
#endif
   
   String fixedNick = user->nickname.IRCtoLower();
   
   // Broadcast the change
   
   
   // Increment the user count on the server this user is on
   user->server->numUsers++;

   // Increment the global user count
   numTotalUsers++;
   if (numTotalUsers > numTotalUsersPeak) {
      numTotalUsersPeak = numTotalUsers;
   }
   
   // Add the user, lowercasing the nickname for easier searching later on
   users[fixedNick] = user;
   
   // If the user is local, add them to the local list too
   if (user->local) {
      localUsers[fixedNick] = user->local;
   }
   
   /* CPU hogging time.. Run through the local users list and check each
    * user's watch list to see if this nick is on it..
    */
   for (localuser_map_t::iterator it = localUsers.begin();
	it != localUsers.end(); it++) {
#ifdef DEBUG_EXTENDED
      if (!(*it).second->user || !(*it).second->user->local) {
	 debug(String::printf("SANITY CHECK FAILED! User %s discovered in "
			      "Daemon::localUsers and is missing a local "
			      "record!",
			      (char const *)(*it).first));
	 continue;
      }
#endif
      
      if ((*it).second->watching(fixedNick)) {
	 (*it).second->handler->sendWatchOn(user);
      }
   }
}


/* getUser - Get a user record from the users list, if we can
 * Original 14/08/01, Simon Butcher <pickle@austnet.org>
 */
User *Daemon::getUser(String const &nick)
{
   // Look for this nick
   String n = nick.IRCtoLower();
   User *u = users[n];
   
   // Make sure we got this user
   if (u) {
      return u;
   };
   
   // Else we gotta delete this empty record we just made and return nothing
   users.erase(n);

   return NULL;
}


/* changeUserNick - Change a user's nickname (may also mean replacing records)
 * Original 24/08/01, Simon Butcher <pickle@austnet.org>
 */
void Daemon::changeUserNick(User *user, String const &newnick, 
			    time_t changeTime)
{
#ifdef DEBUG_EXTENDED
   debug(String::printf("changeUserNick() <- %s -> %s",
			(char const *)user->nickname,
			(char const *)newnick));
#endif
   
   String on = user->nickname.IRCtoLower();
   String nn = newnick.IRCtoLower();

   // Fix the last changed nick time for this user
   if (changeTime) {
      user->lastNickChange = changeTime;
   } else {
      user->lastNickChange = getTime();
   }
   
   // Check if we are just doing a simple nick replacement
   if (nn == on) {
      // Replace the nickname
      user->nickname = newnick;
   } else {
      // Remove the user from the userlist, and add them again
      users.erase(on);
      users[nn] = user;
      
      // Check if this user is also on the local list
      if (user->local) {
	 localUsers.erase(on);
	 localUsers[nn] = user->local;
      }
   }

   map <String, bool> messages;
   messages.clear();
   ChannelMember *self = 0;

   /* Check locally connected users for if they were watching the old 
    * nickname, and if they are watching the new one too.
    */
   for (localuser_map_t::iterator it = localUsers.begin();
	it != localUsers.end(); it++) {
#ifdef DEBUG_EXTENDED
      if (!(*it).second->user || !(*it).second->user->local) {
	 debug(String::printf("SANITY CHECK FAILED! User %s discovered in "
			      "Daemon::localUsers and is missing a local "
			      "record!",
			      (char const *)(*it).first));
	 continue;
      }
#endif
      
      // Check the old nickname
      if ((*it).second->watching(on)) {
	 (*it).second->handler->sendWatchOff(user);
      }
      
      // Check the new nickname
      if ((*it).second->watching(nn)) {
	 (*it).second->handler->sendWatchOn(user, newnick);
      }
   }

   // Run through the channels this user is on
   for (User::channel_map_t::iterator it = user->channels.begin();
	it != user->channels.end(); it++) {
      self = 0;

      // Run through the list of users in this channel
      for (Channel::member_map_t::iterator it2 = (*it).second->members.begin();
	   it2 != (*it).second->members.end(); it2++) {
	 // Is this the user changing the nick or someone else?
	 if ((*it2).second->getUser() != user) {
	    // If we have not already done so, send the change to this use
	    if (!messages[(*it2).second->getUser()->nickname]) {
	       // Mark this user down as being touched upon
	       messages[(*it2).second->getUser()->nickname] = true;
	       
	       // Check if the user is local
	       if ((*it2).second->getUser()->local) {
		  // Send it
		  (*it2).second->getUser()->local->handler->
		    sendNickChange(user, newnick);
	       }
	    }
	 } else {
	    // Mark this record down as this user
	    self = (*it2).second;
	    
	    // Remove this from the done list
	    messages.erase(user->nickname);
	 }
      }
      
      // Sanity check!
      if (self) {
	 // Remove this member from the channel member list and add them again
	 (*it).second->members.erase(on);
	 (*it).second->members[nn] = self;
      }
   }
   
   // Broadcast the change to servers
   
   
   // Replace the nickname in the user record
   user->nickname = newnick;
}


/* delUser - Delete a user from the user list
 * Original 16/08/01, Simon Butcher <pickle@austnet.org>
 */
void Daemon::delUser(User *user)
{
#ifdef DEBUG_EXTENDED
   debug(String::printf("delUser() <- %s",
			(char const *)user->nickname));
#endif

   String fixedNick = user->nickname.IRCtoLower();
   
   /* Run through the channels this user was on, and make sure they are no
    * longer on any channels else things might get ugly
    */
   Channel *chan;
   while (!user->channels.empty()) {
      // Grab the channel
      chan = (*user->channels.begin()).second;
      
      // Leave the channel
      leaveChannel(chan, user);
   }

   // Decrement the user count on the server this user is using
   user->server->numUsers--;
   
   // Increment the global user count
   numTotalUsers--;

   // Remove this user from the userlist and delete their entry completely
   users.erase(fixedNick);

   // Check if this user is also on the local list
   if (user->local) {
      LocalUser *lu = localUsers[fixedNick];
      localUsers.erase(fixedNick);
      delete lu;
   }

   // If this user was also a global operator, we need to decrement the counter
   if (user->modes & User::M_GLOBALOPER) {
      numOpers--;
   }
   
   /* Check for people who are watching this nickname, they might want to know
    * that the user has gone offline.. just a thought :)
    */
   for (localuser_map_t::iterator it = localUsers.begin();
	it != localUsers.end(); it++) {
#ifdef DEBUG_EXTENDED
      if (!(*it).second->user || !(*it).second->user->local) {
	 debug(String::printf("SANITY CHECK FAILED! User %s discovered in "
			      "Daemon::localUsers and is missing a local "
			      "record!",
			      (char const *)(*it).first));
	 continue;
      }
#endif
      
      // Check the old nickname
      if ((*it).second->watching(user->nickname)) {
	 (*it).second->handler->sendWatchOff(user);
      }
   }

   delete user;
}


/* quitUser - Delete a user from the user list
 * Original 16/08/01, Simon Butcher <pickle@austnet.org>
 */
void Daemon::quitUser(User *user, String const &reason, bool broadcast)
{
   /* We use a map here because it becomes faster to add things without
    * having to check for repeats (the STL does all this).. and besides
    * I'm a little lazy :)
    */
   map <String, bool> messages;
   messages.clear();
   
   // Leave all of the channels this user is on and send quit messages
   Channel *chan;
   while (!user->channels.empty()) {
      // Grab the channel
      chan = (*user->channels.begin()).second;
      
      // Formulate a list of users who will also want to know about this quit
      for (Channel::member_map_t::iterator it = chan->members.begin();
	   it != chan->members.end(); it++) {
	 // If this user is themselves, ignore this
	 if ((*it).second->getUser() == user) {
	    continue;
	 }
	 
	 // Check if this user is local (easy!)
	 if ((*it).second->getUser()->local) {
	    // If we have not already done so, send a message to this user
	    if (!messages[(*it).second->getUser()->nickname]) {
	       // Send a quit if the channel is not marked anonymous
	       if (!(chan->modes & Channel::M_ANONYMOUS)) {
		  // Mark this user down as being touched upon
		  messages[(*it).second->getUser()->nickname] = true;

		  (*it).second->getUser()->local->handler->sendQuit(user, 
								    reason);
		  continue;
	       } 
	       
	       /* Anonymous channels get a part, since the users are
		* obscured totally by the flag and only joins/parts are
		* allowed in the channel. Note that the user has not been
		* added to the list as this is a part, quits are more
		* permanant and this is just one of the quirks associated
		* with the anonymous channel flag -- you cannot be TOTALLY
		* anonymous if the people you're in the anonymous channel
		* with are in other non-anonymous channels too since they
		* are capable of seeing a REAL quit message in some situations
		*/
	       (*it).second->getUser()->local->handler->sendPart(chan, user, 0);
	    }
	    continue;
	 }
      }
      
      // Leave this channel
      leaveChannel(chan, user);
   }
   
   
   // Broadcast the change to other servers here
//   if (broadcast) {
      // stuff.
//   }
   
   // Delete the user from the list
   delUser(user);
}


/* killUser - Kill a user off the network
 * Original 22/09/01, Simon Butcher <pickle@austnet.org>
 */
void Daemon::killUser(User *user, String const &caller, 
		      String const &reason)
{
   // Same as the quit, use a map
   map <String, bool> messages;
   messages.clear();
   
   // Fix up the reason
   String newReason = String::printf((char *)Language::L_QUIT_KILLED,
				     (char const *)caller,
				     (char const *)reason);
   
   // If this was one of ours, we need to do the physical kill
   if (user->local) {
      // Get the handler connection to clean up the mess
      user->local->handler->kill(caller, reason);
   }
   
   // Just quit them, minus the broadcast. We will do that
   quitUser(user, newReason, false);
   
   // Broadcast the kill to other servers
}


/* addUserSilence - Add a mask to a user's silence list and broadcast it
 * Original 24/09/01, Simon Butcher <pickle@austnet.org>
 */
bool Daemon::addUserSilence(User *user, StringMask const &mask)
{
   // Make sure this is not already in the list (eg. a waste of time)
   for (User::silence_list_t::iterator it = user->silences.begin();
	it != user->silences.end(); it++) {
      // Is this the mask we are adding?
      if (mask == *it) {
	 return false;
      }
   }
   
   // If we got here it is not on the list already, just add it
   user->silences.push_front(mask);
   
   // Broadcast the change to servers
	 
   return true;
}


/* delUserSilence - Delete a mask from a user's silence list and broadcast it
 * Original 24/09/01, Simon Butcher <pickle@austnet.org>
 */
bool Daemon::delUserSilence(User *user, StringMask const &mask)
{
   // Run through the list to find the mask to delete
   for (User::silence_list_t::iterator it = user->silences.begin();
	it != user->silences.end(); it++) {
      // Is this the mask we are deleting?
      if (mask == *it) {
	 user->silences.erase(it);
	 
	 // Broadcast the change to servers
	 
	 return true;
      }
   }

   return false;
}


/* addUserAccept - Add a mask to a user's ACCEPT list and broadcast it
 * Original 23/10/01, Simon Butcher <pickle@austnet.org>
 */
bool Daemon::addUserAccept(User *user, User *target)
{
   // Check that the target is not already on the list
   if (user->isAccepting(target)) {
      return false;
   }
   
   // Add the target to the list
   user->addAccept(target);
   
   // Broadcast the change
   
   return true;
}


/* delUserAccept - Delete a mask from a user's ACCEPT list and broadcast it
 * Original 23/10/01, Simon Butcher <pickle@austnet.org>
 */
bool Daemon::delUserAccept(User *user, User *target)
{
   // Check that the target is on the list
   if (user->isAccepting(target)) {
      // Remove the target
      user->delAccept(target);
      
      // Broadcast the change
      
      return true;
   }
   
   return false;
}


/* snapshotUser - Take a 'snapshot' of a user for the whowas list
 * Original 08/10/01, Simon Butcher <pickle@austnet.org>
 */
void Daemon::snapshotUser(User *u, Whowas::type_t type, String const &details)
{
   // Check the length of the whowas list, we may need to kill the oldest entry
   if (whowas.size() >= whowasMaxEntries) {
      whowas.pop_front();
   }
   
   // Ok, add this entry!
}


/* addChannel - Add a channel to the appropriate channel list
 * Original 15/08/01, Simon Butcher <pickle@austnet.org>
 */
void Daemon::addChannel(Channel *chan, String const &creator)
{
#ifdef DEBUG_EXTENDED
   debug(String::printf("addChannel() <- %s",
			(char const *)chan->name));
#endif
   
   String loweredName = chan->name.IRCtoLower();
   
   // Add the channel with a lowered name for easier searching later on
   if (chan->name[0] == '&') {
      localChannels[loweredName] = chan;
   } else {
      channels[loweredName] = chan;
   }

   /* Run through the local users list to check if someone is weird enough to
    * have this channel on their watch list :)
    */
   for (localuser_map_t::iterator it = localUsers.begin();
	it != localUsers.end(); it++) {
#ifdef DEBUG_EXTENDED
      if (!(*it).second->user || !(*it).second->user->local) {
	 debug(String::printf("SANITY CHECK FAILED! User %s discovered in "
			      "Daemon::localUsers and is missing a local "
			      "record!",
			      (char const *)(*it).first));
	 continue;
      }
#endif
      
      if ((*it).second->watching(loweredName)) {
	 (*it).second->handler->sendWatchOn(chan, creator);
      }
   }
}


/* getChannel - Grab a channel record from the appropriate channel list
 * Original 15/08/01, Simon Butcher <pickle@austnet.org>
 */
Channel *Daemon::getChannel(String const &channel)
{
   // Look for this channel
   String chan = channel.IRCtoLower();
   Channel *c = 0;
   
   // Check which channel list we need to look at
   if (chan[0] == '&') {
      c = localChannels[chan];
   } else {
      c = channels[chan];
   }
   
   // Make sure we got this channel
   if (c) {
      return c;
   }

   // Else we gotta delete this empty record we just made and return nothing
   if (chan[0] == '&') {
      localChannels.erase(chan);
   } else {
      channels.erase(chan);
   }
   
   return NULL;
}


/* joinChannel - Add someone to the channel user list and the user channel list
 * Original 15/08/01, Simon Butcher <pickle@austnet.org>
 * Note: Does not check if the user is already in the channel!!
 */
void Daemon::joinChannel(Channel *c, User *u)
{
#ifdef DEBUG_EXTENDED
   debug(String::printf("joinChannel() <- %s, %s",
			(char const *)c->name,
			(char const *)u->nickname));
#endif

   // Add the user to the channel user list
   c->members[u->nickname.IRCtoLower()] = new ChannelMember(u);
   
   // Add the channel to the user channel list
   u->channels[c->name.IRCtoLower()] = c;

   // Broadcast the join to anyone locally connected
   for (Channel::member_map_t::iterator it = c->members.begin();
	it != c->members.end(); it++) {
      if ((*it).second->getUser()->local) {
	 (*it).second->getUser()->local->handler->sendJoin(c, u);
      }
   }
   
   // Broadcast the join to the network
   
}


/* leaveChannel - User leaving a channel, may also include channel destruction
 * Original 15/08/01, Simon Butcher <pickle@austnet.org>
 * Note: This shouldn't be called by a user handling routine.
 */
void Daemon::leaveChannel(Channel *c, User *u)
{
#ifdef DEBUG_EXTENDED
   debug(String::printf("leaveChannel() <- %s, %s",
			(char const *)c->name,
			(char const *)u->nickname));
#endif
   
   String loweredName = c->name.IRCtoLower();
   String loweredNick = u->nickname.IRCtoLower();

   // Delete the channel from the user channel list
   u->channels.erase(loweredName);
   
   // Delete the user from the channel user list, and kill the member entry
   ChannelMember *cm = c->members[loweredNick];
   c->members.erase(loweredNick);
   delete cm;
   
   // If the channel member list is empty, we must delete this channel
   if (c->members.empty()) {
      // Work out which list to delete this channel from
      if (c->name[0] == '&') {
	 localChannels.erase(loweredName);
      } else {
	 channels.erase(loweredName);
      }
      
      /* Run through the local users list to check if anyone needs to know
       * this channel is going bye byes now
       */
      for (localuser_map_t::iterator it = localUsers.begin();
	   it != localUsers.end(); it++) {
#ifdef DEBUG_EXTENDED
	 if (!(*it).second->user || !(*it).second->user->local) {
	    debug(String::printf("SANITY CHECK FAILED! User %s discovered in "
				 "Daemon::localUsers and is missing a local "
				 "record!",
				 (char const *)(*it).first));
	    continue;
	 }
#endif
	 
	 if ((*it).second->watching(loweredName)) {
	    (*it).second->handler->sendWatchOff(c);
	 }
      }
      
      delete c;
   }
}


/* partChannel - User leaving a channel via PART
 * Original 15/08/01, Simon Butcher <pickle@austnet.org>
 */
void Daemon::partChannel(Channel *c, User *u, String const &reason)
{
   // Broadcast the join to anyone locally connected
   for (Channel::member_map_t::iterator it = c->members.begin();
	it != c->members.end(); it++) {
      if ((*it).second->getUser()->local) {
	 (*it).second->getUser()->local->handler->sendPart(c, u, reason);
      }
   }
   
   // Broadcast the join to the network
   
   // Physically do the leave
   leaveChannel(c, u);
}


/* kickChannelMember - User kicking another user out of a channel
 * Original 15/09/01, Simon Butcher <pickle@austnet.org>
 */
void Daemon::kickChannelMember(Channel *c, User *kicker, User *kickee,
			       String const &reason)
{
   // Trim this if we need to
   String newReason = reason.subString(0, MAXLEN_KICK_REASON);
   
   // Broadcast the kick to anyone locally connected
   for (Channel::member_map_t::iterator it = c->members.begin();
	it != c->members.end(); it++) {
      if ((*it).second->getUser()->local) {
	 (*it).second->getUser()->local->handler->sendKick(c, kicker, kickee, 
							   newReason);
      }
   }
   
   // Broadcast the kick to the network
   
   // Physically do the leave
   leaveChannel(c, kickee);
}


/* changeChannelTopic - [Various Forms] User/Server changing a channel topic
 * Original 19/09/01, Simon Butcher <pickle@austnet.org>
 */
void Daemon::changeChannelTopic(Channel *c, User *from, String const &topic)
{
#ifdef DEBUG_EXTENDED
   debug(String::printf("changeChannelTopic() <- %s, %s (%s)",
			(char const *)c->name,
			(char const *)from->nickname,
			(char const *)topic));
#endif

   // Trim this if we need to
   String newTopic = topic.subString(0, MAXLEN_TOPIC);
   
   // Check if this topic is already set (dumb to reset it)
   if (newTopic == c->topic) {
      return;
   }
   
   // Set the new topic stuff
   c->topic = newTopic;
   c->topicWho = from->nickname;
   c->topicTime = getTime();
      
   // Run through the list of channel members and send them the new topic
   for (Channel::member_map_t::iterator it = c->members.begin();
	it != c->members.end(); it++) {
      if ((*it).second->getUser()->local) {
	 (*it).second->getUser()->local->handler->sendTopic(c, from, newTopic);
      }
   }
   
   // broadcast.
}

void Daemon::changeChannelTopic(Channel *c, Server *from, 
				String const &topic, String const &setter,
				time_t settime)
{
#ifdef DEBUG_EXTENDED
   debug(String::printf("changeChannelTopic() <- %s, %s (%s)",
			(char const *)c->name,
			(char const *)from->hostname,
			(char const *)topic));
#endif

   // Trim this if we need to
   String newTopic = topic.subString(0, MAXLEN_TOPIC);
   
   // Set the new topic stuff
   c->topic = newTopic;
   c->topicWho = setter;
   c->topicTime = settime;
   
   // Run through the list of channel members and send them the new topic
   for (Channel::member_map_t::iterator it = c->members.begin();
	it != c->members.end(); it++) {
      if ((*it).second->getUser()->local) {
	 (*it).second->getUser()->local->handler->sendTopic(c, from, newTopic);
      }
   }
   
   // broadcast.
}


/* getOperator - Grab an operator record from the operators list
 * Original 08/09/01, Simon Butcher <pickle@austnet.org>
 */
Operator *Daemon::getOperator(String const &nickname)
{
   // Fix up the nickname string
   String nick = nickname.IRCtoLower();
   Operator *oper = 0;
   
   // Grab the operator if we can
   oper = operators[nick];
   
   // Make sure we got this record
   if (oper) {
      return oper;
   }

   // Else we gotta delete this empty record we just made and return nothing
   operators.erase(nick);
   
   return NULL;
}


/* addServer - Add a server to the servers map
 * Original 10/09/01, Simon Butcher <pickle@austnet.org>
 */
void Daemon::addServer(Server *server)
{
#ifdef DEBUG_EXTENDED
   debug(String::printf("addServer() <- %s",
			(char const *)server->hostname));
#endif
   
   String fixedName = server->hostname.toLower();
   
   // Broadcast the change

   
   
   // Add the server, fixing the name for easier searching later on
   servers[fixedName] = server;
   
   // Increase the server counter
   numServers++;

   /* Run through the local users list and check if anyone is looking for
    * this server, as long as they are operators/helpers
    */
   for (localuser_map_t::iterator it = localUsers.begin();
	it != localUsers.end(); it++) {
#ifdef DEBUG_EXTENDED
      if (!(*it).second->user || !(*it).second->user->local) {
	 debug(String::printf("SANITY CHECK FAILED! User %s discovered in "
			      "Daemon::localUsers and is missing a local "
			      "record!",
			      (char const *)(*it).first));
	 continue;
      }
#endif

      // Check if this user is not an operator or a helper
      if (!((*it).second->user->modes & User::M_GLOBALOPER) &&
	  !((*it).second->user->modes & User::M_LOCALOPER) &&
	  !((*it).second->user->modes & User::M_HELPER)) {
	 continue;
      }
      
      if ((*it).second->watching(fixedName)) {
	 (*it).second->handler->sendWatchOn(server);
      }
   }
}

/* delServer - Remove a server from the servers map
 * Original 01/11/01, Simon Butcher <pickle@austnet.org>
 */
void Daemon::delServer(Server *server)
{
#ifdef DEBUG_EXTENDED
   debug(String::printf("delServer() <- %s",
			(char const *)server->hostname));
#endif

   // Remove it from the map
   servers.erase(server->hostname.toLower());

   // And finally, delete the server data itself
   delete server;
}


/* getServer - [Various Forms] Grab a record from the servers list if we can
 * Original 10/09/01, Simon Butcher <pickle@austnet.org>
 */
Server *Daemon::getServer(char magicChar)
{
//   // Run through the list of servers to find this server
//   for (server_map_t::iterator it = servers.begin();
//	it != servers.end(); it++) {
//      // Check for a match
//      if ((*it).second->hostname == servname) {
//	 return (*it).second;
//      }
//   }
   
   return NULL;
}

Server *Daemon::getServer(String const &hostname)
{
   // Fix up the hostname
   String servname = hostname.toLower();
   Server *s = 0;
   
   // Try to grab the server record pointer from the map
   s = servers[servname];

   // Make sure we got this record
   if (s) {
      return s;
   }

   // Else we gotta delete this empty record we just made and return nothing
   servers.erase(servname);
   
   return NULL;
}

Server *Daemon::getServer(StringMask const &hostmask)
{
   // Run through the list of servers to find this server
   for (server_map_t::iterator it = servers.begin();
	it != servers.end(); it++) {
      // Check for a match
      if (hostmask.matches((*it).second->hostname)) {
	 return (*it).second;
      }
   }
   
   return NULL;
}


/* processServerModes - Support routine for the change server modes below
 * Original 21/09/01, Simon Butcher <pickle@austnet.org>
 */
String Daemon::processServerModes(Server *server, Handler *handler, 
				  String const &modes, 
				  StringTokens *tokens)
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
	 bool gotModeChar = false;
	 numModes++;
	 
	 // Run through the server mode list
	 for (int ii = 0; Server::modeTable[ii].letter != 0; ii++) {
	    // Check for a match
	    if (Server::modeTable[ii].letter == modes[i]) {
	       // If this mode needs a parameter, grab the next token
	       if ((toggle && Server::modeTable[ii].hasParamOn) ||
		   (!toggle && Server::modeTable[ii].hasParamOff)) {
		  param = tokens->nextToken();
	       } else {
		  // Reset the parameter string
		  param = "";
	       }
	       
	       // Check if this can be toggled
	       if (Server::modeTable[ii].operToggle) {
		  // Run the toggler for this mode, checking if it worked
		  if (Server::modeTable[ii].toggler(toggle, server, &param)) {
		     // It worked, check which toggle string toa dd this to
		     if (toggle) {
			toggleOnStr = toggleOnStr + String(modes[i]);
			
			// If this mode had a parameter, add it to the list too
			if (param.length()) {
			   toggleParamsOn = toggleParamsOn + " " + param;
			}
		     } else {
			toggleOffStr = toggleOffStr + String(modes[i]);
			
			// If this mode had a parameter, add it to the list too
			if (param.length()) {
			   toggleParamsOff = toggleParamsOff + " " + param;
			}
		     }
		  }
	       } else {
		  // Complain to the user that they cannot change this mode
		  if (handler) {
		     handler->sendNumeric(server,
					  ERR_CANNOTCHANGESERVERMODE, 0,
					  String::printf((char *)Language::L_ERR_CANNOTCHANGESERVERMODE,
							 modes[i]));
		  }
	       }
	       
	       gotModeChar = true;
	    }
	 }
	 
	 // Check if we found a valid char. If not,c omplain if we can
	 if (!gotModeChar && handler) {
	    handler->sendNumeric(server, ERR_UNKNOWNSERVERMODE, 0,
				 String::printf((char *)Language::L_ERR_UNKNOWNSERVERMODE,
						modes[i]));

	 }
      }
   }

   // Assembler the mode change string
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


/* changeServerMode - Broadcast and change a server mode modification
 * Original 21/09/01, Simon Butcher <pickle@austnet.org>
 */
void Daemon::changeServerMode(Server *server, Server *from, 
			      String const &modes, StringTokens *tokens)
{
   String modeStr = processServerModes(server, 0, modes, tokens);

   // Check we actually have something here
   if (!modeStr.length()) {
      return;
   }
   
   // Broadcast the mode change to any opers locally connected
   for (localuser_map_t::iterator it = localUsers.begin();
	it != localUsers.end(); it++) {
#ifdef DEBUG_EXTENDED
      if (!(*it).second->user || !(*it).second->user->local) {
	 debug(String::printf("SANITY CHECK FAILED! User %s discovered in "
			      "Daemon::localUsers and is missing a local "
			      "record!",
			      (char const *)(*it).first));
	 continue;
      }
#endif
      
      // Check if this user is to receive this mode change
      if (((*it).second->user->modes & User::M_GLOBALOPER) ||
	  ((*it).second->user->modes & User::M_LOCALOPER)) {
	 // Send the mode change
	 (*it).second->handler->sendServerMode(server, from, modeStr);
      }
   }
   
   // broadcast to servers
}

void Daemon::changeServerMode(Server *server, Handler *handler, User *from, 
			      String const &modes, StringTokens *tokens)
{
   String modeStr = processServerModes(server, handler, modes, tokens);
   
   // Check we actually have something here
   if (!modeStr.length()) {
      return;
   }
   
   // Broadcast the mode change to any opers locally connected
   for (localuser_map_t::iterator it = localUsers.begin();
	it != localUsers.end(); it++) {
#ifdef DEBUG_EXTENDED
      if (!(*it).second->user || !(*it).second->user->local) {
	 debug(String::printf("SANITY CHECK FAILED! User %s discovered in "
			      "Daemon::localUsers and is missing a local "
			      "record!",
			      (char const *)(*it).first));
	 continue;
      }
#endif
      
      // Check if this user is to receive this mode change
      if (((*it).second->user->modes & User::M_GLOBALOPER) ||
	  ((*it).second->user->modes & User::M_LOCALOPER)) {
	 // Send the mode change
	 (*it).second->handler->sendServerMode(server, from, modeStr);
      }
   }

   // broadcast to servers
}
   

/* routeTo - [Various Forms] Find a route to a particular object
 * Original 06/09/01, Simon Butcher <pickle@austnet.org>
 */
Handler *Daemon::routeTo(Server *server)
{
   // Check the obvious - is this server locally connected
   if (server->handler) {
      return server->handler;
   }
   
   // eek?!
   return NULL;
}

Handler *Daemon::routeTo(User *user)
{
   // Check the obvious - is this user locally connected
   if (user->local) {
      return user->local->handler;
   }
   
   // Otherwise route via the server the user is connected to
   return routeTo(user->server);
}


/* wipeFails - Clean the fail lists
 * Original 06/09/01, Simon Butcher <pickle@austnet.org>
 */
void Daemon::wipeFails(void)
{
   RelationMask *rm;

   while (!failNicknames.empty()) {
      rm = *failNicknames.begin();
      failNicknames.erase(failNicknames.begin());
      delete rm; 
   }

   while (!failChannels.empty()) {
      rm = *failChannels.begin();
      failChannels.erase(failChannels.begin());
      delete rm;
   }
}


/* wipeListens - Shutdown all of our listening sockets
 * Original 18/08/01, Simon Butcher <pickle@austnet.org>
 */
void Daemon::wipeListens(void)
{
   Listen *l;
   
   while (!listens.empty()) {
      l = *listens.begin();
      listens.erase(listens.begin());
      delInputFD(l->socket->getFD());
      delete l; // will close() for us
   }
}


/* wipeOpers - Clean out the operators list
 * Original 08/09/01, Simon Butcher <pickle@austnet.org>
 */
void Daemon::wipeOpers(void)
{
   Operator *oper;
   
   while (!operators.empty()) {
      oper = (*operators.begin()).second;
      operators.erase(operators.begin());
      delete oper;
   }
}


/* wipeRedirects - Clean the redirects out
 * Original 13/09/01, Simon Butcher <pickle@austnet.org>
 */
void Daemon::wipeRedirects(void)
{
   RelationMask *rm;

   while (!redirectChannels.empty()) {
      rm = *redirectChannels.begin();
      redirectChannels.erase(redirectChannels.begin());
      delete rm; 
   }
}


/* onRelationMaskList - Check if something is on the given relation list
 * Original 06/09/01, Simon Butcher <pickle@austnet.org>
 */
String Daemon::onRelationMaskList(relationmask_list_t *rmList, 
				  String const &item)
{
   // Run through the list and check the masks
   for (relationmask_list_t::iterator it = rmList->begin();
	it != rmList->end(); it++) {
      // Check for a match
      if ((*it)->mask.matches(item)) {
	 // Return the reason
	 return (*it)->data;
      }
   }
   
   // All is well, return nothing
   return "";
}


/* shutdown - Start the shutdown cycle
 * Original 11/08/01, Simon Butcher <pickle@austnet.org>
 */
void Daemon::shutdown(String const &reason)
{
#ifdef DEBUG
   debug(String("shutdown() <- ") + reason);
#endif

   // Make up a nice message
   String message = (reason.length() ?
		     (String("Shutting down (") + reason + String(")")) :
		     String("Shutting down"));
   
   // Log our nice message
   logger(message, LOGPRI_INFO);

   // Kill any ports we are listening on - we are no longer accepting ppl
   wipeListens();
   
   /* Send our nice message to all our connections, and remove their 
    * socket's file descriptor from the input list. We don't wanna hear what
    * they have to say anymore.
    */
   for (connection_list_t::iterator it = connections.begin();
	it != connections.end(); it++) {
      if (!((*it)->status & CONFLAG_CONNECTED)) {
	 (*it)->goodbye(message);
      }
   }
   
   stage = STAGE_SHUTDOWN;
}


/* run - The main loop
 * Original 11/08/01, Simon Butcher <pickle@austnet.org>
 * Note: Unfortuantely not a very nice looking routine..
 */
#define CheckInput(x)	FD_ISSET(x, &inFDtemp)
#define CheckOutput(x)	FD_ISSET(x, &outFDtemp)
void Daemon::run(void)
{
   /* Make sure the init was all happy, else there isn't much point us
    * going beyond this point really
    */
   if (stage != STAGE_NORMAL) {
      return;
   }
   
   struct timeval timeout;
   bool running = true;
   int selectOut;
   fd_set inFDtemp, outFDtemp;
   
#ifdef DEBUG
   debug("Entering main loop");
#endif

   while (running) {
      // Set the timeout..
      timeout.tv_sec = SELECT_TIMEOUT;
      timeout.tv_usec = 0;

      // Copy the FD sets over, just in case we get an error
      inFDtemp = inFDSET;
      outFDtemp = outFDSET;
      
      // Poll select()
      selectOut = select(maxDescriptors, &inFDtemp, &outFDtemp, 
			 SELECT_TYPE_ARG234 NULL, &timeout);
      
      // Update the clock
      gettimeofday(&currentTime, NULL);
      
      // Now, what did select say?
      switch (selectOut) {
       case -1: // Select aborted (caused by a signal, usually)
#ifdef DEBUG_EXTENDED
	 perror("run() - select()");
#endif
	 break;
       case 0: // Select timed out, no descriptor change
	 garbo();
	 break;
       default:
	 if (stage >= STAGE_NORMAL) {
	    // Check for a new connection
	    for (listen_list_t::iterator it = listens.begin();
		 it != listens.end(); it++) {
	       // Check if there is a new connection we should be aware of
	       if (CheckInput((*it)->socket->getFD())) {
		  newConnection(*it);
	       }
	    }
	    
	    // Check for activity on connections
	    for (connection_list_t::iterator it = connections.begin();
		 it != connections.end(); it++) {
#ifdef DEBUG_EXTENDED
	       if (!(*it) || !(*it)->socket) {
		  debug("broken?!");
		  continue;
	       }
#endif
	       
	       // Check for input waiting
	       if (CheckInput((*it)->socket->getFD())) {
		  (*it)->handleInput();
	       }
	       
	       // Check for OK to send and something in the queue
	       if (CheckOutput((*it)->socket->getFD())) {
		  (*it)->sendQueue();
	       }
	    }

	 }
	 
	 // Check if we are desparate for a garbage run
	 if ((getTime() - lastGarboRun) >= MAX_GARBO_DELAY_TIME) {
	    garbo();
	 }
      }
      
      // Check if we are in shutdown mode
      if (stage == STAGE_SHUTDOWN) {
	 unsigned int totalQueued = 0;
	 
	 // Talley up the total queue length
	 for (connection_list_t::iterator it = connections.begin();
	      it != connections.end(); it++) {
	    totalQueued += (*it)->outQueue.size();
	 }

	 /* If there is nothing left in the output queue we can safely
	  * die now
	  */
	 if (!totalQueued) {
#ifdef DEBUG
	    debug("Output queues exhausted; running = false");
#endif
	    running = false;
	 }
      }
   }
}
