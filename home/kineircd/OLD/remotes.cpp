/* remotes.cpp
 * Protocol reply routines, remote replies.
 * 
 * NOTE: This needs to be seriously reworked! If/When irc3user comes half of
 * this may (most likely) become obsolete - a more generic approach must be
 * taken!
 */

#include "autoconf.h"

#include <ctype.h>
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "handler.h"
#include "daemon.h"
#include "numerics.h"
#include "language.h"
#include "version.h"


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
   handler->sendNumeric(Daemon::myServer(), Numerics::RPL_ADMINME, from, 
			Daemon::myServer()->getHostname() + " :" +
			from->lang(Language::L_RPL_ADMINME));
   handler->sendNumeric(Daemon::myServer(), Numerics::RPL_ADMINLOC1, from,
			Daemon::adminName);
   
   // The second location line is optional...
   if (Daemon::adminLocation.length()) {
      handler->sendNumeric(Daemon::myServer(), Numerics::RPL_ADMINLOC2, from,
			   Daemon::adminLocation);
   }
   
   handler->sendNumeric(Daemon::myServer(), Numerics::RPL_ADMINEMAIL, from,
			Daemon::adminEmail);
}


/* doLUSERS
 * Original 13/08/01, Simon Butcher <pickle@austnet.org>
 */
void Handler::doLUSERS(Handler *handler, User *from, String const &request)
{
   handler->
     sendNumeric(Daemon::myServer(), Numerics::RPL_LUSERCLIENT, from,
		 String::printf(":%s %u %s %u %s %u %s",
				(char const *)from->lang(Language::B_RPL_LUSERCLIENT),
				Daemon::numTotalUsers,
				(char const *)from->lang(Language::M_RPL_LUSERCLIENT_A),
				Daemon::numServices,
				(char const *)from->lang(Language::M_RPL_LUSERCLIENT_B),
				Daemon::numServers,
				(char const *)from->lang(Language::E_RPL_LUSERCLIENT)));
   handler->
     sendNumeric(Daemon::myServer(), Numerics::RPL_LUSEROP, from,
		 String::printf("%u :%s",
				Daemon::numOpers,
				(char const *)from->lang(Language::E_RPL_LUSEROP)));
   handler->
     sendNumeric(Daemon::myServer(), Numerics::RPL_LUSERHELPERS, from,
		 String::printf("%u :%s",
				Daemon::numHelpers,
				(char const *)from->lang(Language::E_RPL_LUSERHELPERS)));
   handler->
     sendNumeric(Daemon::myServer(), Numerics::RPL_LUSERUNKNOWN, from,
		 String::printf("%u :%s",
				Daemon::numUnknown,
				(char const *)from->lang(Language::E_RPL_LUSERUNKNOWN)));
   handler->
     sendNumeric(Daemon::myServer(), Numerics::RPL_LUSERCHANNELS, from,
		 String::printf("%u :%s",
				(Daemon::channels.size() +
				 Daemon::localChannels.size()),
				(char const *)from->lang(Language::E_RPL_LUSERCHANNELS)));
   handler->
     sendNumeric(Daemon::myServer(), Numerics::RPL_LUSERME, from,
		 String::printf(":%s %u %s %u %s",
				(char const *)from->lang(Language::B_RPL_LUSERME),
				Daemon::numClientConns,
				(char const *)from->lang(Language::M_RPL_LUSERME),
				Daemon::numServerConns,
				(char const *)from->lang(Language::E_RPL_LUSERME)));
   handler->
     sendNumeric(Daemon::myServer(), Numerics::RPL_LOCALUSERS, from,
		 String::printf(":%s: %u %s: %u",
				(char const *)from->lang(Language::B_RPL_LOCALUSERS),
				Daemon::numClientConns,
				(char const *)from->lang(Language::W_PEAK),
				Daemon::numClientConnsPeak));
   handler->
     sendNumeric(Daemon::myServer(), Numerics::RPL_GLOBALUSERS, from,
		 String::printf(":%s: %u %s: %u",
				(char const *)from->lang(Language::B_RPL_GLOBALUSERS),
				Daemon::numTotalUsers,
				(char const *)from->lang(Language::W_PEAK),
				Daemon::numTotalUsersPeak));
}


/* doMAP
 * Original 01/11/01, Simon Butcher <pickle@austnet.org>
 */
void Handler::doMAP(Handler *handler, User *from)
{
   // Run through the server list (this needs to change, ripped from LINKS)
   for (Daemon::server_map_t::iterator it = Daemon::servers.begin();
	it != Daemon::servers.end(); it++) {
      // Send the user this record
      handler->
	sendNumeric(Daemon::myServer(), Numerics::RPL_MAP, from,
		    String::printf("x %u %u %u %.3f * * * * +ModesGoHere :?- %s",
				   (*it).second->getProtocol(),
				   (*it).second->getNumHops(),
				   (*it).second->getNumUsers(),
				   (((float)(*it).second->getNumUsers() / 
				     Daemon::numTotalUsers) * 100),
				   (char const *)(*it).second->getHostname()));
   }
   
   handler->sendNumeric(Daemon::myServer(), Numerics::RPL_MAPEND, from,
			":End of dodgey map command");
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
   if ((from->server != Daemon::myServer()) &&
       !(Daemon::myServer()->isModeSet(Server::M_REMOTEMOTD))) {
      // Not allowed to send the motd over the network
      handler->
	sendNumeric(Daemon::myServer(), Numerics::ERR_NOMOTD, from,
		    String(':') + from->lang(Language::L_ERR_NOMOTD_NOREMOTE));
      return;
   }
   
   // Check if we have an MOTD to send..
   if (!Daemon::motd.empty()) {
      // Send the header
      handler->
	sendNumeric(Daemon::myServer(), Numerics::RPL_MOTDSTART, from,
		    String::printf(":- %s %s -",
				   (char const *)Daemon::myServer()->getHostname(),
				   (char const *)from->lang(Language::L_RPL_MOTDSTART)));

      // Run through the MOTD and send each line in the list
      for (Daemon::motd_t::iterator it = Daemon::motd.begin();
	   it != Daemon::motd.end(); 
	   it++) {
	 handler->sendNumeric(Daemon::myServer(), Numerics::RPL_MOTD, from, 
			      String(":- ") + *it);
      }
	 
      // Send the footer
      handler->
	sendNumeric(Daemon::myServer(), Numerics::RPL_ENDOFMOTD, from, 
		    String(':') + from->lang(Language::L_RPL_ENDOFMOTD));
   } else {
      // No MOTD to send
      handler->
	sendNumeric(Daemon::myServer(), Numerics::ERR_NOMOTD, from,
		    String(':') + from->lang(Language::L_ERR_NOMOTD));
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
	for (Daemon::connection_list_t::iterator it = Daemon::connections.begin();
	     it != Daemon::connections.end(); it++) {
	   handler->sendNumeric(Daemon::myServer(),
				Numerics::RPL_STATSLINKINFO, from,
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
						(Daemon::getTime() -
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
	for (Daemon::operator_map_t::iterator it = Daemon::operators.begin();
	     it != Daemon::operators.end(); it++) {
	   handler->sendNumeric(Daemon::myServer(),
				Numerics::RPL_STATSOLINE, from,
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
	   handler->sendNumeric(Daemon::myServer(),
				Numerics::RPL_DUMMY_NUMERIC, from,
				String::printf(":Memory: Shared %ld, Data %ld, Stack %ld",
					       usage.ru_ixrss,
					       usage.ru_idrss,
					       usage.ru_isrss));
	   handler->sendNumeric(Daemon::myServer(),
				Numerics::RPL_DUMMY_NUMERIC, from,
				String::printf(":Page faults: Major %ld, Minor %ld (%ld swaps)",
					       usage.ru_majflt,
					       usage.ru_minflt,
					       usage.ru_nswap));
	   handler->sendNumeric(Daemon::myServer(),
				Numerics::RPL_DUMMY_NUMERIC, from,
				String::printf(":Blocks: In %ld, Out %ld",
					       usage.ru_inblock,
					       usage.ru_oublock));
	   handler->sendNumeric(Daemon::myServer(),
				Numerics::RPL_DUMMY_NUMERIC, from,
				String::printf(":Messages: Received %ld, Sent %ld",
					       usage.ru_msgrcv,
					       usage.ru_msgsnd));
	   handler->sendNumeric(Daemon::myServer(),
				Numerics::RPL_DUMMY_NUMERIC, from,
				String::printf(":Context Switches: Voluntary %ld, Involuntary %ld (%ld signals)",
					       usage.ru_nvcsw,
					       usage.ru_nivcsw,
					       usage.ru_nsignals));
	} else {
	   handler->sendNumeric(Daemon::myServer(),
				Numerics::RPL_DUMMY_NUMERIC, from,
				String::printf(":getrusage() failed (%s)",
					       strerror(errno)));
	}
     }
   
   
   /* replyU - Server uptime information
    * Original 26/08/01, Simon Butcher <pickle@austnet.org>
    */
   static STATS_REPLY(replyU)
     {
        time_t uptime = (Daemon::getTime() - Daemon::startTime);

	handler->
	  sendNumeric(Daemon::myServer(),
		      Numerics::RPL_STATSUPTIME, from,
		      String::printf(":%s %d %s %d:%02d:%02d",
				     (char const *)from->lang(Language::B_RPL_STATSUPTIME),
				     (int)(uptime / 86400),
				     (char const *)from->lang(Language::W_DAY_PL),
				     ((int)(uptime % 86400) / 3600),
				     ((int)(uptime % 3600) / 60),
				     (int)(uptime % 60)));
	handler->
	  sendNumeric(Daemon::myServer(),
		      Numerics::RPL_STATSCONN, from,
		      String::printf(":%s: %u (%u %s)",
				     (char *)Language::B_RPL_STATSCONN,
				     Daemon::numConnsPeak,
				     Daemon::numClientConnsPeak,
				     (char const *)from->lang(Language::W_CLIENT_PL)));
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
   Daemon::
     serverNotice(String::printf((char *)Language::L_SERVNOTICE_NOTIFY_ON_CMD_STATS,
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
   handler->sendNumeric(Daemon::myServer(), Numerics::RPL_ENDOFSTATS, from, 
			request + " :" +
			from->lang(Language::L_RPL_ENDOFSTATS));
}


/* doTIME
 * Original 29/08/01, Simon Butcher <pickle@austnet.org>
 */
#define TIMESTR_BUF_LEN 64
void Handler::doTIME(Handler *handler, User *from)
{
   // Send time on server data (more accurate)
   handler->
     sendNumeric(Daemon::myServer(), Numerics::RPL_TIMEONSERVERIS, from,
		 String::printf("%lu %ld %s %s :%s ...fix-me...",
				Daemon::getTime(),
				Daemon::getTimeUsecs(),
				(char const *)Daemon::getTimeZone(),
				(char const *)Daemon::getTimeFlags(),
				(char const *)from->lang(Language::B_RPL_TIMEONSERVERIS)));
   
   // Compile the time string (human readable)
   char timestr[TIMESTR_BUF_LEN];
   time_t timenow = time(NULL); // Should we use the Daemon::getTime() instead?
   struct tm *ltime = localtime(&timenow);
   strftime(timestr, TIMESTR_BUF_LEN, "%a, %_d %b %Y %H:%M:%S %Z", ltime);
     
   // Send the time string
   handler->sendNumeric(Daemon::myServer(), Numerics::RPL_TIME, from,
			String::printf(":%s", timestr));
}


/* doVERSION
 * Original 24/08/01, Simon Butcher <pickle@austnet.org>
 * 24/10/01 simonb - Added ISUPPORT output
 */
void Handler::doVERSION(Handler *handler, User *from)
{
   handler->
     sendNumeric(Daemon::myServer(), Numerics::RPL_VERSION, from,
		 String::printf("%s %s :%s (%s)",
				Version::version,
				(char const *)Daemon::myServer()->getHostname(),
				Version::versionChars,
				(char const *)from->lang(Language::L_RPL_VERSION)));
   handler->sendNumeric(Daemon::myServer(), Numerics::RPL_ISUPPORT, from,
			Daemon::makeISUPPORT() + " :" + 
			from->lang(Language::E_RPL_ISUPPORT));
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
      User *u = Daemon::getUser(user);
      
      // Make sure we found that user record
      if (!u) {
	 handler->sendNumeric(Daemon::myServer(), Numerics::ERR_NOSUCHNICK, from,
			      user + " :" +
			      from->lang(Language::L_ERR_NOSUCHNICK_NICK));
	 continue; 
      }
      
      // Send the user entry
      handler->sendNumeric(Daemon::myServer(), Numerics::RPL_WHOISUSER, from,
			   String::printf("%s %s %s * :%s",
					  (char const *)u->nickname,
					  (char const *)u->username,
					  (u->showVW(from) ?
					   (char const *)u->vwhostname :
					   (char const *)u->hostname),
					  (char const *)u->realname));
      
      // Send the virtual world information if we need to
      if (!u->showVW(from) && 
	  (u->modes & User::M_VWORLD)) {
	 handler->
	   sendNumeric(Daemon::myServer(), Numerics::RPL_WHOISVIRT, from,
		       String::printf("%s :%s %s",
				      (char const *)u->nickname,
				      (char const *)from->lang(Language::P_RPL_WHOISVIRT),
				      (char const *)u->vwhostname));
      }
      
      // Send the channel list(s)
      String channels = "";
      for (User::channel_map_t::iterator it = u->channels.begin();
	   it != u->channels.end(); it++) {
	 // Add this channel to the list, if it is not a 'hidden' channel
	 if (!(*it).second->isHidden()) {
	    ChannelMember *cm = (*it).second->getMember(u);
	    
	    // Sanity check, this should work..
	    if (cm) {
	       channels = channels + (String(((u->modes & User::M_DEAF) ?
					      "-" : "")) +
				      (cm->isModeSet(ChannelMember::M_OPPED) ?
				       "@" : 
				       (cm->isModeSet(ChannelMember::M_HALFOPPED) ?
					"%" :
					(cm->isModeSet(ChannelMember::M_VOICED) ?
					 "+" : ""))) +
				      // channel op/voice here?!
				      (*it).second->name + " ");
	       
	       // Check if the string needs to be sent
	       if (channels.length() > 400) {
		  handler->sendNumeric(Daemon::myServer(),
				       Numerics::RPL_WHOISCHANNELS, from,
				       u->nickname + " :" + channels);
		  
		  // reset the channel list string
		  channels = "";
	       }
	    }
	 }
      }
      
      // Send the rest of the string if needs be
      if (channels.length()) {
	 handler->sendNumeric(Daemon::myServer(), Numerics::RPL_WHOISCHANNELS, 
			      from, u->nickname + " :" + channels);
      }
      
      // Send language specification
      if (u->hasLangInfo()) {
	 handler->sendNumeric(Daemon::myServer(), Numerics::RPL_WHOIS_LANGUAGE, 
			      from,
			      u->getNickname() + " " + u->getLangList() + " " +
			      u->getLangCharset());
      }
      
      // Check if this user is a helper
//      if (User::isHelper(u)/* && u->getStatus()*/) {
//	 handler->sendNumeric(Daemon::myServer(), Numerics::RPL_WHOISHELPER,
//			      from, u->getNickname() + " :" +
//			      from->lang((Language::E_HELPER_01 - 1) +
//					 u->getStatus()));
//      }
      
      // Check if this user is an irc operator
      if (User::isOper(u)) {
#ifdef NOTIFY_PARANOID_OPERS_ON_WHOIS
	 // Send a notice to a paranoid operator, sheesh
	 Daemon::routeTo(u)->
	   sendNotice(Daemon::myServer(), from,
		      String::printf(Language::L_NOTIFY_PARANOID_OPERS_ON_WHOIS,
				     (char const *)from->nickname,
				     (char const *)from->getAddress()));
#endif
	 handler->sendNumeric(Daemon::myServer(), Numerics::RPL_WHOISOPERATOR, from,
			      u->nickname + " :" +
			      from->lang(Language::E_RPL_WHOISOPERATOR));
      }
      
      // Check if this user is connected via an SSL enabled port
      if (u->modes & User::M_SECURE) {
	 handler->
	   sendNumeric(Daemon::myServer(), Numerics::RPL_WHOISSECURE, from,
		       String::printf("%s * :%s",
				      (char const *)u->nickname,
				      (char const *)from->lang(Language::E_RPL_WHOISSECURE)));
      }

      // If the user is away, send the away message
      if (u->awayMessage.length()) {
	 handler->sendNumeric(Daemon::myServer(), Numerics::RPL_AWAY, from,
			      u->nickname + " :" + u->awayMessage);
      }
      
      // Check if this user is local, and whether this user can see idle info
      if ((u->server == Daemon::myServer()) &&
#ifdef TIME_TO_BE_IDLE
	  (difftime(Daemon::getTime(),
		    u->local->handler->getConnection()->getLastSpoke()) >=
	   TIME_TO_BE_IDLE) &&
#endif
	  (!u->isModeSet(User::M_INVISIBLE) || User::isHelper(from))) {
	 handler->
	   sendNumeric(Daemon::myServer(), Numerics::RPL_WHOISIDLE, from,
		       String::printf("%s %lu %lu :%s",
				      (char const *)u->nickname,
				      ((unsigned long)
				       difftime(Daemon::getTime(),
						u->local->handler->getConnection()->getLastSpoke())),
				      u->signonTime,
				      (char const *)from->lang(Language::E_RPL_WHOISIDLE)));
      }

      /* Send the server information, if the server is not hidden and the
       * user is visible or if the user looking at the information is an
       * operator
       */
      if ((!(u->server->isModeSet(Server::M_HIDDEN)) &&
	   !(u->isModeSet(User::M_INVISIBLE))) ||
	  User::isOper(from)) {
	 handler->sendNumeric(Daemon::myServer(), Numerics::RPL_WHOISSERVER, from,
			      u->nickname + " " + u->server->getHostname() + 
			      " :" + u->server->getDescription());
      }
   }
   
   // Send the footer
   handler->sendNumeric(Daemon::myServer(), Numerics::RPL_ENDOFWHOIS, from,
			request + " :" +
			from->lang(Language::L_RPL_ENDOFWHOIS));
}

