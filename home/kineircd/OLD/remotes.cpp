/* remotes.cpp
 * Protocol reply routines, remote replies. Majorly based on IRC2 :(
 * 
 * NOTE: This needs to be seriously reworked! When irc3user/AN1 comes this
 * will be wrong. This MUST be broken up and reproduced in the protocol
 * handlers themselves, then protocol translation executed.
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

#include "austhex/austhexconf.h"

#include <cctype>
#include <ctime>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>

#include "austhex/handler.h"
#include "austhex/daemon.h"
#include "austhex/numerics.h"
#include "austhex/lang.h"
#include "austhex/version.h"


/* doADMIN
 * Original 27/08/01 simonb
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
			from->lang(LangTags::L_RPL_ADMINME));
   handler->sendNumeric(Daemon::myServer(), Numerics::RPL_ADMINLOC1, from,
			Daemon::getConfig().getAdminName());
   
   // Check if the location line exists (it is optional)
   if (!Daemon::getConfig().getAdminLocation().empty()) {
      handler->sendNumeric(Daemon::myServer(), Numerics::RPL_ADMINLOC2, from,
			   Daemon::getConfig().getAdminLocation());
   }

   handler->sendNumeric(Daemon::myServer(), Numerics::RPL_ADMINEMAIL, from,
			Daemon::getConfig().getAdminEmail());
}


/* doLUSERS
 * Original 13/08/01 simonb
 */
void Handler::doLUSERS(Handler *handler, User *from, String const &request)
{
   handler->
     sendNumeric(Daemon::myServer(), Numerics::RPL_LUSERCLIENT, from,
		 ':' + from->lang(LangTags::B_RPL_LUSERCLIENT) + ' ' +
		 String::convert(Daemon::numTotalUsers) + ' ' +
		 from->lang(LangTags::M_RPL_LUSERCLIENT_A) + ' ' +
		 String::convert(Daemon::numServices) + ' ' +
		 from->lang(LangTags::M_RPL_LUSERCLIENT_B) + ' ' +
		 String::convert(Daemon::numServers) + ' ' +
		 from->lang(LangTags::E_RPL_LUSERCLIENT));
   handler->
     sendNumeric(Daemon::myServer(), Numerics::RPL_LUSEROP, from,
		 String::convert(Daemon::numOpers) + " :" +
		 from->lang(LangTags::E_RPL_LUSEROP));
   handler->
     sendNumeric(Daemon::myServer(), Numerics::RPL_LUSERHELPERS, from,
		 String::convert(Daemon::numHelpers) + " :" +
		 from->lang(LangTags::E_RPL_LUSERHELPERS));
   handler->
     sendNumeric(Daemon::myServer(), Numerics::RPL_LUSERUNKNOWN, from,
		 String::convert(Daemon::numUnknown) + " :" +
		 from->lang(LangTags::E_RPL_LUSERUNKNOWN));
   handler->
     sendNumeric(Daemon::myServer(), Numerics::RPL_LUSERCHANNELS, from,
		 String::convert(Daemon::channels.size() + 
				 Daemon::localChannels.size()) + " :" +
		 from->lang(LangTags::E_RPL_LUSERCHANNELS));
   handler->
     sendNumeric(Daemon::myServer(), Numerics::RPL_LUSERME, from,
		 ':' + from->lang(LangTags::B_RPL_LUSERME) + ' ' +
		 String::convert(Daemon::numClientConns) + ' ' +
		 from->lang(LangTags::M_RPL_LUSERME) + ' ' +
		 String::convert(Daemon::numServerConns) + ' ' +
		 from->lang(LangTags::E_RPL_LUSERME));
   handler->
     sendNumeric(Daemon::myServer(), Numerics::RPL_LOCALUSERS, from,
		 ':' + from->lang(LangTags::B_RPL_LOCALUSERS) + ": " +
		 String::convert(Daemon::numClientConns) + ' ' +
		 from->lang(LangTags::W_PEAK) + ": " +
		 String::convert(Daemon::numClientConnsPeak));
   handler->
     sendNumeric(Daemon::myServer(), Numerics::RPL_GLOBALUSERS, from,
		 ':' + from->lang(LangTags::B_RPL_GLOBALUSERS) + ": " +
		 String::convert(Daemon::numTotalUsers) + ' ' +
		 from->lang(LangTags::W_PEAK) + ": " +
		 String::convert(Daemon::numTotalUsersPeak));
}


/* doMAP
 * Original 01/11/01 simonb
 */
void Handler::doMAP(Handler *handler, User *from)
{
   // Run through the server list (this needs to change, ripped from LINKS)
   for (Daemon::server_map_t::iterator it = Daemon::servers.begin();
	it != Daemon::servers.end(); it++) {
      // Send the user this record
      handler->
	sendNumeric(Daemon::myServer(), Numerics::RPL_MAP, from,
		    String::printf("x %u %u %u %.2f * * * * %s :?- %s",
				   (*it).second->getProtocol(),
				   (*it).second->getNumHops(),
				   (*it).second->getNumUsers(),
				   (((float)(*it).second->getNumUsers() / 
				     Daemon::numTotalUsers) * 100),
				   Server::makeModes((*it).second).c_str(),
				   (*it).second->getHostname().c_str()));
   }
   
   handler->sendNumeric(Daemon::myServer(), Numerics::RPL_MAPEND, from,
			":End of dodgey map command");
}


/* doMOTD
 * Original 13/08/01 simonb
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
		    ':' + from->lang(LangTags::L_ERR_NOMOTD_NOREMOTE));
      return;
   }
   
   // Check if we have an MOTD to send..
   if (!Daemon::motd.empty()) {
      // Send the header
      handler->
	sendNumeric(Daemon::myServer(), Numerics::RPL_MOTDSTART, from,
		    ":- " + Daemon::myServer()->getHostname() + ' ' +
		    from->lang(LangTags::L_RPL_MOTDSTART) + " -");

      // Run through the MOTD and send each line in the list
      for (Daemon::motd_t::iterator it = Daemon::motd.begin();
	   it != Daemon::motd.end(); 
	   it++) {
	 handler->sendNumeric(Daemon::myServer(), Numerics::RPL_MOTD, from, 
			      ":- " + *it);
      }
	 
      // Send the footer
      handler->
	sendNumeric(Daemon::myServer(), Numerics::RPL_ENDOFMOTD, from, 
		    ':' + from->lang(LangTags::L_RPL_ENDOFMOTD));
   } else {
      // No MOTD to send
      handler->
	sendNumeric(Daemon::myServer(), Numerics::ERR_NOMOTD, from,
		    ':' + from->lang(LangTags::L_ERR_NOMOTD));
   }
}


#define STATS_REPLY(name)	void name(Handler *handler, User *from)

// This is in a class so it can be friends with the ircd class
class statsFunctions {
 public:
   /* replyL - Link info (List of connections)
    * Original 26/08/01 simonb
    */
   static STATS_REPLY(replyL)
     {
	// Run through the connection list...
	for (Daemon::connection_list_t::iterator it = Daemon::connections.begin();
	     it != Daemon::connections.end(); it++) {
	   handler->
	     sendNumeric(Daemon::myServer(),
			 Numerics::RPL_STATSLINKINFO, from,
			 ((*it)->name ? *(*it)->name : "!") + ' ' +
			 String::convert((*it)->outQueue.size()) + ' ' +
			 String::convert((*it)->sentMessages) + ' ' +
			 String::convert((*it)->sentBytes / 1024) + ' ' +
			 String::convert((*it)->receivedMessages) + ' ' +
			 String::convert((*it)->receivedBytes / 1024) + ' ' +
			 String::convert(Daemon::getTime() - 
					 (*it)->connectedTime) + " :" +
			 (*it)->socket->io->getInfo());
	}
     }
   
   
   /* replyO - Server operator information ('o-lines')
    * Original 26/08/01 simonb
    * Note: Slightly modified, most clients ignore it, other clients use it :)
    */
   static STATS_REPLY(replyO)
     {
	// Run through the list of operators and send a reply per identify host
	for (Daemon::operator_map_t::iterator it = Daemon::operators.begin();
	     it != Daemon::operators.end(); it++) {
	   handler->sendNumeric(Daemon::myServer(),
				Numerics::RPL_STATSOLINE, from,
				String((*it).second->isGlobal() ? 
				       'O' : 'o') + ' ' +
				"*@*" + ' ' + // Fix this
				(*it).first + " :" +
				(*it).second->getName());
	}
     }

   
   /* replyRUSAGE - Server resource usage information
    * Original 28/08/01 simonb
    */
   static STATS_REPLY(replyRUSAGE)
     {
	struct rusage usage;
	
	// Grab usage information for ourselves
	if (!getrusage(RUSAGE_SELF, &usage)) { // RUSAGE_SELF = 0?!
	   handler->sendNumeric(Daemon::myServer(),
				Numerics::RPL_DUMMY_NUMERIC, from,
				":Memory: Shared " +
				String::convert(usage.ru_ixrss) + ", Data " +
				String::convert(usage.ru_idrss) + ", Stack " +
				String::convert(usage.ru_isrss));
	   handler->sendNumeric(Daemon::myServer(),
				Numerics::RPL_DUMMY_NUMERIC, from,
				":Page faults: Major " +
				String::convert(usage.ru_majflt) + ", Minor " +
				String::convert(usage.ru_minflt) + " (" +
				String::convert(usage.ru_nswap) + " swaps)");
	   handler->sendNumeric(Daemon::myServer(),
				Numerics::RPL_DUMMY_NUMERIC, from,
				":Blocks: In " +
				String::convert(usage.ru_inblock) + ", Out " +
				String::convert(usage.ru_oublock));
	   handler->sendNumeric(Daemon::myServer(),
				Numerics::RPL_DUMMY_NUMERIC, from,
				":Messages: Received " +
				String::convert(usage.ru_msgrcv) + ", Sent " +
				String::convert(usage.ru_msgsnd));
	   handler->sendNumeric(Daemon::myServer(),
				Numerics::RPL_DUMMY_NUMERIC, from,
				":Context Switches: Voluntary " +
				String::convert(usage.ru_nvcsw) + 
				", Involuntary " +
				String::convert(usage.ru_nivcsw) + " (" +
				String::convert(usage.ru_nsignals) + " signals)");
	} else {
	   handler->sendNumeric(Daemon::myServer(),
				Numerics::RPL_DUMMY_NUMERIC, from,
				":getrusage() failed (" + 
				String(strerror(errno)) + ')');
	}
     }
   
   
   /* replyU - Server uptime information
    * Original 26/08/01 simonb
    */
   static STATS_REPLY(replyU)
     {
        time_t uptime = (Daemon::getTime() - Daemon::startTime);

	handler->
	  sendNumeric(Daemon::myServer(),
		      Numerics::RPL_STATSUPTIME, from,
		      ':' + 
		      from->lang(LangTags::B_RPL_STATSUPTIME) + ' ' +
		      String::convert(uptime / 86400) + ' ' +
		      from->lang(LangTags::W_DAY_PL) + ' ' +
		      String::printf("%d:%02d:%02d",
				     ((int)(uptime % 86400) / 3600),
				     ((int)(uptime % 3600) / 60),
				     (int)(uptime % 60)));
	handler->
	  sendNumeric(Daemon::myServer(),
		      Numerics::RPL_STATSCONN, from,
		      ':' + from->lang(LangTags::B_RPL_STATSCONN) + ": " +
		      String::convert(Daemon::numConnsPeak) + " (" +
		      String::convert(Daemon::numClientConnsPeak) + ' ' +
		      from->lang(LangTags::W_CLIENT_PL) + ')');
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
 * Original 14/08/01 simonb
 */
void Handler::doSTATS(Handler *handler, User *from, String const &request)
{
   // Fix up the request for easy searching
   StringMask requestMask(request.toUpper());

#ifdef NOTIFY_ON_CMD_STATS
   // Send a server notice out regarding this stats request
   Daemon::
     serverNotice(String::printf((char *)LangTags::L_SERVNOTICE_NOTIFY_ON_CMD_STATS,
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
			from->lang(LangTags::L_RPL_ENDOFSTATS));
}


/* doTIME
 * Original 29/08/01 simonb
 */
#define TIMESTR_BUF_LEN 64
void Handler::doTIME(Handler *handler, User *from)
{
   // Send time on server data (more accurate)
   handler->
     sendNumeric(Daemon::myServer(), Numerics::RPL_TIMEONSERVERIS, from,
		 String::convert(Daemon::getTime()) + ' ' + 
		 String::convert(Daemon::getTimeUsecs()) + ' ' +
		 Daemon::getTimeZone() + ' ' + Daemon::getTimeFlags() + 
		 " :" + from->lang(LangTags::B_RPL_TIMEONSERVERIS));
   
   // Compile the time string (human readable)
   char timestr[TIMESTR_BUF_LEN];
//   time_t timenow = time(NULL); // Should we use the Daemon::getTime() instead?
   time_t timenow = Daemon::getTime();
   struct tm *ltime = localtime(&timenow);
   strftime(timestr, TIMESTR_BUF_LEN, "%a, %_d %b %Y %H:%M:%S %Z", ltime);
     
   // Send the time string
   handler->sendNumeric(Daemon::myServer(), Numerics::RPL_TIME, from,
			':' + timestr);
}


/* doVERSION
 * Original 24/08/01 simonb
 * 24/10/01 simonb - Added ISUPPORT output
 */
void Handler::doVERSION(Handler *handler, User *from)
{
   handler->
     sendNumeric(Daemon::myServer(), Numerics::RPL_VERSION, from,
		 Version::version + ' ' + Daemon::myServer()->getHostname() +
		 " :" + Version::versionChars + " (" +
		 from->lang(LangTags::L_RPL_VERSION) + ')');
   handler->
     sendNumeric(Daemon::myServer(), Numerics::RPL_ISUPPORT, from,
		 Daemon::makeISUPPORT() + " :" + 
		 from->lang(LangTags::E_RPL_ISUPPORT));
}


/* doWHOIS
 * Original 23/08/01 simonb
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
			      from->lang(LangTags::L_ERR_NOSUCHNICK_NICK));
	 continue; 
      }
      
      // Send the user entry
      handler->sendNumeric(Daemon::myServer(), Numerics::RPL_WHOISUSER, from,
			   u->nickname + ' ' + u->username + ' ' +
			   (u->showVW(from) ?
			    u->vwhostname : u->hostname) + " * :" +
			   u->realname);
      
      // Send the virtual world information if we need to
      if (!u->showVW(from) && 
	  (u->modes & User::M_VWORLD)) {
	 handler->
	   sendNumeric(Daemon::myServer(), Numerics::RPL_WHOISVIRT, from,
		       u->nickname + " :" + 
		       from->lang(LangTags::P_RPL_WHOISVIRT) + ' ' +
		       u->vwhostname);
      }
      
      // Send the channel list(s)
      String channels;
      for (User::channel_map_t::iterator it = u->channels.begin();
	   it != u->channels.end(); it++) {
	 // Add this channel to the list, if it is not a 'hidden' channel
	 if (!(*it).second->isHidden()) {
	    ChannelMember *cm = (*it).second->getMember(u);
	    
	    // Sanity check, this should work..
	    if (cm) {
	       channels = channels + ((u->modes & User::M_DEAF) ?
				      "-" : "") +
				      (cm->isModeSet(ChannelMember::M_OPPED) ?
				       "@" : 
				       (cm->isModeSet(ChannelMember::M_HALFOPPED) ?
					"%" :
					(cm->isModeSet(ChannelMember::M_VOICED) ?
					 "+" : "")) +
				      // channel op/voice here?!
				      (*it).second->name + ' ');
	       
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
			      u->getNickname() + ' ' + u->getLangList() + ' ' +
			      u->getLangCharset());
      }
      
      // Check if this user is a helper
//      if (User::isHelper(u)/* && u->getStatus()*/) {
//	 handler->sendNumeric(Daemon::myServer(), Numerics::RPL_WHOISHELPER,
//			      from, u->getNickname() + " :" +
//			      from->lang((LangTags::E_HELPER_01 - 1) +
//					 u->getStatus()));
//      }
      
      // Check if this user is an irc operator
      if (User::isOper(u)) {
#ifdef NOTIFY_PARANOID_OPERS_ON_WHOIS
	 // Send a notice to a paranoid operator, sheesh
	 Daemon::routeTo(u)->
	   sendNotice(Daemon::myServer(), from,
		      'w' + from->nickname);
#endif
	 handler->sendNumeric(Daemon::myServer(), Numerics::RPL_WHOISOPERATOR, from,
			      u->nickname + " :" +
			      from->lang(LangTags::E_RPL_WHOISOPERATOR));
      }
      
      // Check if this user is connected via an SSL enabled port
      if (u->modes & User::M_SECURE) {
	 handler->
	   sendNumeric(Daemon::myServer(), Numerics::RPL_WHOISSECURE, from,
		       u->nickname + " * :" +
		       from->lang(LangTags::E_RPL_WHOISSECURE));
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
		       u->nickname + ' ' +
		       String::convert(Daemon::getTime() -
				       u->local->handler->getConnection()->getLastSpoke()) +
		       ' ' + String::convert(u->signonTime) + " :" +
		       from->lang(LangTags::E_RPL_WHOISIDLE));
      }

      /* Send the server information, if the server is not hidden and the
       * user is visible or if the user looking at the information is an
       * operator
       */
      if ((!(u->server->isModeSet(Server::M_HIDDEN)) &&
	   !(u->isModeSet(User::M_INVISIBLE))) ||
	  User::isOper(from)) {
	 handler->sendNumeric(Daemon::myServer(), Numerics::RPL_WHOISSERVER, 
			      from,
			      u->nickname + ' ' + u->server->getHostname() + 
			      " :" + u->server->getDescription());
      }
   }
   
   // Send the footer
   handler->sendNumeric(Daemon::myServer(), Numerics::RPL_ENDOFWHOIS, from,
			request + " :" + 
			from->lang(LangTags::L_RPL_ENDOFWHOIS));
}

