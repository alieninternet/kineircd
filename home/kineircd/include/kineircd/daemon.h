/* daemon.h
 * The main class
 */

#ifndef __DAEMON_H_
# define __DAEMON_H_

# include <time.h>
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

/* These are 'server notice' types, used to determine what is sent to users.
 * Do not change these once they are set! A protocol may depend on them!!
 */
// Grr, should this typedef be shoved in the LocalUser class or kept global?!
typedef unsigned long snotice_bitmask_t;
# define SERVERNOTICE_OPER		0x00000001 // Oper/deop
# define SERVERNOTICE_NETWORK		0x00000002 // Connect/Squit/TBSs etc
# define SERVERNOTICE_KILL		0x00000004 // Kill notifications
# define SERVERNOTICE_HELPME		0x00000008 // HELPME requests
# define SERVERNOTICE_GLOBOPS		0x00000010 // GLOBOPS messages
# define SERVERNOTICE_LOCOPS		0x00000020 // LOCOPS messages
# define SERVERNOTICE_HOUSEKEEPING	0x00000040 // Config/Rehash/Garbo etc
# define SERVERNOTICE_HTM		0x00000080 // High-Traffic mode notices
# define SERVERNOTICE_SYNC_TS		0x00000100 // Timestamp sync obscurities
# define SERVERNOTICE_SYNC_NOTARGET	0x00000200 // Target sync obscurities
# define SERVERNOTICE_SYNC_GENERAL	0x00000400 // General out of sync
# define SERVERNOTICE_DEBUG		0x00000800 // Debugging information


// This is for the RPL_RIMEONSERVERIS reply. Only one byte, as you can see
typedef unsigned char TYPE_RPL_TIMEONSERVERIS_FLAGS;
# define TIMEONSERVERFLAG_DST		0x01	// Daylight Saving Time
# define TIMEONSERVERFLAG_BADCLOCKZONE	0x02	// Bad clock or zone (inaccuracy)


// Phew, finally onto our local header files!
# include "user.h"
# include "connection.h"
# include "socket.h"
# include "server.h"
# include "channel.h"
# include "str.h"
# include "operator.h"
# include "whowas.h"


// If we are in debugging mode, we need this routine!
# ifdef DEBUG
extern void debug(String const &);		// Output/Broadcast debug line
# endif


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

class ConfigData;

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
   
 private:
   String const configFile;			// Main configuration file
   
   String adminName;				// Administrator name
   String adminEmail;				// Administrator e-mail
   String adminLocation;			// Admin location (optional)
   
   relationmask_list_t failNicknames;		// Invalid nicks
   relationmask_list_t failChannels;		// Invalid channels

   relationmask_list_t redirectChannels;	// Redirected channels
   
   operator_map_t operators;			// IRC Operators
   
   listen_list_t listens;			// Listening sockets
   connection_list_t connections;		// Actual connections
   
   int maxDescriptors;				// Descriptors for select()
   fd_set inFDSET;				// Input FD_SET
   fd_set outFDSET;				// Output FD_SET

   time_t lastGarboRun;				// Last time garbo() was run

   unsigned long long sentBytes;		// Total bytes sent
   unsigned long long receivedBytes;		// Total bytes received

   unsigned long long lastDataMark;		// Last data rx+tx total
   time_t lastDataMarkTime;			// Last time the mark was done
   unsigned long dataRate;			// Data rate in kilibits/sec

   enum {
      INIT,				// Starting up
      NORMAL,				// Running normally
      SHUTDOWN				// Shutting down
   } stage;					// What stage are we running at

   String networkName;				// Name of 'network'
   
   struct timeval currentTime;			// Current time
   const time_t startTime;			// Time the server started

# ifdef HAVE_OPENSSL
//   SSL_METHOD *sslMethod;			// SSL method data
   SSL_CTX *sslContext;				// SSL context data
# endif
   
   Server *server;				// Our server record

   void configComplain(bool, String const &);	// Configuration complaints
   void configWarning(bool, String const &);	// Configuration warnings
   bool configCopy(bool, ConfigData *);		// Copy configuration data over
   
   void garbo(bool = false);			// Garbage collector
   
   String onRelationMaskList(relationmask_list_t *,
			     String const &);	// Check for a bad channel name

   String processServerModes(Server *, Handler *, String const &,
			     StringTokens *);	// Process a server mode change
   
 public:
   // Servers
   server_map_t servers;			// Servers list

   // Channels
   channel_map_t channels;			// Channel list
   channel_map_t localChannels;			// Local channel list (&)

   // Users
   user_map_t users;				// User list
   localuser_map_t localUsers;			// Local users list

   // WHOWAS facility for signed off users
   whowas_deque_t whowas;			// Old users (whowas) list
   time_t whowasDecay;				// Max 'age' of whowas entries
   unsigned int whowasMaxEntries;		// Maximum whowas list entries
   
   unsigned int numConns;			// Current connections number
   unsigned int numConnsPeak;			// Peak connection count
   unsigned int numClientConns;			// Current # client connections
   unsigned int numClientConnsPeak;		// Peak client connections
   unsigned int numServerConns;			// Current # server connections
   unsigned int numServices;			// Number of services online
   unsigned int numTotalUsers;			// Total # users online
   unsigned int numTotalUsersPeak;		// Peak number of users online
   unsigned int numServers;			// Total # servers online
   unsigned int numOpers;			// Total # of global opers
   unsigned int numUnknown;			// # of registering connections
   unsigned int numHelpers;			// Total # of helpers
   
   motd_t motd;					// The cached MOTD
   
 public:
   Daemon(String const &);			// Class constructor
   ~Daemon(void);				// Class destructor

   // Do we have a network name?
   bool haveNetworkName(void) const 
     {
	return (networkName.length() > 0);
     };
   
   // Return the name of the network this server is configured for
   String getNetworkName(void) const
     {
	return networkName;
     };
   
   // Get the current time (simple time_t)
   time_t getTime(void) const
     {
	return (time_t)currentTime.tv_sec;
     };
   
   // Get the current time (timeval style)
   long getTimeUsecs(void) const
     {
	return currentTime.tv_usec;
     };

   // Get the server start time
   time_t getStartTime(void) const
     {
	return startTime;
     };

   // Return our server record
   Server *myServer(void) const
     {
	return server;
     };
   
   void logger(String const &, 
	       int = LOGPRI_NOTICE);		// Log a string of text

   void rehash(Handler *, User *);		// Rehash

   String makeISUPPORT(void);			// Make an ISUPPORT string
   
   void broadcastServerNotice(snotice_bitmask_t,
			      String const &);	// Broadcast a server notice
   void broadcastWallops(Server *, 
			 String const &);	// Broadcast a wallops message
   void broadcastWallops(User *, String const &);
   void broadcastWallops(String const &message) 
     {
	broadcastWallops(server, message);
     };
   
   void addInputFD(int);			// Add an input fd
   void delInputFD(int);			// Delete an input fd
   void addOutputFD(int);			// Add an output fd
   void delOutputFD(int);			// Delete an output fd

   void newConnection(Listen *);		// accept() new connection

   void addUser(User *);			// Add a user to the user list
   User *getUser(String const &);		// Find a user
   void changeUserNick(User *, String const &,
		       time_t = 0);		// Change a users nickname
   void delUser(User *);			// Delete a user from the list
   void quitUser(User *, String const &, bool);	// User quitting IRC
   void killUser(User *, String const &, 
		 String const &);		// Kill a user off the network
   bool addUserSilence(User *, 
		       StringMask const &);	// Add/Broadcast a user silence
   bool delUserSilence(User *, 
		       StringMask const &);	// Delete a user silence
   bool addUserAccept(User *, User *);		// Add/Broadcast a user silence
   bool delUserAccept(User *, User *);		// Delete a user silence
   void snapshotUser(User *, Whowas::type_t,
		     String const &);		// Snapshot a user for WHOWAS
   
   void addChannel(Channel *, String const &);	// Add a channel to a list
   Channel *getChannel(String const &);		// Find a channel
   void joinChannel(Channel *, User *);		// User joining a channel
   void leaveChannel(Channel *, User *);	// Raw user leave routine
   void partChannel(Channel *, User *, 
		    String const &);		// User parting a channel
   void kickChannelMember(Channel *, User *, User *,
		    String const &);		// User kicking another user
   void changeChannelTopic(Channel *, User *,
			   String const &);	// Change the channel topic
   void changeChannelTopic(Channel *, Server *, String const &, 
			   String const &, time_t);
   
   Operator *getOperator(String const &);	// Grab an operator record

   void addServer(Server *);			// Add a server to the list
   Server *getServer(char);			// Grab a server record
   Server *getServer(String const &);
   Server *getServer(StringMask const &);
   void changeServerMode(Server *, Server *, String const &,
			 StringTokens *);	// Change a server mode
   void changeServerMode(Server *, Handler *, User *, String const &, 
			 StringTokens *);
   
   Handler *routeTo(Server *);			// Find a route to a server
   Handler *routeTo(User *);			// Find a route to a user
   
   void wipeFails(void);			// Wipe fails lists clean
   void wipeListens(void);			// Wipe all listening sockets
   void wipeOpers(void);			// Wipe the operators list
   void wipeRedirects(void);			// Wipe the redirects
   
   // Check if a channel is bad
   String failedChannel(String const &channel)
     {
	return onRelationMaskList(&failChannels, channel.IRCtoLower());
     };
     
   // Check if a nickname is bad
   String failedNickname(String const &nickname)
     {
	return onRelationMaskList(&failNicknames, nickname.IRCtoLower());
     };
   
   // Check for channel redirects
   String redirectedChannel(String const &channel)
     {
	return onRelationMaskList(&redirectChannels, channel.IRCtoLower());
     };
   
   // Configuration top-level class parsing routines
   bool configure(bool = false);
   static void configADMIN(Daemon *daemon, ConfigData *conf, String *line, String::length_t *pos, bool firstRun);
   static void configARBITER(Daemon *daemon, ConfigData *conf, String *line, String::length_t *pos, bool firstRun);
   static void configCONF(Daemon *daemon, ConfigData *conf, String *line, String::length_t *pos, bool firstRun);
   static void configFAIL(Daemon *daemon, ConfigData *conf, String *line, String::length_t *pos, bool firstRun);
   static void configLISTEN(Daemon *daemon, ConfigData *conf, String *line, String::length_t *pos, bool firstRun);
   static void configOPERS(Daemon *daemon, ConfigData *conf, String *line, String::length_t *pos, bool firstRun);
   static void configREDIRECT(Daemon *daemon, ConfigData *conf, String *line, String::length_t *pos, bool firstRun);
   static void configSSL(Daemon *daemon, ConfigData *conf, String *line, String::length_t *pos, bool firstRun);
   static void configSTATUS(Daemon *daemon, ConfigData *conf, String *line, String::length_t *pos, bool firstRun);
   
   void shutdown(String const & = "");		// Start the shutdown sequence
   void run(void);				// The main loop

   friend class Handler;
   friend class Connection;
   friend class statsFunctions;
# ifdef HAVE_OPENSSL
   friend class SSLSocketIO;
# endif
};


// Configuration data class, used during configuration file parsing only
class ConfigData {
 public:
# ifdef STL_HAS_SLIST
   typedef list <String> motd_buffer_t;
# endif
   
   // ADMIN class
   String adminName;
   String adminEmail;
   String adminLocation;

   // ARBITER class
   
   // CONF class
   bool confAutobone;
   String confDescription;
   bool confHidden;
   String confMOTD;
   String confNetwork;
   bool confNoop;
   String confServername;
   
   // FAIL class
   Daemon::relationmask_list_t failNicknames;
   Daemon::relationmask_list_t failChannels;
   
   // LISTEN class
   
   // OPERS class
   Daemon::operator_map_t opersOperators;
   
   // REDIRECT class
   Daemon::relationmask_list_t redirectChannels;

   // STATUS class
   
   // extra stuff
# ifdef STL_HAS_SLIST
   motd_buffer_t motd;
# endif
   
   ConfigData(void);				// Constructor
};


#endif

