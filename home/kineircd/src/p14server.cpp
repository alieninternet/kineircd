/* p14server.cpp
 * Handle SERVER connection data using the austnet P14 server<->server protocol
 */

#include "autoconf.h"

#include "p14server.h"
#include "daemon.h"
#include "debug.h"
#include "lang.h"

#warning "The P14 server protocol is incomplete. Only developers should compile it."

// Functions table (from P14_Notes)
struct p14serverHandler::functionTableStruct const 
  p14serverHandler::functionsTable[] = {
       { 0 },			// 0x00 = NICK (change)
       { 0 },			// 0x01 = NICK (assignment)
       { 0 },			// 0x02 = MODE (user)
       { 0 },			// 0x03 = MODE (channel)
       { 0 },			// 0x04 = MODE (server)
       { 0 },			// 0x05 = PRIVMSG (to channel)
       { 0 },			// 0x06 = PRIVMSG (to user)
       { 0 },			// 0x07 = PRIVMSG (to broadcast mask)
       { 0 },			// 0x08 = NOTICE (to channel)
       { 0 },			// 0x09 = NOTICE (to user)
       { 0 },			// 0x0A = NOTICE (to broadcast mask)
       { 0 },			// 0x0B = JOIN
       { 0 },			// 0x0C = PART
       { 0 },			// 0x0D = GNOTICE (broadcast)
       { 0 },			// 0x0E = TOPIC (broadcast)
       { 0 },			// 0x0F = KICK (broadcast)
       { 0 },			// 0x10 = PING
       { 0 },			// 0x11 = PONG
       { 0 },			// 0x12 = KILL (broadcast)
       { 0 },			// 0x13 = AWAY (broadcast)
       { 0 },			// 0x14 = WALLOPS (broadcast)
       { 0 },			// 0x15 = ACCEPT (broadcast)
       { 0 },			// 0x16 = SILENCE (broadcast)
       { 0 },			// 0x17 = QUIT (user quit)
       { 0 },			// 0x18 = SQUIT (server quit)
       { 0 },			// 0x19 = LANGUAGE (broadcast)
       { 0 },			// 0x1A = WHOIS (remote)
       { 0 },			// 0x1B = SERVER (broadcast)
       { 0 },			// 0x1C = SERVICE (broadcast)
       { 0 },			// 0x1D = INVITE
       { 0 },			// 0x1E = ADMIN (remote)
       { 0 },			// 0x1F = CONNECT (remote)
       { 0 },			// 0x20 = MOTD (remote)
       { 0 },			// 0x21 = TIME (remote)
       { 0 },			// 0x22 = VERSION (remote)
       { 0 },			// 0x23 = TRACEROUTE (remote)
       { 0 },			// 0x24 = TRACE
       { 0 },			// 0x25 = STATS (remote)
       { 0 },			// 0x26 = RPING
       { 0 },			// 0x27 = RPONG
       { 0 },			// 0x28 = UPING
       { 0 },			// 0x29 = UPONG
       { 0 },			// 0x30 = NUMERIC
       { 0 },			// 0x31 = EOB_REPLY
       { 0 }			// 0x32 = END_OF_BURST
  };


/* p14serverHandler - Constructor for the server handler (P14) sub-class
 * Original 07/10/01, Simon Butcher <pickle@austnet.org>
 */
p14serverHandler::p14serverHandler(Connection *c, Server *s)
: Handler(c),
  server(s)
{
#ifdef DEBUG
   debug("New Handler: p14serverHandler");
#endif

   // Send a nice server notice out
   Daemon::serverNotice(LocalUser::SN_NETWORK,
			String::printf((char *)Lang::L_SERVNOTICE_LINK,
				       (char const *)server->getHostname(),
				       "P14"));
   
   // Increase the server connection count
   Daemon::numServerConns++;
}


/* ~p14serverHandler - Class destructor
 * Original 07/10/01, Simon Butcher <pickle@austnet.org>
 */
p14serverHandler::~p14serverHandler(void)
{
   // Lower the server connection count
   Daemon::numServerConns--;
}


