/* daemon.h
 * The main class
 * 
 * Copyright (c) 2001,2002 AustHex Development Team
 * (See DEV-TEAM file for details)
 *
 * This file is a part of AustHex.
 * 
 * AustHex is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * AustHex is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with AustHex; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _INCLUDE_DAEMON_H_
# define _INCLUDE_DAEMON_H_

# include <ctime>
# include <sys/time.h>
# include <unistd.h>

# include <map>
# include <list>
# include <deque>

# ifdef STL_HAS_HASH
#  include <hash_map>
# endif

# ifdef STL_HAS_SLIST
#  include <slist>
# endif

# ifdef HAVE_OPENSSL
#  include <openssl/ssl.h>
# endif

class Daemon;


# include "config.h"
# include "user.h"
# include "connection.h"
# include "socket.h"
# include "server.h"
# include "channel.h"
# include "str.h"
# include "operator.h"
# include "whowas.h"


// A 'Listen' record
class Listen {
 public:
   Socket *socket;				// Socket connection
   bool secure;					// Is this a secure connection?
   
   // Minature constructor
   Listen(Socket *sock, bool sec)
     : socket(sock), secure(sec)
     {};
};


// RelationMask class, used to match a mask to a particular reason
class RelationMask {
 public:
   StringMask mask;				// Mask
   String data;					// Data
   
   // Minature constructor
   RelationMask(StringMask m, String const &d)
     : mask(m), data(d)
     {};
};

// The Daemon class
class Daemon {
 public:
   typedef map <String, Operator *> operator_map_t;
# ifdef STL_HAS_HASH
   typedef hash_map <String, Server *> server_map_t;
   typedef hash_map <String, Channel *> channel_map_t;
   typedef hash_map <String, User *> user_map_t;
   typedef hash_map <String, LocalUser *> localuser_map_t;
# else
   typedef map <String, Server *> server_map_t;
   typedef map <String, Channel *> channel_map_t;
   typedef map <String, User *> user_map_t;
   typedef map <String, LocalUser *> localuser_map_t;
# endif
# ifdef STL_HAS_SLIST
   typedef slist <Connection *> connection_list_t;
   typedef slist <Listen *> listen_list_t;
   typedef slist <RelationMask *> relationmask_list_t;
   typedef slist <String> motd_t;
   typedef list <String> motd_reader_t;
# else
   typedef list <Connection *> connection_list_t;
   typedef list <Listen *> listen_list_t;
   typedef list <RelationMask *> relationmask_list_t;
   typedef list <String> motd_t;
# endif
   typedef deque <Whowas> whowas_deque_t;

   enum stages {
      STAGE_INIT,			// Starting up
      STAGE_NORMAL,			// Running normally
      STAGE_SHUTDOWN			// Shutting down
   };
   
 private:
   static Config *config;			// Configuration
   
   static relationmask_list_t failNicknames;	// Invalid nicks
   static relationmask_list_t failChannels;	// Invalid channels

   static relationmask_list_t redirectChannels;	// Redirected channels
   
   static operator_map_t operators;		// IRC Operators
   
   static listen_list_t listens;		// Listening sockets
   static connection_list_t connections;	// Actual connections
   
   static int maxDescriptors;			// Descriptors for select()
   static fd_set inFDSET;			// Input FD_SET
   static fd_set outFDSET;			// Output FD_SET

   static time_t lastGarboRun;			// Last time garbo() was run

   static unsigned long long sentBytes;		// Total bytes sent
   static unsigned long long receivedBytes;	// Total bytes received

   static unsigned long long lastDataMark;	// Last data rx+tx total
   static time_t lastDataMarkTime;		// Last time the mark was done
   static unsigned long dataRate;		// Data rate in kilibits/sec

   static stages stage;				// What stage are we running at

   static struct timeval currentTime;		// Current time
   static String timeZone;			// Local timezone string (TOSI)
   static String timeFlags;			// TOSI flags
   static time_t startTime;			// Time the server started

# ifdef HAVE_OPENSSL
   static SSL_CTX *sslContext;			// SSL context data
# endif
   
   static Server *server;			// Our server record

   
   Daemon(void) {};				// Constructor - cannot be run!

   static void garbo(bool = false);		// Garbage collector

   static void checkClock(void);		// Check the time information
   
   static String 
     onRelationMaskList(relationmask_list_t *,
			String const &);	// Check for a bad channel name

   static String 
     processServerModes(Server *, Handler *, User *, String const &,
			StringTokens *);	// Process a server mode change
   
 public:
   // Servers
   static server_map_t servers;			// Servers list

   // Channels
   static channel_map_t channels;		// Channel list
   static channel_map_t localChannels;		// Local channel list (&)

   // Users
   static user_map_t users;			// User list
   static localuser_map_t localUsers;		// Local users list

   // WHOWAS facility for signed off users
   static whowas_deque_t whowas;		// Old users (whowas) list
   static time_t whowasDecay;			// Max 'age' of whowas entries
   static unsigned short whowasMaxEntries;	// Maximum whowas list entries
   
   static unsigned int numConns;		// Current connections number
   static unsigned int numConnsPeak;		// Peak connection count
   static unsigned int numClientConns;		// Current # client connections
   static unsigned int numClientConnsPeak;	// Peak client connections
   static unsigned int numServerConns;		// Current # server connections
   static unsigned int numServices;		// Number of services online
   static unsigned int numTotalUsers;		// Total # users online
   static unsigned int numTotalUsersPeak;	// Peak number of users online
   static unsigned int numServers;		// Total # servers online
   static unsigned int numOpers;		// Total # of global opers
   static unsigned int numUnknown;		// # of registering connections
   static unsigned int numHelpers;		// Total # of helpers
   
   static motd_t motd;				// The cached MOTD
   
 public:
   ~Daemon(void);				// Class destructor

   static bool init(Config &);			// Initialise the daemon

   // Get the configuration class
   static const Config &getConfig(void)
     { return *config; };
   
   // Get the current time (simple time_t)
   static time_t getTime(void)
     { return (time_t)currentTime.tv_sec; };
   
   // Get the current time (timeval style)
   static long getTimeUsecs(void)
     { return currentTime.tv_usec; };
   
   // Get the local timezone
   static String getTimeZone(void)
     { return timeZone; };

   // Get time on server flags
   static String getTimeFlags(void)
     { return timeFlags; };
   
   // Get the server start time
   static time_t getStartTime(void)
     { return startTime; };

   // Return our server record
   static Server *myServer(void)
     { return server; };
   
   static void rehash(Handler *, User *);	// Rehash the server
   static void restart(Handler *, User *);	// Restart the server

   static String makeISUPPORT(void);		// Make an ISUPPORT string
   
//   static void serverNotice(ServerNotice::servnotice_t,
//			    String const &);	// Broadcast a server notice
   static void broadcastWallops(Server *, 
				String const &);// Broadcast a wallops message
   static void broadcastWallops(User *, String const &);
   static void broadcastWallops(String const &message) 
     { broadcastWallops(server, message); };
   
   static void addInputFD(int);			// Add an input fd
   static void delInputFD(int);			// Delete an input fd
   static void addOutputFD(int);		// Add an output fd
   static void delOutputFD(int);		// Delete an output fd

   static void newConnection(Listen *);		// accept() new connection

   static void addUser(User *);			// Add a user to the user list
   static User *getUser(String const &);	// Find a user
   static void changeUserNick(User *, String const &,
			      time_t = 0);	// Change a users nickname
   static void delUser(User *);			// Delete a user from the list
   static void quitUser(User *, String const &, 
			bool);			// User quitting IRC
   static void killUser(User *, String const &, 
			String const &);	// Kill a user off the network
   static bool 
     addUserSilence(User *, 
		    StringMask const &);	// Add/Broadcast a user silence
   static bool 
     delUserSilence(User *, 
		    StringMask const &);	// Delete a user silence
   static bool addUserAccept(User *, User *);	// Add/Broadcast a user silence
   static bool delUserAccept(User *, User *);	// Delete a user silence
   static void snapshotUser(User *, Whowas::type_t,
			    String const &);	// Snapshot a user for WHOWAS
   
   static void addChannel(Channel *, 
			  String const &);	// Add a channel to a list
   static Channel *getChannel(String const &);	// Find a channel
   static void joinChannel(Channel *, User *);	// User joining a channel
   static void leaveChannel(Channel *, User *);	// Raw user leave routine
   static void partChannel(Channel *, User *, 
			   String const &);	// User parting a channel
   static void 
     kickChannelMember(Channel *, User *, User *,
		       String const &);		// User kicking another user
   static void 
     changeChannelTopic(Channel *, User *,
			String const &);	// Change the channel topic
   static void changeChannelTopic(Channel *, Server *, String const &, 
				  String const &, time_t);
   
   static Operator 
     *getOperator(String const &);		// Grab an operator record

   static void addServer(Server *);		// Add a server to the list
   static void delServer(Server *);		// Remove a server off the list
   static Server *getServer(char);		// Grab a server record
   static Server *getServer(String const &);
   static Server *getServer(StringMask const &);
   static void 
     changeServerMode(Server *, Server *, String const &,
		      StringTokens *);	// Change a server mode
   static void changeServerMode(Server *, Handler *, User *, String const &, 
				StringTokens *);
   
   static Handler *routeTo(Server *);		// Find a route to a server
   static Handler *routeTo(User *);		// Find a route to a user
   
   static void wipeFails(void);			// Wipe fails lists clean
   static void wipeListens(void);		// Wipe all listening sockets
   static void wipeOpers(void);			// Wipe the operators list
   static void wipeRedirects(void);		// Wipe the redirects
   
   // Check if a channel is bad
   static String failedChannel(String const &channel)
     { return onRelationMaskList(&failChannels, channel.IRCtoLower()); };
     
   // Check if a nickname is bad
   static String failedNickname(String const &nickname)
     { return onRelationMaskList(&failNicknames, nickname.IRCtoLower()); };
   
   // Check for channel redirects
   static String redirectedChannel(String const &channel)
     { return onRelationMaskList(&redirectChannels, channel.IRCtoLower()); };

   // Little macro to get to the logger, in a neat way
   static void logger(const String &s, Logger::mask_type m)
     { config->getLoggingLog().log(s, m); };

   static void shutdown(String const & = "");	// Start the shutdown sequence
   static void run(void);			// The main loop

   friend class Handler;
   friend class Connection;
   friend class statsFunctions; // in remotes.cpp
};

#endif

