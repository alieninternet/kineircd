/* irc2user.h
 * The handler class for the IRC-2 user protocol
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

#ifdef HAVE_IRC2USER_PROTOCOL
# ifndef __IRC2USER_H__
#  define __IRC2USER_H__

#  include "handler.h"
#  include "lang.h"

// IRC-2 User-handler sub-class
class irc2userHandler : public Handler {
 private:
#  ifdef FLOODLOCK_AWAY_SET
   time_t lastAwaySet;				// Time the away msg was set
   bool lastAwaySetGrace;			// Grace to allow an extra set
#  endif
   
   User *user;					// Link to the user data

   struct functionTableStruct {
      char const *command;			// Command name
      void (*function)(irc2userHandler *handler, 
		       StringTokens *tokens);	// Parsing handler function
      unsigned char const penalty;		// Calling penalty amount
      bool const (*access)(User *u);		// Access checking function
      char const *parameters;			// Parameter string for HELP
      char const *help;				// Plain-text line for HELP
   }; 
   static struct functionTableStruct const functionsTable[];

   String processUserModes(String &, StringTokens *,
			   bool = false);	// Process a usermode request
   
   void sendGoodbye(String const &) const;	// Closing link message
   void sendNumeric(Numerics::numeric_t, String const &) 
     const;					// Send numeric, breif
   void sendPong(String const &) const;		// PING reply
   void sendWhoReply(User *, Channel *,
		     ChannelMember *) const;	// WHO reply

   // Helpers
   void doNAMES(String const &);		// Do a NAMES request
   String lang(LangTags::tag_t const &t) const;	// Pull out a language string

   
   // Protocol input
   static void parseACCEPT(irc2userHandler *handler, StringTokens *tokens);
   static void parseADMIN(irc2userHandler *handler, StringTokens *tokens);
   static void parseAWAY(irc2userHandler *handler, StringTokens *tokens);
   static void parseCONNECT(irc2userHandler *handler, StringTokens *tokens);
#  ifdef ALLOW_COMMAND_DIE
   static void parseDIE(irc2userHandler *handler, StringTokens *tokens);
#  endif
   static void parseGLOBOPS(irc2userHandler *handler, StringTokens *tokens);
   static void parseHELP(irc2userHandler *handler, StringTokens *tokens);
   static void parseHELPME(irc2userHandler *handler, StringTokens *tokens);
   static void parseINFO(irc2userHandler *handler, StringTokens *tokens);
   static void parseINVITE(irc2userHandler *handler, StringTokens *tokens);
   static void parseISON(irc2userHandler *handler, StringTokens *tokens);
   static void parseJOIN(irc2userHandler *handler, StringTokens *tokens);
   static void parseKICK(irc2userHandler *handler, StringTokens *tokens);
   static void parseKILL(irc2userHandler *handler, StringTokens *tokens);
   static void parseKNOCK(irc2userHandler *handler, StringTokens *tokens);
   static void parseLANGUAGE(irc2userHandler *handler, StringTokens *tokens);
   static void parseLINKS(irc2userHandler *handler, StringTokens *tokens);
   static void parseLIST(irc2userHandler *handler, StringTokens *tokens);
   static void parseLOCOPS(irc2userHandler *handler, StringTokens *tokens);
   static void parseLUSERS(irc2userHandler *handler, StringTokens *tokens);
   static void parseMAP(irc2userHandler *handler, StringTokens *tokens);
   static void parseMODE(irc2userHandler *handler, StringTokens *tokens);
   static void parseMOTD(irc2userHandler *handler, StringTokens *tokens);
   static void parseNAMES(irc2userHandler *handler, StringTokens *tokens);
   static void parseNICK(irc2userHandler *handler, StringTokens *tokens);
   static void parseNOTICE(irc2userHandler *handler, StringTokens *tokens);
   static void parseOPER(irc2userHandler *handler, StringTokens *tokens);
   static void parsePART(irc2userHandler *handler, StringTokens *tokens);
   static void parsePING(irc2userHandler *handler, StringTokens *tokens);
   static void parsePONG(irc2userHandler *handler, StringTokens *tokens);
   static void parsePRIVMSG(irc2userHandler *handler, StringTokens *tokens);
   static void parseQUIT(irc2userHandler *handler, StringTokens *tokens);
   static void parseREHASH(irc2userHandler *handler, StringTokens *tokens);
   static void parseRESTART(irc2userHandler *handler, StringTokens *tokens);
   static void parseSERVLIST(irc2userHandler *handler, StringTokens *tokens);
   static void parseSILENCE(irc2userHandler *handler, StringTokens *tokens);
   static void parseSQUIT(irc2userHandler *handler, StringTokens *tokens);
   static void parseSTATS(irc2userHandler *handler, StringTokens *tokens);
   static void parseTIME(irc2userHandler *handler, StringTokens *tokens);
   static void parseTOPIC(irc2userHandler *handler, StringTokens *tokens);
   static void parseTRACE(irc2userHandler *handler, StringTokens *tokens);
#  ifdef HAVE_CMD_TRACEROUTE
   static void parseTRACEROUTE(irc2userHandler *handler, StringTokens *tokens);
#  endif
   static void parseUSERHOST(irc2userHandler *handler, StringTokens *tokens);
   static void parseUSERIP(irc2userHandler *handler, StringTokens *tokens);
   static void parseVERSION(irc2userHandler *handler, StringTokens *tokens);
   static void parseWATCH(irc2userHandler *handler, StringTokens *tokens);
   static void parseWALLCHOPS(irc2userHandler *handler, StringTokens *tokens);
#  ifdef ALLOW_OPER_WALLOPS
   static void parseWALLOPS(irc2userHandler *handler, StringTokens *tokens);
#  endif
   static void parseWHO(irc2userHandler *handler, StringTokens *tokens);
   static void parseWHOIS(irc2userHandler *handler, StringTokens *tokens);
   static void parseWHOWAS(irc2userHandler *handler, StringTokens *tokens);

 public:
   irc2userHandler(Connection *, User *, String);	// Constructor
   ~irc2userHandler(void);				// Destructor

   // Operations
   void goodbye(String const &);
   void kill(String const &, String const &);
   
   // Protocol output routines...
   void sendInvite(User *, Channel *, User *, time_t) const;
   void sendJoin(Channel *, User *) const;
   void sendChannelMode(Channel *, User *, String const &) const;
   void sendKick(Channel *, User *, User *, String const &) const;
   void sendKill(User *, String const &, String const &) const;
   void sendKnock(User *, Channel *, String const &) const;
   void sendLanguage(User *, String const &, String const &) const;
   void sendNickChange(User *, String const &) const;
   void sendNotice(Server *, User *, String const &) const;
   void sendNotice(User *, Channel *, String const &) const;
   void sendNotice(User *, User *, String const &) const;
   void sendNotice(User *, StringMask const &, String const &) const;
   void sendNoticeAnon(Channel *, String const &) const;
   void sendNumeric(Server *, Numerics::numeric_t, User *, 
		    String const &) const;
   void sendPart(Channel *, User *, String const &) const;
   void sendPing(void) const;
   void sendPrivmsg(Server *, User *, String const &) const;
   void sendPrivmsg(User *, Channel *, String const &) const;
   void sendPrivmsg(User *, User *, String const &) const;
   void sendPrivmsg(User *, StringMask const &, String const &) const;
   void sendPrivmsgAnon(Channel *, String const &) const;
   void sendQuit(User *, String const &) const;
   void sendServerMode(Server *, Server *, String const &) const;
   void sendServerMode(Server *, User *, String const &) const;
   void sendSilence(User *, bool, StringMask const &) const;
   void sendUserMode(User *, String const &) const;
   void sendTopic(Channel *, Server *, String const &) const;
   void sendTopic(Channel *, User *, String const &) const;
   void sendWallops(Server *, String const &) const;
   void sendWallops(User *, String const &) const;
   void sendWatchOff(Server *) const;
   void sendWatchOff(Channel *) const;
   void sendWatchOff(User *) const;
   void sendWatchOn(Server *) const;
   void sendWatchOn(User *, String const & = "") const;
   void sendWatchOn(Channel *, String const &) const;
   
   // Protocol input
   void parseLine(String const &);
};

# endif
#endif

