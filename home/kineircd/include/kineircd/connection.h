/* connection.h
 * Connection class
 */

#ifndef __CONNECTION_H_
# define __CONNECTION_H_

# include <queue>

class Connection;

# include "daemon.h"
# include "handler.h"
# include "socket.h"

// Connection status flags
# define CONFLAG_CONNECTED	0x0001	// Connected
# define CONFLAG_PING_SENT	0x0002	// Sent a ping, waiting for a pong

class Connection {
 private:
   Socket *socket;			// The connected socket

   std::queue< String > outQueue;		// Output queue
   
   unsigned long long sentBytes;	// Number of bytes sent
   unsigned long sentMessages;		// Number of messages sent (lines)
   
   unsigned long long receivedBytes;	// Number of bytes received
   unsigned long receivedMessages;	// Number of messages sent (lines)
   
   short status;			// Connection status flags

   Handler *handler;			// Data handler
   
   Daemon *daemon;			// **RECURSIVE** Return path

   bool secure;				// Is this connection secured?
   
   time_t const connectedTime;		// Time connection was established
   time_t lastSpoke;			// Time the connection last spoke

   String *name;			// The name of this connection
   
 public:
   Connection(Socket *, Daemon *, 
	      bool);			// Class constructor
   ~Connection(void);			// Shutdown connection

   // Return the link up to the Daemon class
   Daemon *getDaemon(void) const
     {
	return daemon;
     };
   
   // Is this connection secure?
   bool isSecure(void) const
     {
	return secure;
     };
   
   // Return the connection time
   time_t getConnectedTime(void) const
     {
	return connectedTime;
     };
   
   // Return the last time this connection 'spoke'
   time_t getLastSpoke(void) const
     {
	return lastSpoke;
     };

   // Update the 'lastSpoke' variable (this should be inlined0)
   void touch(void);

   void handleInput(void);		// Handle input on connection
   
   void sendRaw(String *);		// Send a line to the connection raw
   void sendRaw(String);

   void killQueue(void);		// Kill the queue
   void sendQueue(void);		// Send an output queue line
   
   void goodbye(String * = 0);		// Close down socket
   void goodbye(String);
   void kill(void);			// Kill the connection (sharp)
   
   friend class Daemon;
   friend class statsFunctions;
   friend class registerHandler;
};

#endif

