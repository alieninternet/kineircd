/* socket.cpp
 * Socket handling classes
 * 
 * Copyright (c) 1999-2001 Alien Internet Services
 */

#include "config.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>

#ifdef HAVE_OPENSSL
# include <openssl/ssl.h>
#endif

#include "daemon.h"
#include "socket.h"

#if defined(__MACH__)
typedef int socklen_t;
#endif

#if defined(__MACH__)
#define NO_IPV6_SUPPORT
#endif

/* Socket - Base class destructor
 * Original 13/08/01, Simon Butcher <simonb@alien.net.au>
 */
Socket::~Socket(void)
{
   // Close the socket, perhaps
   if (fd >= 0) {
      ::close(fd);
   }
   
   // Shut down the IO class if we can
   if (io) {
      delete io;
   }
}


// Make this inline?!
void Socket::close(void)
{
#ifdef DEBUG_EXTENDED
   debug(String::printf("[%d] close()", fd));
#endif
   ::close(fd);
   fd = -1;
}


/* setNonBlocking - Set socket NONBLOCKING so it doesn't slow us down
 * Original 18/12/00, Simon Butcher <simonb@alien.net.au>
 */
bool Socket::setNonBlocking(bool nonblock /* = true */)
{
   long flags = 0;
   
   // Sanity check
   if (fd < 0) {
#ifdef DEBUG
      debug("setNonBlocking(): Called with a bad file descriptor!");
#endif
      return false;
   }
   
   // We first get the file descriptor flags
   if (!fcntl(fd, F_GETFL, &flags)) {
#ifdef DEBUG
      debug(String::printf("setNonBlocking(): Could not F_GETFL with "
			   "fcntl (%s)",
			   strerror(errno)));
#endif
      return false;
   }
   
   // Check if it is already set
   if (flags & O_NONBLOCK) {
#ifdef DEBUG
      debug(String::printf("setNonBlocking(): Socket is already set "
			   "non-blocking (%d)",
			   fd));
#endif
      return true;
   }
   
   // Modify them as necessary
   if (nonblock) {
      return (fcntl(fd, F_SETFL, flags | O_NONBLOCK) >= 0);
   } 

   return (fcntl(fd, F_SETFL, flags & ~O_NONBLOCK) >= 0);
}


/* setReuseAddress - Set socket SO_REUSEADDR to save binding time :)
 * Original 09/01/01, Simon Butcher <simonb@alien.net.au>
 */
bool Socket::setReuseAddress(void)
{
   int sockopts = 1;
   
   return (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR,
		      (char *)&sockopts, sizeof(sockopts)) == 0);
}


/* SocketIPv4 - [Various forms] Create a new socket
 * Original 06/02/99, Simon Butcher <simonb@alien.net.au
 */
SocketIPv4::SocketIPv4(Daemon *d, Connection *c /* = 0 */ )
: Socket(d, c, ::socket(AF_INET, SOCK_STREAM, 0), Socket::IPv4, 0, 0)
{
   remoteAddress.s_addr = localAddress.s_addr = 0;
}

SocketIPv4::SocketIPv4(unsigned long newAddress, unsigned short newPort,
		       bool accepting, bool secure, Daemon *d, 
		       Connection *c /* = 0 */)
: Socket(d, c, ::socket(AF_INET, SOCK_STREAM, 0), Socket::IPv4, 0, newPort)
{
   remoteAddress.s_addr = 0;
   localAddress.s_addr = htonl(newAddress);

#ifdef HAVE_OPENSSL
   if (!secure) {
#ifdef DEBUG
      debug("New SocketIPv4 with PlainSocketIO");
#endif
      io = new PlainSocketIO(this);
   } else {
#ifdef DEBUG
      debug("New SocketIPv4 with SSLSocketIO");
#endif
      io = new SSLSocketIO(this, accepting);
   }
#else
   io = new PlainSocketIO(this);
#endif
}

SocketIPv4::SocketIPv4(int newfd, in_addr newAddress, unsigned short newPort,
		       bool accepting, bool secure, Daemon *d, 
		       Connection *c /* = 0 */)
: Socket(d, c, newfd, Socket::IPv4, newPort, 0),
  remoteAddress(newAddress)
{
   localAddress.s_addr = 0;

#ifdef HAVE_OPENSSL
   if (!secure) {
#ifdef DEBUG
      debug("New SocketIPv4 with PlainSocketIO");
#endif
      io = new PlainSocketIO(this);
   } else {
#ifdef DEBUG
      debug("New SocketIPv4 with SSLSocketIO");
#endif
      io = new SSLSocketIO(this, accepting);
   }
#else
   io = new PlainSocketIO(this);
#endif
}


/* getRemoteAddressStr - Return the remote address as a string
 * Original 13/08/01, Simon Butcher <simonb@alien.net.au>
 */
String SocketIPv4::getRemoteAddressStr(void) const
{
   return String(inet_ntoa(remoteAddress));
}


/* getLocalAddressStr - Return the local address as a string
 * Original 13/08/01, Simon Butcher <simonb@alien.net.au>
 */
String SocketIPv4::getLocalAddressStr(void) const
{
   return String(inet_ntoa(localAddress));
}


/* bind - Bind socket to a port for incoming connections
 * Original 02/01/01, Simon Butcher <simonb@alien.net.au>
 * Note: Yes, it is day two of 2001 :)
 */
bool SocketIPv4::bind(void)
{
   struct sockaddr_in addr;
   
   // Quickly clean the structure
   memset(&addr, 0, sizeof(addr));
 
   // Set up this address...
   addr.sin_family = AF_INET;
   addr.sin_addr = localAddress;
   addr.sin_port = htons(localPort);
   
   // Do the bind..
   return (::bind(fd, (struct sockaddr *)&addr, sizeof(addr)) == 0);
}


/* accept - Accept a new connection on a socket
 * Original 02/01/01, Simon Butcher <simonb@alien.net.au>
 */
SocketIPv4 *SocketIPv4::accept(bool secure /* = false */)
{
   struct sockaddr_in addr;
   socklen_t addrlen = sizeof(addr);
   
   int newfd = ::accept(fd, (sockaddr *)&addr, &addrlen);
   
   // Check that worked..
   if (newfd < 0) {
      struct in_addr dummy;
      dummy.s_addr = 0;
      return new SocketIPv4(-1, dummy, true, false, 0, 0); // Dud socket.
   }

   // Create the new socket
   SocketIPv4 *newsock = new SocketIPv4(newfd,
					addr.sin_addr,
					ntohs(addr.sin_port),
					true, secure, daemon);

   // Fix up local details
   newsock->localPort = localPort;
   newsock->localAddress = localAddress;
   
   // Return with the fresh socket
   return newsock;
}


/* connect - Connect a socket to a remote site
 * Original 07/02/99, Simon Butcher <simonb@alien.net.au>
 */
bool SocketIPv4::connect(void)
{
   struct sockaddr_in addr;
   
   memset(&addr, 0, sizeof(addr));
   
   addr.sin_family = AF_INET;
   addr.sin_addr = remoteAddress;
   addr.sin_port = htons(remotePort);
   
   return (::connect(fd, (struct sockaddr *)&addr, sizeof(addr)) == 0);
}


#if !defined(NO_IPV6_SUPPORT)

/* SocketIPv6 - [Various forms] Create a new socket
 * Original 19/08/01, Simon Butcher <simonb@alien.net.au
 */
SocketIPv6::SocketIPv6(Daemon *d, Connection *c = 0)
: Socket(d, c, ::socket(AF_INET6, SOCK_STREAM, 0), Socket::IPv6, 0, 0)
{
   memset(&remoteAddress, 0, sizeof(remoteAddress));
   memset(&localAddress, 0, sizeof(localAddress));
}

SocketIPv6::SocketIPv6(unsigned long newAddress, unsigned short newPort,
		       bool accepting, bool secure, Daemon *d,
		       Connection *c = 0)
: Socket(d, c, ::socket(AF_INET6, SOCK_STREAM, 0), Socket::IPv6, 0, newPort)
{
   memset(&remoteAddress, 0, sizeof(remoteAddress));
   // localAddress = ?!
   // localAddress = ?!
   // localAddress = ?!
   // localAddress = ?!
   // localAddress = ?!
   // localAddress = ?!
   memset(&localAddress, 0, sizeof(localAddress));

#ifdef HAVE_OPENSSL
   if (!secure) {
#ifdef DEBUG
      debug("New SocketIPv6 with PlainSocketIO");
#endif
      io = new PlainSocketIO(this);
   } else {
#ifdef DEBUG
      debug("New SocketIPv6 with SSLSocketIO");
#endif
      io = new SSLSocketIO(this, accepting);
   }
#else
   io = new PlainSocketIO(this);
#endif
}

SocketIPv6::SocketIPv6(int newfd, in6_addr newAddress, unsigned short newPort,
		       bool accepting, bool secure, Daemon *d, 
		       Connection *c = 0)
: Socket(d, c, newfd, Socket::IPv6, newPort, 0),
  remoteAddress(newAddress)
{
   memset(&localAddress, 0, sizeof(localAddress));

#ifdef HAVE_OPENSSL
   if (!secure) {
#ifdef DEBUG
      debug("New SocketIPv6 with PlainSocketIO");
#endif
      io = new PlainSocketIO(this);
   } else {
#ifdef DEBUG
      debug("New SocketIPv6 with SSLSocketIO");
#endif
      io = new SSLSocketIO(this, accepting);
   }
#else
   io = new PlainSocketIO(this);
#endif
}


/* getRemoteAddressStr - Return the remote address as a string
 * Original 19/08/01, Simon Butcher <simonb@alien.net.au>
 */
String SocketIPv6::getRemoteAddressStr(void) const
{
   char buff[INET6_ADDRSTRLEN + 1];
   
   return String(inet_ntop(AF_INET6, &remoteAddress, 
			   buff, INET6_ADDRSTRLEN + 1));
}


/* getLocalAddressStr - Return the local address as a string
 * Original 19/08/01, Simon Butcher <simonb@alien.net.au>
 */
String SocketIPv6::getLocalAddressStr(void) const
{
   char buff[INET6_ADDRSTRLEN + 1];
   
   return String(inet_ntop(AF_INET6, &localAddress,
			   buff, INET6_ADDRSTRLEN + 1));
}


/* bind - Bind socket to a port for incoming connections
 * Original 19/08/01, Simon Butcher <simonb@alien.net.au>
 */
bool SocketIPv6::bind(void)
{
   struct sockaddr_in6 addr;
   
   // Quickly clean the structure
   memset(&addr, 0, sizeof(addr));
 
   // Set up this address...
   addr.sin6_family = AF_INET6;
   addr.sin6_addr = localAddress;
   addr.sin6_port = htons(localPort);
   
   // Do the bind..
   return (::bind(fd, (struct sockaddr *)&addr, sizeof(addr)) == 0);
}


/* accept - Accept a new connection on a socket
 * Original 19/08/01, Simon Butcher <simonb@alien.net.au>
 */
SocketIPv6 *SocketIPv6::accept(bool secure = false)
{
   struct sockaddr_in6 addr;
   socklen_t addrlen = sizeof(addr);
   
   int newfd = ::accept(fd, (sockaddr *)&addr, &addrlen);
   
   // Check that worked..
   if (newfd < 0) {
      struct in6_addr dummy;
      memset(&dummy, 0, sizeof(dummy));
      return new SocketIPv6(-1, dummy, true, false, 0, 0); // Dud socket.
   }

   // Create the new socket
   SocketIPv6 *newsock = new SocketIPv6(newfd,
					addr.sin6_addr,
					ntohs(addr.sin6_port),
					true, secure, daemon);
   
   // Fix up local details
   newsock->localPort = localPort;
   newsock->localAddress = localAddress;
   
   // Return with the fresh socket
   return newsock;
}


/* connect - Connect a socket to a remote site
 * Original 19/08/01, Simon Butcher <simonb@alien.net.au>
 */
bool SocketIPv6::connect(void)
{
   struct sockaddr_in6 addr;
   
   memset(&addr, 0, sizeof(addr));
   
   addr.sin6_family = AF_INET6;
   addr.sin6_addr = remoteAddress;
   addr.sin6_port = htons(remotePort);
   
   return (::connect(fd, (struct sockaddr *)&addr, sizeof(addr)) == 0);
}

#endif

/* PlainSocketIO - Constructor
 * Original 16/09/01, Simon Butcher <simonb@alien.net.au>
 */
PlainSocketIO::PlainSocketIO(Socket *sock)
: SocketIO(sock),
  pos(0)
{
   buff[0] = '\0';
}


/* write - Write text to a socket
 * Original 07/02/99, Simon Butcher <simonb@alien.net.au>
 */
bool PlainSocketIO::write(String str)
{
   // Sanity check...
   if (socket->fd >= 0) {
      // Send the data
      return (::write(socket->fd, (const char *)str, str.length()) == 
	      str.length());
   }

   return false;
}


/* read - Read a line from the socket
 * Original 09/02/99, Simon Butcher <simonb@alien.net.au>
 * Needs: Rewrite.
 */
String PlainSocketIO::read(void)
{
   // Check that the file descriptor is ok!!
   if (socket->fd < 0) {
#ifdef DEBUG
      debug("PlainSocketIO::read(): Called on a closed file descriptor!");
#endif
      return String("");
   }

   int nb;
   char c;

   // Loop until we get a CR
   while (c != '\n') {
      nb = ::read(socket->fd, &c, 1);
      switch (nb) {
       case 0: // Broke
       case -1: // Nothing there
	 // If an interrupt interrupted us (well, duh) return an empty string
	 switch (errno) {
	  case EINTR:
	  case EAGAIN:
	    return String("");
	 }
	 
	 // Otherwise, shut this connection down
	 if (socket->connection) {
	    socket->connection->killQueue();
	    socket->connection->goodbye(String::printf("Read error: %s",
						       strerror(errno)));
	 }
	 
	 return String("");
      }
      
      // Add the char if we can
      if (nb != -1) {
	 buff[pos++] = c;
      }
   }
   
   // Add a null appropriately
   if (pos > 1 && buff[pos-2] == '\r') {
      buff[pos-2] = '\0';
   } else {
      buff[pos-1] = '\0';
   }
   
   // Make the string for reply
   String reply = buff;
   
   // Reset the position and buffer for the next run
   pos = 0;
   buff[0] = '\0';
   
   return reply;
}


// Should we compile the ssl components or not?
#ifdef HAVE_OPENSSL

/* SSLSocketIO - Constructor
 * Original 16/09/01, Simon Butcher <simonb@alien.net.au>
 */
SSLSocketIO::SSLSocketIO(Socket *sock, bool accepting)
: SocketIO(sock),
  pos(0)
{
   buff[0] = '\0';
   
   // Set up SSL stuff
# ifdef DEBUG
   debug("Setting up new SSL data");
# endif
   ssl = SSL_new(socket->daemon->sslContext);
   SSL_set_fd(ssl, socket->fd);
   
   // Assert the link
   if (accepting) { 
# ifdef DEBUG
      debug("Accepting on SSL connection...");
# endif
      SSL_set_accept_state(ssl);
      
      if (!SSL_accept(ssl)) {
# ifdef DEBUG
	 debug("(Failed)");
# endif
	 socket->close();
      }
# ifdef DEBUG
      else {
	 debug("(OK)");
# endif
      }
   } else {
# ifdef DEBUG
      debug("Connecting on SSL connection...");
# endif
      SSL_set_connect_state(ssl);
      
      if (!SSL_connect(ssl)) {
# ifdef DEBUG
	 debug("(Failed)");
# endif
	 socket->close();
      }
# ifdef DEBUG
      else {
	 debug("(OK)");
# endif
      }
   }
}


/* write - Write text to a socket
 * Original 07/02/99, Simon Butcher <simonb@alien.net.au>
 */
bool SSLSocketIO::write(String str)
{
   // Sanity check...
   if (socket->fd >= 0) {
      // Send the data
      return (SSL_write(ssl, (const char *)str, str.length()) == 
	      str.length());
   }

   return false;
}


/* read - Read a line from the socket
 * Original 09/02/99, Simon Butcher <simonb@alien.net.au>
 * Needs: Rewrite.
 */
String SSLSocketIO::read(void)
{
   // Check that the file descriptor is ok!!
   if (socket->fd < 0) {
# ifdef DEBUG
      debug("SSLSocketIO::read(): Called on a closed file descriptor!");
# endif
      return String("");
   }

   int nb;
   char c;

   // Loop until we get a CR
   while (c != '\n') {
      nb = SSL_read(ssl, &c, 1);
      switch (nb) {
       case 0: // Broke
       case -1: // Nothing there
	 // If an interrupt interrupted us (well, duh) return an empty string
	 switch (errno) {
	  case EINTR:
	  case EAGAIN:
	    return String("");
	 }
	 
	 // Otherwise, shut this connection down
	 if (socket->connection) {
	    socket->connection->killQueue();
	    socket->connection->goodbye(String::printf("Read error: %s",
						       strerror(errno)));
	 }
	 
	 return String("");
      }
      
      // Add the char if we can
      if (nb != -1) {
	 buff[pos++] = c;
      }
   }
   
   // Add a null appropriately
   if (pos > 1 && buff[pos-2] == '\r') {
      buff[pos-2] = '\0';
   } else {
      buff[pos-1] = '\0';
   }
   
   // Make the string for reply
   String reply = buff;
   
   // Reset the position and buffer for the next run
   pos = 0;
   buff[0] = '\0';
   
   return reply;
}



/* getInfo - Return information about the socket
 * Original 16/09/01, Simon Butcher <pickle@austnet.org>
 */
String SSLSocketIO::getInfo(void)
{
   return String::printf("SSL %s; Cipher: %s %s",
			 SSL_get_version(ssl),
			 SSL_get_cipher_name(ssl),
			 SSL_get_cipher_version(ssl));
}

#endif
