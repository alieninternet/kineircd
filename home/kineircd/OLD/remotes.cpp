/* remotes.cpp
 * Protocol reply routines, remote replies.
 * 
 * NOTE: This needs to be seriously reworked! If/When irc3user comes half of
 * this may (most likely) become obsolete - a more generic approach must be
 * taken!
 */

#include "config.h"

#include <ctype.h>
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "handler.h"
#include "numerics.h"
#include "language.h"
#include "version.h"
#include "modes.h"


/* doADMIN
 * Original 27/08/01, Simon Butcher <pickle@austnet.org>
 * Note: We do something slightly dodgey here; The colon prefixes are set
 * 	 up in the variables themselves so we do not have to waste time
 *       creating a new string (adding the colon to the string).. Ok, it is
 *       a waste of three bytes of memory, but three bytes of memory is worth
 *       it :)
 */
void Handler::doADMIN(Handler *handler, User *from)
{
   handler->sendNumeric(TO_DAEMON->server, RPL_ADMINME, from, 
			TO_DAEMON->server->getHostname() + LNG_RPL_ADMINME);
   handler->sendNumeric(TO_DAEMON->server, RPL_ADMINLOC1, from,
			TO_DAEMON->adminName);
   
   // The second location line is optional...
   if (TO_DAEMON->adminLocation.length()) {
      handler->sendNumeric(TO_DAEMON->server, RPL_ADMINLOC2, from,
			   TO_DAEMON->adminLocation);
   }
   
   handler->sendNumeric(TO_DAEMON->server, RPL_ADMINEMAIL, from,
			TO_DAEMON->adminEmail);
}


/* doINFO
 * Original 14/08/01, Simon Butcher <pickle@austnet.org>
 */
void Handler::doINFO(Handler *handler, User *from)
{
   handler->sendNumeric(TO_DAEMON->server, RPL_INFO, from,
":Portions of this code Copyright (c) 1996-2001 Alien Internet Services");
   handler->sendNumeric(TO_DAEMON->server, RPL_INFO, from,
":Umm.. stuff goes here... :)");

   // End of the list
   handler->sendNumeric(TO_DAEMON->server, RPL_ENDOFINFO, from, 
			LNG_RPL_ENDOFINFO);
}


/* doLINKS
 * Original 10/09/01, Simon Butcher <pickle@austnet.org>
 * Note: Yes, this works a little differently than usual. Usually with send
 *       out a server name and the link to name, then the hops etc. This time
 *       we send out the server name and OUR name. Why? Firstly the client
 *       doesn't need that much information anyway, and secondly when it comes
 *       to a routed network rather than the star topology network of
 *       old IRC networks, the data is meaningless and all that matters are
 *       the hops.
 */
void Handler::doLINKS(Handler *handler, User *from, String const &request)
{
   // Grab the appropriate mask requested
   String maskStr = "";
   
   if (!request.length()) {
      maskStr = "*";
   } else {
      maskStr = request.toLower();
   }
   
   StringMask mask(maskStr);
   
   // Run through the list
   for (Daemon::server_map_t::iterator it = 
	TO_DAEMON->servers.begin();
	it != TO_DAEMON->servers.end(); it++) {
      // Check for a match, and send this if the server is not hidden
      if (mask.matches((*it).second->getHostname()) && 
	  !((*it).second->isModeSet(Server::MODE_HIDDEN))) {
	 // Send the user this record
	 handler->sendNumeric(TO_DAEMON->server, RPL_LINKS, from,
			      String::printf("%s %s :%d %s",
					     (char const *)(*it).second->getHostname(),
					     (char const *)TO_DAEMON->server->getHostname(),
					     (*it).second->getNumHops(),
					     (char const *)(*it).second->getDescription()));
      }
   }
   
   // End of the list
   handler->sendNumeric(TO_DAEMON->server, RPL_ENDOFLINKS, from, 
			request + LNG_RPL_ENDOFLINKS);
}

   
/* doLUSERS
 * Original 13/08/01, Simon Butcher <pickle@austnet.org>
 */
void Handler::doLUSERS(Handler *handler, User *from, String const &request)
{
   handler->sendNumeric(TO_DAEMON->server, RPL_LUSERCLIENT, from,
			String::printf(LNG_RPL_LUSERCLIENT,
				       TO_DAEMON->numTotalUsers,
				       TO_DAEMON->numServices,
				       TO_DAEMON->numServers));
   handler->sendNumeric(TO_DAEMON->server, RPL_LUSEROP, from,
			String::printf(LNG_RPL_LUSEROP,
				       TO_DAEMON->numOpers));
   handler->sendNumeric(TO_DAEMON->server, RPL_LUSERHELPERS, from,
			String::printf(LNG_RPL_LUSERHELPERS,
				       TO_DAEMON->numHelpers));
   handler->sendNumeric(TO_DAEMON->server, RPL_LUSERUNKNOWN, from,
			String::printf(LNG_RPL_LUSERUNKNOWN,
				       TO_DAEMON->numUnknown));
   handler->sendNumeric(TO_DAEMON->server, RPL_LUSERCHANNELS, from,
			String::printf(LNG_RPL_LUSERCHANNELS,
				       (TO_DAEMON->channels.size() +
					TO_DAEMON->localChannels.size())));
   handler->sendNumeric(TO_DAEMON->server, RPL_LUSERME, from,
			String::printf(LNG_RPL_LUSERME,
				       TO_DAEMON->numClientConns,
				       TO_DAEMON->numServerConns));
   handler->sendNumeric(TO_DAEMON->server, RPL_LOCALUSERS, from,
			String::printf(LNG_RPL_LOCALUSERS,
				       TO_DAEMON->numClientConns,
				       TO_DAEMON->numClientConnsPeak));
   handler->sendNumeric(TO_DAEMON->server, RPL_GLOBALUSERS, from,
			String::printf(LNG_RPL_GLOBALUSERS,
				       TO_DAEMON->numTotalUsers,
				       TO_DAEMON->numTotalUsersPeak));
}


/* doMOTD
 * Original 13/08/01, Simon Butcher <pickle@austnet.org>
 * Note: Not too happy about the length of the request to get the server name
 * Note: The RPL_MOTDSTART conforms to the RFC - mIRC seems to mangle it now
 *       and then -- don't blame me!!
 * Note: The MOTD sequence in the main ircd class has the ':- ' suffixes added
 *       to the lines already, so we do not need to do additions to each line
 */
void Handler::doMOTD(Handler *handler, User *from)
{
   // Are we being called remote? Can we allow this to happen?
   if ((from->server != TO_DAEMON->server) &&
       !(TO_DAEMON->server->isModeSet(Server::MODE_REMOTEMOTD))) {
      // Not allowed to send the motd over the network
      handler->sendNumeric(TO_DAEMON->server, ERR_NOMOTD, from,
			   String::printf(LNG_ERR_NOMOTD_NOREMOTE,
					  ((char const *)
					   TO_DAEMON->server->getHostname())));
      return;
   }
   
   // Check if we have an MOTD to send..
   if (!TO_DAEMON->motd.empty()) {
      // Send the header
      handler->sendNumeric(TO_DAEMON->server, RPL_MOTDSTART, from,
			   String::printf(LNG_RPL_MOTDSTART,
					  (char const *)TO_DAEMON->server->getHostname()));

      // Run through the MOTD and send each line in the list
      for (Daemon::motd_t::iterator it = TO_DAEMON->motd.begin();
	   it != TO_DAEMON->motd.end(); 
	   it++) {
	 handler->sendNumeric(TO_DAEMON->server, RPL_MOTD, from, *it);
      }
	 
      // Send the footer
      handler->sendNumeric(TO_DAEMON->server, RPL_ENDOFMOTD, from, 
			   LNG_RPL_ENDOFMOTD);
   } else {
      // No MOTD to send
      handler->sendNumeric(TO_DAEMON->server, ERR_NOMOTD, from, 
			   LNG_ERR_NOMOTD);
   }
}


#define STATS_REPLY(name)	void name(Handler *handler, User *from)

// This is in a class so it can be friends with the ircd class
class statsFunctions {
 public:
   /* replyL - Link info (List of connections)
    * Original 26/08/01, Simon Butcher <pickle@austnet.org>
    */
   static STATS_REPLY(replyL)
     {
	// Run through the connection list...
	for (Daemon::connection_list_t::iterator it = TO_DAEMON->connections.begin();
	     it != TO_DAEMON->connections.end(); it++) {
	   handler->sendNumeric(TO_DAEMON->server,
				RPL_STATSLINKINFO, from,
				String::printf("%s %d %lu %llu %lu %llu %lu :%s",
					       ((*it)->name ?
						(char const *)(*(*it)->name) :
						"!"),
					       (*it)->outQueue.size(),
					       (*it)->sentMessages,
					       ((*it)->sentBytes / 
						1024),
					       (*it)->receivedMessages,
					       ((*it)->receivedBytes /
						1024),
					       ((unsigned long)
						(TO_DAEMON->getTime() -
						 (*it)->connectedTime)),
					       ((char const *)
						(*it)->socket->io->getInfo())));
	}
     }
   
   
   /* replyO - Server operator information ('o-lines')
    * Original 26/08/01, Simon Butcher <pickle@austnet.org>
    * Note: Slightly modified, most clients ignore it, other clients use it :)
    */
   static STATS_REPLY(replyO)
     {
	// Run through the list of operators and send a reply per identify host
	for (Daemon::operator_map_t::iterator it = TO_DAEMON->operators.begin();
	     it != TO_DAEMON->operators.end(); it++) {
	   handler->sendNumeric(TO_DAEMON->server,
				RPL_STATSOLINE, from,
				String::printf("%c %s * %s :%s",
					       ((*it).second->isGlobal() ?
						'O' : 'o'),
					       "*@*",
					       (char const *)(*it).first,
					       (char const *)(*it).second->getName()));
	}
     }

   
   /* replyRUSAGE - Server resource usage information
    * Original 28/08/01, Simon Butcher <pickle@austnet.org>
    */
   static STATS_REPLY(replyRUSAGE)
     {
	struct rusage usage;
	
	// Grab usage information for ourselves
	if (!getrusage(RUSAGE_SELF, &usage)) { // RUSAGE_SELF = 0?!
	   handler->sendNumeric(TO_DAEMON->server,
				999, from,
				String::printf(":Memory: Shared %ld, Data %ld, Stack %ld",
					       usage.ru_ixrss,
					       usage.ru_idrss,
					       usage.ru_isrss));
	   handler->sendNumeric(TO_DAEMON->server,
				999, from,
				String::printf(":Page faults: Major %ld, Minor %ld (%ld swaps)",
					       usage.ru_majflt,
					       usage.ru_minflt,
					       usage.ru_nswap));
	   handler->sendNumeric(TO_DAEMON->server,
				999, from,
				String::printf(":Blocks: In %ld, Out %ld",
					       usage.ru_inblock,
					       usage.ru_oublock));
	   handler->sendNumeric(TO_DAEMON->server,
				999, from,
				String::printf(":Messages: Received %ld, Sent %ld",
					       usage.ru_msgrcv,
					       usage.ru_msgsnd));
	   handler->sendNumeric(TO_DAEMON->server,
				999, from,
				String::printf(":Context Switches: Voluntary %ld, Involuntary %ld (%ld signals)",
					       usage.ru_nvcsw,
					       usage.ru_nivcsw,
					       usage.ru_nsignals));
	} else {
	   handler->sendNumeric(TO_DAEMON->server,
				999, from,
				String::printf(":getrusage() failed (%s)",
					       strerror(errno)));
	}
     }
   
   
   /* replyU - Server uptime information
    * Original 26/08/01, Simon Butcher <pickle@austnet.org>
    */
   static STATS_REPLY(replyU)
     {
        time_t uptime = (TO_DAEMON->getTime() - TO_DAEMON->startTime);
	
	handler->sendNumeric(TO_DAEMON->server,
			     RPL_STATSUPTIME, from,
			     String::printf(LNG_RPL_STATSUPTIME,
					    (int)(uptime / 86400),
					    ((int)(uptime % 86400) / 3600),
					    ((int)(uptime % 3600) / 60),
					    (int)(uptime % 60)));
	handler->sendNumeric(TO_DAEMON->server,
			     RPL_STATSCONN, from,
			     String::printf(LNG_RPL_STATSCONN,
					    TO_DAEMON->numConnsPeak,
					    TO_DAEMON->numClientConnsPeak));
     }
};
   

// Stats functions table
struct {
   char *request;
   STATS_REPLY((*function));
} statsFunctionsTable[] = {
     { "L",		statsFunctions::replyL },
     { "O",		statsFunctions::replyO },
     { "RUSAGE",	statsFunctions::replyRUSAGE },
     { "U",		statsFunctions::replyU },
     { 0 }
};


/* doSTATS
 * Original 14/08/01, Simon Butcher <pickle@austnet.org>
 */
void Handler::doSTATS(Handler *handler, User *from, String const &request)
{
   // Fix up the request for easy searching
   StringMask requestMask(request.toUpper());

#ifdef NOTIFY_ON_CMD_STATS
   // Send a server notice out regarding this stats request
   TO_DAEMON->
     broadcastServerNotice(String::printf(LNG_SERVNOTICE_NOTIFY_ON_CMD_STATS,
					  (char const *)request,
					  (char const *)from->nickname,
					  (char const *)from->getAddress()));
#endif
   
   // Run through the list of stats functions
   for (int i = 0; statsFunctionsTable[i].request != 0; i++) {
      // Check for a match
      if (requestMask.matches((char const *)statsFunctionsTable[i].request)) {
	 // Run the function
	 statsFunctionsTable[i].function(handler, from);
      }
   }

   // Send the footer
   handler->sendNumeric(TO_DAEMON->server, RPL_ENDOFSTATS, from, 
			request + LNG_RPL_ENDOFSTATS);
}


/* doTIME
 * Original 29/08/01, Simon Butcher <pickle@austnet.org>
 * Note: The RFC doesn't mention the additional integer before the time string,
 *       but some IRC servers seem to include it, so I'vesneaked it in too - 
 *	 shhh maybe nobody will notice :) This is really fairly obsolete now
 *       in anycase considering clients can now receive a much more accurate
 *       snapshot of the time on the server from the connection data.
 */
void Handler::doTIME(Handler *handler, User *from)
{
   // Some tables we need (would using locales be better here??)
   static char const *dayNames[7] = {
      "Sunday",
      "Monday", 
      "Tuesday", 
      "Wednesday", 
      "Thursday", 
      "Friday", 
      "Saturday"
   };
   static char const *monthNames[12] = {
      "January",
      "February", 
      "March", 
      "April",
      "May",
      "June",
      "July",
      "August",
      "September",
      "October",
      "November", 
      "December"
   };
   
   // Grab the time and rip it apart
   time_t timeNow = TO_DAEMON->getTime();
   struct tm *timeinfo;
   timeinfo = localtime(&timeNow);
   
   // Send the time
   handler->sendNumeric(TO_DAEMON->server, RPL_TIME, from,
			String::printf("%s %lu "
				       ":%s %s %d, %02d:%02d:%02d "
#ifdef HAVE_STRUCT_TM_TM_ZONE
				       "%s (%c%02d:%02d) "
#endif
				       "%d",
				       (char const *)TO_DAEMON->server->getHostname(),
				       TO_DAEMON->getTime(),
				       dayNames[timeinfo->tm_wday],
				       monthNames[timeinfo->tm_mon],
				       timeinfo->tm_mday,
				       timeinfo->tm_hour,
				       timeinfo->tm_min,
				       timeinfo->tm_sec,
#ifdef HAVE_STRUCT_TM_TM_ZONE
				       timeinfo->tm_zone,
				       ((timeinfo->tm_gmtoff > 0) ?
					'+' : '-'),
				       (int)(timeinfo->tm_gmtoff / 3600),
				       ((int)
					((timeinfo->tm_gmtoff % 3600) / 60)),
#endif
				       (timeinfo->tm_year + 1900)));
}


/* doVERSION
 * Original 24/08/01, Simon Butcher <pickle@austnet.org>
 * Note: This is messy :) Check doc/VersionChars for more info. If you are
 *       updating this, update the VersionChars file too! Don't let the
 *       documentation go stale :(
 */
void Handler::doVERSION(Handler *handler, User *from)
{
   handler->sendNumeric(TO_DAEMON->server, RPL_VERSION, from,
			String::printf(LNG_RPL_VERSION,
				       getVersion,
				       (char const *)TO_DAEMON->server->getHostname(),
				       getVersionChars));
}


/* doWHOIS
 * Original 23/08/01, Simon Butcher <pickle@austnet.org>
 * Note: Needs 'is an ircop', 'is an austnet service' etc etc added
 *       Also needs channel display code to be fixed!!
 */
void Handler::doWHOIS(Handler *handler, User *from, String const &request)
{
   // Rip out the two token sequences
   StringTokens users(request);
   
   // Run through the parameter tokens
   for (String user = users.nextToken(','); user.length();
	user = users.nextToken(',')) {
      User *u = TO_DAEMON->getUser(user);
      
      // Make sure we found that user record
      if (!u) {
	 handler->sendNumeric(TO_DAEMON->server, ERR_NOSUCHNICK, from,
			      user + LNG_ERR_NOSUCHNICK_NICK);
	 continue; 
      }
      
      // Send the user entry
      handler->sendNumeric(TO_DAEMON->server, RPL_WHOISUSER, from,
			   String::printf("%s %s %s * :%s",
					  (char const *)u->nickname,
					  (char const *)u->username,
					  (u->showVW(from) ?
					   (char const *)u->vwhostname :
					   (char const *)u->hostname),
					  (char const *)u->realname));
      
      // Send the virtual world information if we need to
      if (!u->showVW(from) && 
	  (u->modes & User::MODE_VWORLD)) {
	 handler->sendNumeric(TO_DAEMON->server, RPL_WHOISVIRT, from,
			      u->nickname + LNG_RPL_WHOISVIRT +
			      u->vwhostname);
      }
      
      // Send the channel list(s)
      String channels = "";
      for (User::channel_map_t::iterator it = u->channels.begin();
	   it != u->channels.end(); it++) {
	 // Add this channel to the list, if it is not a 'hidden' channel
	 if (!Channel::isHidden((*it).second)) {
	    ChannelMember *cm = (*it).second->getMember(u);
	    
	    // Sanity check, this should work..
	    if (cm) {
	       channels = channels + (String(((u->modes & User::MODE_DEAF) ?
					      "-" : "")) +
				      ((cm->modes & ChannelMember::MODE_OPPED) ?
				       "@" : 
				       ((cm->modes & ChannelMember::MODE_HALFOPPED) ?
					"%" :
					((cm->modes & ChannelMember::MODE_VOICED) ?
					 "+" : ""))) +
				      // channel op/voice here?!
				      (*it).second->name + " ");
	       
	       // Check if the string needs to be sent
	       if (channels.length() > 400) {
		  handler->sendNumeric(TO_DAEMON->server,
				       RPL_WHOISCHANNELS, from,
				       u->nickname + " :" + channels);
		  
		  // reset the channel list string
		  channels = "";
	       }
	    }
	 }
      }
      
      // Send the rest of the string if needs be
      if (channels.length()) {
	 handler->sendNumeric(TO_DAEMON->server, RPL_WHOISCHANNELS, from,
			      u->nickname + " :" + channels);
      }
      
      // Send the server information, if the server is not hidden
      if (!(u->server->isModeSet(Server::MODE_HIDDEN)) ||
	  User::isOper(from)) {
	 handler->sendNumeric(TO_DAEMON->server, RPL_WHOISSERVER, from,
			      u->nickname + " " + u->server->getHostname() + 
			      " :" + u->server->getDescription());
      }
      
      // If the user is away, send the away message
      if (u->awayMessage.length()) {
	 handler->sendNumeric(TO_DAEMON->server, RPL_AWAY, from,
			      u->nickname + " :" + u->awayMessage);
      }
      
      // hmm... i do not like this very much
      if (u->server == TO_DAEMON->server) {
	 handler->sendNumeric(TO_DAEMON->server, RPL_WHOISIDLE, from,
			      u->nickname +
			      String::printf(LNG_RPL_WHOISIDLE,
					     ((unsigned long)
					      difftime(TO_DAEMON->getTime(),
						       u->local->handler->connection->getLastSpoke())),
					     u->signonTime));
      }
      
      // Check if this user is an irc operator
      if (User::isOper(u)) {
#ifdef NOTIFY_PARANOID_OPERS_ON_WHOIS
	 // Send a notice to a paranoid operator if they are locally connected
	 if (u->local) {
	    String message = String::printf(LNG_NOTIFY_PARANOID_OPERS_ON_WHOIS,
					    (char const *)from->nickname,
					    (char const *)from->getAddress());
	    u->local->handler->
	      sendNotice(TO_DAEMON->server, from, message);
			 
	 }
#endif
	 handler->sendNumeric(TO_DAEMON->server, RPL_WHOISOPERATOR, from,
			      u->nickname + LNG_RPL_WHOISOPERATOR);
      }
      
      // Check if this user is connected via an SSL enabled port
      if (u->modes & User::MODE_SECURE) {
	 handler->sendNumeric(TO_DAEMON->server, RPL_WHOISSECURE, from,
			      u->nickname + " *" + LNG_RPL_WHOISSECURE);
      }
   }
   
   // Send the footer
   handler->sendNumeric(TO_DAEMON->server, RPL_ENDOFWHOIS, from,
			request + LNG_RPL_ENDOFWHOIS);
}

