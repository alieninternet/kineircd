/* p14server.cpp
 * Handle SERVER connection data using the austnet P14 server<->server protocol
 */

#include "config.h"

#include "handler.h"
#include "language.h"

// Functions table (from P14_Notes)
p14serverHandler::functionTableStruct p14serverHandler::functionsTable[] = {
     { 'a',	0 },			// ADMIN (remote)
     { 'A',	0 },			// AWAY
     { 'b',	0 },			// NOTICE (to broadcast mask)
     { 'B',	0 },			// PRIVMSG (to broadcast mask)
     { 'c',	0 },			// CONNECT (remote)
     { 'E',	0 },			// END_OF_BURST
     { 'f',	0 },			// MODE (user)
     { 'F',	0 },			// MODE (channel)
     { 'g',	0 },			// MODE (server)
     { 'G',	0 },			// GNOTICE (broadcast)
     { 'i',	0 },			// INFO (remote)
     { 'I',	0 },			// INVITE
     { 'J',	0 },			// JOIN
     { 'k',	0 },			// KICK
     { 'K',	0 },			// KILL (broadcast)
     { 'l',	0 },			// LINKS (remote)
     { 'L',	0 },			// PART
     { 'm',	0 },			// NOTICE (to user)
     { 'M',	0 },			// PRIVMSG (to user)
     { 'n',	0 },			// NICK (change)
     { 'N',	0 },			// NICK (assignment)
     { 'O',	0 },			// MOTD (remote)
     { 'p',	0 },			// PONG
     { 'P',	0 },			// PING
     { 'q',	0 },			// QUIT (user quit)
     { 'Q',	0 },			// SQUIT (server quit)
     { 'r',	0 },			// TRACEROUTE (remote)
     { 'R',	0 },			// TRACE
     { 's',	0 },			// STATS (remote)
     { 'S',	0 },			// SERVER (broadcast)
     { 't',	0 },			// TIME (remote)
     { 'T',	0 },			// TOPIC (broadcast)
     { 'v',	0 },			// VERSION (remote)
     { 'w',	0 },			// WHOIS (remote)
     { 'W',	0 },			// WALLOPS (broadcast)
     { 'x',	0 },			// NOTICE (to channel)
     { 'X',	0 },			// PRIVMSG (to channel)
     { 'y',	0 },			// ACCEPT (broadcast)
     { 'Y',	0 },			// SILENCE (broadcast)
     { 'z',	0 },			// RPONG
     { 'Z',	0 },			// RPING
     { 0 }
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
   getConnection()->getDaemon()->
     broadcastServerNotice(SERVERNOTICE_NETWORK,
			   String::printf(LNG_SERVNOTICE_LINK " (P14)",
					  (char const *)server->getHostname()));
   
   // Increase the server connection count
   getConnection()->getDaemon()->numServerConns++;
}


/* ~p14serverHandler - Class destructor
 * Original 07/10/01, Simon Butcher <pickle@austnet.org>
 */
p14serverHandler::~p14serverHandler(void)
{
   // Lower the server connection count
   getConnection()->getDaemon()->numServerConns--;
}


