/* modes.cpp
 * Server/Channel/User mode tables and toggling routines
 */

#include "config.h"

#include "modes.h"
#include "language.h"
#include "numerics.h"
#include "channel.h"
#include "utils.h"


/*
 * Woah, slow down there! If you add or delete or comment out a mode, make
 * sure you edit the defines in include/config.h so that the RPL_MYINFO and
 * RPL_ISUPPORT lines are in sync!!
 */


// Channel modes
char const *channelModeStr 		= "abdeEhiIklmnoOpqrRstv";
char const *channelModeParamStr 	= "bdehIkloOv";
char const *channelPrefixes		= "(ohv)@%+";
channelModeTableStruct channelModeTable[] = {
     { 'a',	CHANMODE_ANONYMOUS,	false,	false,	true,
	toggleChanmodeANONYMOUS 
     },
     { 'b',	0,			true,	true,	true,
	toggleChanmodeBAN 
     },
     { 'd',	0,			true,	true,	true,
	toggleChanmodeDENY 
     },
     { 'e',	0,			true,	true,	true,
	toggleChanmodeBANEXCEPT 
     },
     { 'E',	CHANMODE_EVENT,		false,	false,	true,
	toggleChanmodeEVENT
     },
     { 'h',	0,			true,	true,	true,
	toggleChanmodeHALFOPER
     },
     { 'i',	CHANMODE_INVITE,	false,	false,	true,
	toggleChanmodeINVITE
     },
     { 'I',	0,			true,	true,	true,
	toggleChanmodeINVITEALLOW
     },
     { 'k',	0,			true,	true,	true,
	toggleChanmodeKEY
     },
     { 'l',	0,			true,	false,	true,
	toggleChanmodeLIMIT
     },
     { 'm',	CHANMODE_MODERATED,	false,	false,	true,
	toggleChanmodeMODERATED
     },
     { 'n',	CHANMODE_NOOUTSIDEMSG,	false,	false,	true,
	toggleChanmodeNOOUTSIDEMSG
     },
     { 'o',	0,			true,	true,	true,
	toggleChanmodeCHANOPER
     },
     { 'O',	0,			true,	true,	false,
	toggleChanmodeCHANCREATOR
     },
     { 'p',	CHANMODE_PRIVATE,	false,	false,	true,
	toggleChanmodePRIVATE
     },
     { 'q',	CHANMODE_QUIET,		false,	false,	false,
	toggleChanmodeQUIET
     },
     { 'r',	CHANMODE_REOP,		false,	false,	false,
	toggleChanmodeREOP
     },
     { 'R',	CHANMODE_REGNICKSONLY,	false,	false,	true,
	toggleChanmodeREGNICKSONLY
     },
     { 's',	CHANMODE_SECRET,	false,	false,	true,
	toggleChanmodeSECRET
     },
     { 't',	CHANMODE_TOPICLOCK,	false,	false,	true,
	toggleChanmodeTOPICLOCK
     },
     { 'v',	0,			true,	true,	true,
	toggleChanmodeVOICE
     },
     { 0 }
};


// User modes
char const *userModeStr 		= "dghkiInoOrRsSvw";
char const *userModeParamStr 		= "s";
userModeTableStruct userModeTable[] = {
     { 'd',	USERMODE_DEAF,		false,	false,	true,
	toggleUsermodeDEAF
     },
     { 'g',	USERMODE_IGNORING,	false,	false,	false,
	toggleUsermodeIGNORING
     },
     { 'h',	USERMODE_HELPER,	false,	false,	true,
	toggleUsermodeHELPER
     },
     { 'k',	USERMODE_NONKICKABLE,	false,	false,	false,
	toggleUsermodeNONKICKABLE
     },
     { 'i',	USERMODE_INVISIBLE,	false,	false,	true,
	toggleUsermodeINVISIBLE
     },
     { 'I',	USERMODE_IDENTIFIED,	false,	false,	false,
	toggleUsermodeIDENTIFIED
     },
     { 'n',	USERMODE_GOT_IDENTD,	false,	false,	false,
	toggleUsermodeGOT_IDENTD
     },
     { 'o',	USERMODE_LOCALOPER,	false,	false,	true,
	toggleUsermodeOPER
     },
     { 'O',	USERMODE_GLOBALOPER,	false,	false,	true,
	toggleUsermodeOPER
     },
     { 'r',	USERMODE_RESTRICTED,	false,	false,	false,
	toggleUsermodeRESTRICTED
     },
     { 'R',	USERMODE_REGNICKSMSG,	false,	false,	true,
	toggleUsermodeREGNICKSMSG
     },
     { 's',	0,			true,	false,	true,
	toggleUsermodeSERVNOTICES
     },
     { 'S',	USERMODE_SECURE,	false,	false,	false,
	toggleUsermodeSECURE
     },
     { 'v',	USERMODE_VWORLD,	false,	false,	true,
	toggleUsermodeVWORLD
     },
     { 'w',	USERMODE_WALLOPS,	false,	false,	true,
	toggleUsermodeWALLOPS
     },
     { 0 }
};


// Server modes
char const *serverModeStr		= "aFhLmMn";
char const *serverModeParamStr		= "*";
serverModeTableStruct serverModeTable[] = {
     { 'a',	SERVERMODE_AUTOTBS,	false,	false,	true,
	toggleServermodeAUTOTBS
     },
     { 'F',	SERVERMODE_USERSFULL,	false,	false,	false,
	toggleServermodeUSERSFULL
     },
     { 'h',	SERVERMODE_HIDDEN,	false,	false,	true,
	toggleServermodeHIDDEN
     },
     { 'L',	SERVERMODE_MODELOCK,	false,	false,	false,
	toggleServermodeMODELOCK
     },
     { 'm',	SERVERMODE_SHORTMOTD,	false,	false,	true,
	toggleServermodeSHORTMOTD
     },
     { 'M',	SERVERMODE_REMOTEMOTD,	false,	false,	true,
	toggleServermodeREMOTEMOTD
     },
     { 'n',	SERVERMODE_NOOP,	false,	false,	true,
	toggleServermodeNOOP
     },
     { 0 }
};



/* makeChannelModes - Create a string of channel modes according to what is set
 * Original 24/08/01, Simon Butcher <pickle@austnet.org>
 */
String makeChannelModes(Channel *channel)
{
   String reply = "+";
   String params = "";
   
   // Run through the list of standard modes and check what needs to be shown
   for (int i = 0; channelModeTable[i].letter != 0; i++) {
      // Check if this mode flag is set
      if (channel->modes & channelModeTable[i].flag) {
	 reply = reply + String(channelModeTable[i].letter);
      }
   }
   
   // Parameter based modes
   if (channel->key.length()) {
      reply = reply + "k";
      params = String(" ") + channel->key;
   }
   if (channel->limit > 0) {
      reply = reply + "l";
      params = params + " " + String(channel->limit);
   }
   
   return reply + params;
}


/* makeUserModes - [Various forms] Create a string of user modes
 * Original 24/08/01, Simon Butcher <pickle@austnet.org>
 */
String makeUserModes(TYPE_USERMODES modes)
{
   String reply = "+";
   
   // Run through the list of modes and check what needs to be shown
   for (int i = 0; userModeTable[i].letter != 0; i++) {
      // Check if this mode flag is set
      if (modes & userModeTable[i].flag) {
	 reply = reply + String(userModeTable[i].letter);
      }
   }

   return reply;
}

String makeUserModes(User *user)
{
   String reply = "+";
   String params = "";
   
   // Run through the list of modes and check what needs to be shown
   for (int i = 0; userModeTable[i].letter != 0; i++) {
      // Check if this mode flag is set
      if (user->modes & userModeTable[i].flag) {
	 reply = reply + String(userModeTable[i].letter);
      }
   }
   
   // Parameter based modes
   if (user->local && user->local->serverNotices) {
      reply = reply + "s";
      params = String(" ") + "?";
   }
   
   return reply + params;
}


/* makeServerModes - Create a string of server modes according to what is set
 * Original 21/09/01, Simon Butcher <pickle@austnet.org>
 */
String makeServerModes(Server *server)
{
   String reply = "+";
   String params = "";
   
   // Run through the list of standard modes and check what needs to be shown
   for (int i = 0; serverModeTable[i].letter != 0; i++) {
      // Check if this mode flag is set
      if (server->modes & serverModeTable[i].flag) {
	 reply = reply + String(serverModeTable[i].letter);
      }
   }
   
   return reply + params;
}


/* toggleServermodeAUTOTBS
 * Original 21/09/01, Simon Butcher <pickle@austnet.org>
 */
bool toggleServermodeAUTOTBS(bool setting, Server *server, String *param)
{
   if (setting && !(server->modes & SERVERMODE_AUTOTBS)) {
      server->modes |= SERVERMODE_AUTOTBS;
      return true;
   } else if (!setting && (server->modes & SERVERMODE_AUTOTBS)) {
      server->modes &= ~SERVERMODE_AUTOTBS;
      return true;
   }
   
   return false;
}


/* toggleServermodeHIDDEN
 * Original 21/09/01, Simon Butcher <pickle@austnet.org>
 */
bool toggleServermodeHIDDEN(bool setting, Server *server, String *param)
{
   if (setting && !(server->modes & SERVERMODE_HIDDEN)) {
      server->modes |= SERVERMODE_HIDDEN;
      return true;
   } else if (!setting && (server->modes & SERVERMODE_HIDDEN)) {
      server->modes &= ~SERVERMODE_HIDDEN;
      return true;
   }
   
   return false;
}


/* toggleServermodeMODELOCK
 * Original 23/09/01, Simon Butcher <pickle@austnet.org>
 */
bool toggleServermodeMODELOCK(bool setting, Server *server, String *param)
{
   if (setting && !(server->modes & SERVERMODE_MODELOCK)) {
      server->modes |= SERVERMODE_MODELOCK;
      return true;
   } else if (!setting && (server->modes & SERVERMODE_MODELOCK)) {
      server->modes &= ~SERVERMODE_MODELOCK;
      return true;
   }
   
   return false;
}


/* toggleServermodeAUTONOOP
 * Original 21/09/01, Simon Butcher <pickle@austnet.org>
 */
bool toggleServermodeNOOP(bool setting, Server *server, String *param)
{
   if (setting && !(server->modes & SERVERMODE_NOOP)) {
      server->modes |= SERVERMODE_NOOP;
      return true;
   } else if (!setting && (server->modes & SERVERMODE_NOOP)) {
      server->modes &= ~SERVERMODE_NOOP;
      return true;
   }
   
   return false;
}


/* toggleServermodeREMOTEMOTD
 * Original 23/09/01, Simon Butcher <pickle@austnet.org>
 */
bool toggleServermodeREMOTEMOTD(bool setting, Server *server, String *param)
{
   if (setting && !(server->modes & SERVERMODE_REMOTEMOTD)) {
      server->modes |= SERVERMODE_REMOTEMOTD;
      return true;
   } else if (!setting && (server->modes & SERVERMODE_REMOTEMOTD)) {
      server->modes &= ~SERVERMODE_REMOTEMOTD;
      return true;
   }
   
   return false;
}


/* toggleServermodeSHORTMOTD
 * Original 23/09/01, Simon Butcher <pickle@austnet.org>
 */
bool toggleServermodeSHORTMOTD(bool setting, Server *server, String *param)
{
   if (setting && !(server->modes & SERVERMODE_SHORTMOTD)) {
      server->modes |= SERVERMODE_SHORTMOTD;
      return true;
   } else if (!setting && (server->modes & SERVERMODE_SHORTMOTD)) {
      server->modes &= ~SERVERMODE_SHORTMOTD;
      return true;
   }
   
   return false;
}


/* toggleServermodeUSERSFULL
 * Original 23/09/01, Simon Butcher <pickle@austnet.org>
 */
bool toggleServermodeUSERSFULL(bool setting, Server *server, String *param)
{
   if (setting && !(server->modes & SERVERMODE_USERSFULL)) {
      server->modes |= SERVERMODE_USERSFULL;
      return true;
   } else if (!setting && (server->modes & SERVERMODE_USERSFULL)) {
      server->modes &= ~SERVERMODE_USERSFULL;
      return true;
   }
   
   return false;
}


/* toggleChanmodeANONYMOUS
 * Original 30/08/01, Simon Butcher <pickle@austnet.org>
 */
bool toggleChanmodeANONYMOUS(bool setting, Handler *handler, Channel *channel, ChannelMember *member, String *setter, String *param)
{
   /* If this is a ! channel, this can only be set/removed by the CREATOR.
    * In our case, so far, we cannot have a safe-channel creator so we just
    * fail any requests to do this on a safe channel
    */
   if (channel->name[0] == '!') {
      if (handler) {
	 handler->sendNumeric(TO_DAEMON->myServer(),
			      ERR_UNIQOPRIVSNEEDED, 0,
			      LNG_ERR_UNIQOPRIVSNEEDED);
      }
      return false;
   }
   
   // Check that this user is a channel operator
   if (member && 
       !(member->modes & CHANMEMMODE_OPPED) &&
       !(member->modes & CHANMEMMODE_HALFOPPED)) {
      if (handler) {
	 handler->sendNumeric(TO_DAEMON->myServer(),
			      ERR_CHANOPRIVSNEEDED, 0,
			      channel->name + LNG_ERR_CHANOPRIVSNEEDED);
      }
      return false;
   }
   
   if (setting && !(channel->modes & CHANMODE_ANONYMOUS)) {
      channel->modes |= CHANMODE_ANONYMOUS;
      return true;
   } else if (!setting && (channel->modes & CHANMODE_ANONYMOUS)) {
      channel->modes &= ~CHANMODE_ANONYMOUS;
      return true;
   }
   
   return false;
}


/* toggleChanmodeBAN
 * Original 28/08/01, Simon Butcher <pickle@austnet.org>
 */
bool toggleChanmodeBAN(bool setting, Handler *handler, Channel *channel, ChannelMember *member, String *setter, String *param)
{
   // Check if the user is requesting the ban list
   if (!param->length()) {
      if (handler) {
	 for (Channel::mask_list_t::iterator it = channel->bans.begin();
	      it != channel->bans.end(); it++) {
	    handler->sendNumeric(TO_DAEMON->myServer(),
				 RPL_BANLIST, 0,
				 String::printf("%s %s %s %lu",
						(char const *)channel->name,
						(char const *)(*it)->mask.getMask(),
						(char const *)(*it)->whoset,
						(*it)->whenset));
	 }

	 handler->sendNumeric(TO_DAEMON->myServer(),
			      RPL_ENDOFBANLIST, 0,
			      channel->name + LNG_RPL_ENDOFBANLIST);
      }
      
      // Return false since no mode was actually changed
      return false;
   }

   // Check that this user is a channel operator
   if (member && !(member->modes & CHANMEMMODE_OPPED)) {
      if (handler) {
	 handler->sendNumeric(TO_DAEMON->myServer(),
			      ERR_CHANOPRIVSNEEDED, 0,
			      channel->name + LNG_ERR_CHANOPRIVSNEEDED);
      }
      return false;
   }
   
   *param = param->toLower();
   
   // Are we setting or removing a mask?
   if (setting) {
      // Look for this mask in the list to check it is not already set
      for (Channel::mask_list_t::iterator it = channel->bans.begin();
	   it != channel->bans.end(); it++) {
	 if ((*it)->mask.getMask().toLower() == *param) {
	    return false;
	 }
      }

      // Create a new mask and add it to the list
      channel->bans.push_front(new ChannelMask(fixToIdentityMask(param),
					       *setter,
					       channel->daemon->getTime()));

      return true;
   } else {
      ChannelMask *mask;
      
      // Find this mask in the list
      for (Channel::mask_list_t::iterator it = channel->bans.begin();
	   it != channel->bans.end(); it++) {
	 // Check for a match
	 if ((*it)->mask.getMask().toLower() == *param) {
	    mask = *it;
	    channel->bans.erase(it);
	    delete mask;
	    return true;
	 }
      }
   }
   
   return false;
}


/* toggleChanmodeBANEXCEPT
 * Original 28/08/01, Simon Butcher <pickle@austnet.org>
 */
bool toggleChanmodeBANEXCEPT(bool setting, Handler *handler, Channel *channel, ChannelMember *member, String *setter, String *param)
{
   // Check if the user is requesting the ban exception list
   if (!param->length()) {
      if (handler) {
	 for (Channel::mask_list_t::iterator it = 
	      channel->exceptions.begin();
	      it != channel->exceptions.end(); it++) {
	    handler->sendNumeric(TO_DAEMON->myServer(),
				 RPL_EXCEPTLIST, 0,
				 String::printf("%s %s %s %lu",
						(char const *)channel->name,
						(char const *)(*it)->mask.getMask(),
						(char const *)(*it)->whoset,
						(*it)->whenset));
	 }

	 handler->sendNumeric(TO_DAEMON->myServer(),
			      RPL_ENDOFEXCEPTLIST, 0,
			      channel->name + LNG_RPL_ENDOFEXCEPTLIST);
      }
      
      // Return false since no mode was actually changed
      return false;
   }

   // Check that this user is a channel operator
   if (member && !(member->modes & CHANMEMMODE_OPPED)) {
      if (handler) {
	 handler->sendNumeric(TO_DAEMON->myServer(),
			      ERR_CHANOPRIVSNEEDED, 0,
			      channel->name + LNG_ERR_CHANOPRIVSNEEDED);
      }
      return false;
   }
   
   *param = param->toLower();
   
   // Are we setting or removing a mask?
   if (setting) {
      // Look for this mask in the list to check it is not already set
      for (Channel::mask_list_t::iterator it = channel->exceptions.begin();
	   it != channel->exceptions.end(); it++) {
	 if ((*it)->mask.getMask().toLower() == *param) {
	    return false;
	 }
      }

      // Create a new mask and add it to the list
      channel->exceptions.push_front(new ChannelMask(fixToIdentityMask(param),
						     *setter,
						     channel->daemon->getTime()));

      return true;
   } else {
      ChannelMask *mask;
      
      // Find this mask in the list
      for (Channel::mask_list_t::iterator it = channel->exceptions.begin();
	   it != channel->exceptions.end(); it++) {
	 // Check for a match
	 if ((*it)->mask.getMask().toLower() == *param) {
	    mask = *it;
	    channel->exceptions.erase(it);
	    delete mask;
	    return true;
	 }
      }
   }
   
   return false;
}


/* toggleChanmodeCHANCREATOR
 * Original , Simon Butcher <pickle@austnet.org>
 */
bool toggleChanmodeCHANCREATOR(bool setting, Handler *handler, Channel *channel, ChannelMember *member, String *setter, String *param)
{
   return false;
}


/* toggleChanmodeCHANOPER
 * Original 31/08/01, Simon Butcher <pickle@austnet.org>
 */
bool toggleChanmodeCHANOPER(bool setting, Handler *handler, Channel *channel, ChannelMember *member, String *setter, String *param)
{
   // If no parameter was given, ignore this
   if (!param->length()) {
      return false;
   }
   
   // Check that this user is a channel operator
   if (member && !(member->modes & CHANMEMMODE_OPPED)) {
      if (handler) {
	 // If they are a half oper, give them a better explanation
	 if (member->modes & CHANMEMMODE_HALFOPPED) {
	    handler->sendNumeric(TO_DAEMON->myServer(),
				 ERR_CHANOPRIVSNEEDED, 0,
				 channel->name + LNG_ERR_CHANOPRIVSNEEDED);
	 } else {
	    handler->sendNumeric(TO_DAEMON->myServer(),
				 ERR_CHANOPRIVSNEEDED, 0,
				 channel->name + 
				 LNG_ERR_CHANOPRIVSNEEDED_HALFOPERVSOPER);
	 }
      }
      return false;
   }
   
   /* Look up this nickname. We should check that it is valid but most of
    * the time it will be valid so checking would be a waste of time, 
    * perhaps...
    */
   User *u = channel->daemon->getUser(*param);
   
   // Check..
   if (!u) {
      if (handler) {
	 handler->sendNumeric(TO_DAEMON->myServer(),
			      ERR_NOSUCHNICK, 0,
			      *param + LNG_ERR_NOSUCHNICK_NICK);
      }
      return false;
   }
   
   // Grab the channel member record for this user
   ChannelMember *cm = channel->getMember(u);
   
   // Check..
   if (!cm) {
      if (handler) {
	 handler->sendNumeric(TO_DAEMON->myServer(),
			      ERR_USERNOTINCHANNEL, 0,
			      String::printf(LNG_ERR_USERNOTINCHANNEL,
					     (char const *)u->nickname,
					     (char const *)channel->name));
      }
      return false;
   }

   // Check if this user is immune from deopping
   if (u->modes & USERMODE_NONKICKABLE) {
      handler->sendNumeric(TO_DAEMON->myServer(),
			   ERR_ISCHANSERVICE, 0,
			   String::printf(LNG_ERR_ISCHANSERVICE,
					  (char const *)u->nickname,
					  (char const *)channel->name));
      return false;
   }
      
   
   // OK!! Are we setting or removing this status?
   if (setting) {
      // Check that this channel member is not already opped
      if (cm->modes & CHANMEMMODE_OPPED) {
	 return false;
      }
      
      // Set the mode, finally
      cm->modes |= CHANMEMMODE_OPPED;
   } else {
      // Check that this channel member is already deopped
      if (!(cm->modes & CHANMEMMODE_OPPED)) {
	 return false;
      }
      
      // Remove the flag
      cm->modes &= ~CHANMEMMODE_OPPED;
   }
   
   // Make the parameter string a little more accurate, for neatness!
   *param = cm->user->nickname;
   
   // We made it!
   return true;
}


/* toggleChanmodeDENY
 * Original , Simon Butcher <pickle@austnet.org>
 */
bool toggleChanmodeDENY(bool setting, Handler *handler, Channel *channel, ChannelMember *member, String *setter, String *param)
{
   return true;
}


/* toggleChanmodeEVENT
 * Original 30/08/01, Simon Butcher <pickle@austnet.org>
 */
bool toggleChanmodeEVENT(bool setting, Handler *handler, Channel *channel, ChannelMember *member, String *setter, String *param)
{
   // Check that this user is a channel operator
   if (member && 
       !(member->modes & CHANMEMMODE_OPPED) &&
       !(member->modes & CHANMEMMODE_HALFOPPED)) {
      if (handler) {
	 handler->sendNumeric(TO_DAEMON->myServer(),
			      ERR_CHANOPRIVSNEEDED, 0,
			      channel->name + LNG_ERR_CHANOPRIVSNEEDED);
      }
      return false;
   }
   
   if (setting && !(channel->modes & CHANMODE_EVENT)) {
      channel->modes |= CHANMODE_EVENT;
      return true;
   } else if (!setting && (channel->modes & CHANMODE_EVENT)) {
      channel->modes &= ~CHANMODE_EVENT;
      return true;
   }
   
   return false;
}


/* toggleChanmodeHALFOPER
 * Original 19/09/01, Simon Butcher <pickle@austnet.org>
 */
bool toggleChanmodeHALFOPER(bool setting, Handler *handler, Channel *channel, ChannelMember *member, String *setter, String *param)
{
   // If no parameter was given, ignore this
   if (!param->length()) {
      return false;
   }
   
   // If the member is themselves, we can skip a few steps..
   if (member->user->nickname.IRCtoLower() == param->IRCtoLower()) {
      // The user can dehalfop themselves, but cannot halfop themselves.
      if (!(member->modes & CHANMEMMODE_HALFOPPED) &&
	  !(member->modes & CHANMEMMODE_OPPED)) {
	 handler->sendNumeric(TO_DAEMON->myServer(),
			      ERR_CHANOPRIVSNEEDED, 0,
			      channel->name + LNG_ERR_CHANOPRIVSNEEDED);
	 return false;
      }
   } else {
      // Check that this user is a channel operator
      if (member && 
	  !(member->modes & CHANMEMMODE_OPPED) &&
	  !(member->modes & CHANMEMMODE_HALFOPPED)) {
	 if (handler) {
	    handler->sendNumeric(TO_DAEMON->myServer(),
				 ERR_CHANOPRIVSNEEDED, 0,
				 channel->name + LNG_ERR_CHANOPRIVSNEEDED);
	 }
	 return false;
      }
   }
   
   /* Look up this nickname. We should check that it is valid but most of
    * the time it will be valid so checking would be a waste of time, 
    * perhaps...
    */
   User *u = channel->daemon->getUser(*param);
   
   // Check..
   if (!u) {
      if (handler) {
	 handler->sendNumeric(TO_DAEMON->myServer(),
			      ERR_NOSUCHNICK, 0,
			      *param + LNG_ERR_NOSUCHNICK_NICK);
      }
      return false;
   }
   
   // Grab the channel member record for this user
   ChannelMember *cm = channel->getMember(u);
   
   // Check..
   if (!cm) {
      if (handler) {
	 handler->sendNumeric(TO_DAEMON->myServer(),
			      ERR_USERNOTINCHANNEL, 0,
			      String::printf(LNG_ERR_USERNOTINCHANNEL,
					     (char const *)u->nickname,
					     (char const *)channel->name));
      }
      return false;
   }
   
   // OK!! Are we setting or removing this status?
   if (setting) {
      // Check that this channel member is not already opped
      if (cm->modes & CHANMEMMODE_HALFOPPED) {
	 return false;
      }
      
      // Set the mode, finally
      cm->modes |= CHANMEMMODE_HALFOPPED;
   } else {
      // Check that this channel member is already deopped
      if (!(cm->modes & CHANMEMMODE_HALFOPPED)) {
	 return false;
      }
      
      // Remove the flag
      cm->modes &= ~CHANMEMMODE_HALFOPPED;
   }
   
   // Make the parameter string a little more accurate, for neatness!
   *param = cm->user->nickname;
   
   // We made it!
   return true;
}


/* toggleChanmodeINVITE
 * Original 30/08/01, Simon Butcher <pickle@austnet.org>
 */
bool toggleChanmodeINVITE(bool setting, Handler *handler, Channel *channel, ChannelMember *member, String *setter, String *param)
{
   // Check that this user is a channel operator
   if (member && 
       !(member->modes & CHANMEMMODE_OPPED) &&
       !(member->modes & CHANMEMMODE_HALFOPPED)) {
      if (handler) {
	 handler->sendNumeric(TO_DAEMON->myServer(),
			      ERR_CHANOPRIVSNEEDED, 0,
			      channel->name + LNG_ERR_CHANOPRIVSNEEDED);
      }
      return false;
   }
   
   if (setting && !(channel->modes & CHANMODE_INVITE)) {
      channel->modes |= CHANMODE_INVITE;
      return true;
   } else if (!setting && (channel->modes & CHANMODE_INVITE)) {
      channel->modes &= ~CHANMODE_INVITE;
      return true;
   }
   
   return false;
}


/* toggleChanmodeINVITEALLOW
 * Original 28/08/01, Simon Butcher <pickle@austnet.org>
 */
bool toggleChanmodeINVITEALLOW(bool setting, Handler *handler, Channel *channel, ChannelMember *member, String *setter, String *param)
{
   // Check if the user is requesting the ban exception list
   if (!param->length()) {
      if (handler) {
	 for (Channel::mask_list_t::iterator it = channel->invites.begin();
	      it != channel->invites.end(); it++) {
	    handler->sendNumeric(TO_DAEMON->myServer(),
				 RPL_INVITELIST, 0,
				 String::printf("%s %s %s %lu",
						(char const *)channel->name,
						(char const *)(*it)->mask.getMask(),
						(char const *)(*it)->whoset,
						(*it)->whenset));
	 }

	 handler->sendNumeric(TO_DAEMON->myServer(),
			      RPL_ENDOFINVITELIST, 0,
			      channel->name + LNG_RPL_ENDOFINVITELIST);
      }

      // Return false since no mode was actually changed
      return false;
   }

   // Check that this user is a channel operator
   if (member && !(member->modes & CHANMEMMODE_OPPED)) {
      if (handler) {
	 handler->sendNumeric(TO_DAEMON->myServer(),
			      ERR_CHANOPRIVSNEEDED, 0,
			      channel->name + LNG_ERR_CHANOPRIVSNEEDED);
      }
      return false;
   }
   
   *param = param->toLower();
   
   // Are we setting or removing a mask?
   if (setting) {
      // Look for this mask in the list to check it is not already set
      for (Channel::mask_list_t::iterator it = channel->invites.begin();
	   it != channel->invites.end(); it++) {
	 if ((*it)->mask.getMask().toLower() == *param) {
	    return false;
	 }
      }

      // Create a new mask and add it to the list
      channel->invites.push_front(new ChannelMask(fixToIdentityMask(param), 
						  *setter,
						  channel->daemon->getTime()));

      return true;
   } else {
      ChannelMask *mask;
      
      // Find this mask in the list
      for (Channel::mask_list_t::iterator it = channel->invites.begin();
	   it != channel->invites.end(); it++) {
	 // Check for a match
	 if ((*it)->mask.getMask().toLower() == *param) {
	    mask = *it;
	    channel->invites.erase(it);
	    delete mask;
	    return true;
	 }
      }
   }
   
   return false;
}


/* toggleChanmodeKEY
 * Original 30/08/01, Simon Butcher <pickle@austnet.org>
 */
bool toggleChanmodeKEY(bool setting, Handler *handler, Channel *channel, ChannelMember *member, String *setter, String *param)
{
   // Check that this user is a channel operator
   if (member && 
       !(member->modes & CHANMEMMODE_OPPED) &&
       !(member->modes & CHANMEMMODE_HALFOPPED)) {
      if (handler) {
	 handler->sendNumeric(TO_DAEMON->myServer(),
			      ERR_CHANOPRIVSNEEDED, 0,
			      channel->name + LNG_ERR_CHANOPRIVSNEEDED);
      }
      return false;
   }
   
   if (setting) {
      // Make sure we are not resetting the key, got to unset it first
      if (channel->key.length()) {
	 handler->sendNumeric(TO_DAEMON->myServer(),
			      ERR_KEYSET, 0,
			      channel->name + LNG_ERR_KEYSET);
	 return false;
      }
      
      // Make sure we were given something to set as the key!
      if (param->length()) {
	 channel->key = *param;
	 return true;
      }
      
      return false;
   } else if (!setting && (channel->key.length())) {
      // Check that the key matches the one already set
      if (channel->key != *param) {
	 return false;
      }
      
      channel->key = "";
      *param = "";
      return true;
   }

   return false;
}


/* toggleChanmodeLIMIT
 * Original 30/08/01, Simon Butcher <pickle@austnet.org>
 * Note: We reset the parameter here to make the reply 'safe'
 */
bool toggleChanmodeLIMIT(bool setting, Handler *handler, Channel *channel, ChannelMember *member, String *setter, String *param)
{
   // Check that this user is a channel operator
   if (member && 
       !(member->modes & CHANMEMMODE_OPPED) &&
       !(member->modes & CHANMEMMODE_HALFOPPED)) {
      if (handler) {
	 handler->sendNumeric(TO_DAEMON->myServer(),
			      ERR_CHANOPRIVSNEEDED, 0,
			      channel->name + LNG_ERR_CHANOPRIVSNEEDED);
      }
      return false;
   }
   
   if (setting) {
      long limit = param->toLong();
      
      // Check that worked ok
      if (limit <= 0) {
	 return false;
      }

      channel->limit = param->toLong();
      *param = String(channel->limit);
      return true;
   } else if (!setting && (channel->limit > 0)) {
      channel->limit = 0;
      *param = "";
      return true;
   }
   
   return false;
}


/* toggleChanmodeMODERATED
 * Original 30/08/01, Simon Butcher <pickle@austnet.org>
 */
bool toggleChanmodeMODERATED(bool setting, Handler *handler, Channel *channel, ChannelMember *member, String *setter, String *param)
{
   // Check that this user is a channel operator
   if (member && 
       !(member->modes & CHANMEMMODE_OPPED) &&
       !(member->modes & CHANMEMMODE_HALFOPPED)) {
      if (handler) {
	 handler->sendNumeric(TO_DAEMON->myServer(),
			      ERR_CHANOPRIVSNEEDED, 0,
			      channel->name + LNG_ERR_CHANOPRIVSNEEDED);
      }
      return false;
   }
   
   if (setting && !(channel->modes & CHANMODE_MODERATED)) {
      channel->modes |= CHANMODE_MODERATED;
      return true;
   } else if (!setting && (channel->modes & CHANMODE_MODERATED)) {
      channel->modes &= ~CHANMODE_MODERATED;
      return true;
   }
   
   return false;
}


/* toggleChanmodeNOOUTSIDEMSG
 * Original 30/08/01, Simon Butcher <pickle@austnet.org>
 */
bool toggleChanmodeNOOUTSIDEMSG(bool setting, Handler *handler, Channel *channel, ChannelMember *member, String *setter, String *param)
{
   // Check that this user is a channel operator
   if (member && 
       !(member->modes & CHANMEMMODE_OPPED) &&
       !(member->modes & CHANMEMMODE_HALFOPPED)) {
      if (handler) {
	 handler->sendNumeric(TO_DAEMON->myServer(),
			      ERR_CHANOPRIVSNEEDED, 0,
			      channel->name + LNG_ERR_CHANOPRIVSNEEDED);
      }
      return false;
   }
   
   if (setting && !(channel->modes & CHANMODE_NOOUTSIDEMSG)) {
      channel->modes |= CHANMODE_NOOUTSIDEMSG;
      return true;
   } else if (!setting && (channel->modes & CHANMODE_NOOUTSIDEMSG)) {
      channel->modes &= ~CHANMODE_NOOUTSIDEMSG;
      return true;
   }
   
   return false;
}


/* toggleChanmodePRIVATE
 * Original 30/08/01, Simon Butcher <pickle@austnet.org>
 */
bool toggleChanmodePRIVATE(bool setting, Handler *handler, Channel *channel, ChannelMember *member, String *setter, String *param)
{
   // Check that this user is a channel operator
   if (member && 
       !(member->modes & CHANMEMMODE_OPPED) &&
       !(member->modes & CHANMEMMODE_HALFOPPED)) {
      if (handler) {
	 handler->sendNumeric(TO_DAEMON->myServer(),
			      ERR_CHANOPRIVSNEEDED, 0,
			      channel->name + LNG_ERR_CHANOPRIVSNEEDED);
      }
      return false;
   }
   
   if (setting && !(channel->modes & CHANMODE_PRIVATE)) {
      channel->modes |= CHANMODE_PRIVATE;
      return true;
   } else if (!setting && (channel->modes & CHANMODE_PRIVATE)) {
      channel->modes &= ~CHANMODE_PRIVATE;
      return true;
   }
   
   return false;
}


/* toggleChanmodeREGNICKSONLY
 * Original 30/08/01, Simon Butcher <pickle@austnet.org>
 */
bool toggleChanmodeREGNICKSONLY(bool setting, Handler *handler, Channel *channel, ChannelMember *member, String *setter, String *param)
{
   // Check that this user is a channel operator
   if (member && 
       !(member->modes & CHANMEMMODE_OPPED) &&
       !(member->modes & CHANMEMMODE_HALFOPPED)) {
      if (handler) {
	 handler->sendNumeric(TO_DAEMON->myServer(),
			      ERR_CHANOPRIVSNEEDED, 0,
			      channel->name + LNG_ERR_CHANOPRIVSNEEDED);
      }
      return false;
   }
   
   if (setting && !(channel->modes & CHANMODE_REGNICKSONLY)) {
      channel->modes |= CHANMODE_REGNICKSONLY;
      return true;
   } else if (!setting && (channel->modes & CHANMODE_REGNICKSONLY)) {
      channel->modes &= ~CHANMODE_REGNICKSONLY;
      return true;
   }
   
   return false;
}


/* toggleChanmodeSECRET
 * Original 30/08/01, Simon Butcher <pickle@austnet.org>
 */
bool toggleChanmodeSECRET(bool setting, Handler *handler, Channel *channel, ChannelMember *member, String *setter, String *param)
{
   // Check that this user is a channel operator
   if (member && 
       !(member->modes & CHANMEMMODE_OPPED) &&
       !(member->modes & CHANMEMMODE_HALFOPPED)) {
      if (handler) {
	 handler->sendNumeric(TO_DAEMON->myServer(),
			      ERR_CHANOPRIVSNEEDED, 0,
			      channel->name + LNG_ERR_CHANOPRIVSNEEDED);
      }
      return false;
   }
   
   if (setting && !(channel->modes & CHANMODE_SECRET)) {
      channel->modes |= CHANMODE_SECRET;
      return true;
   } else if (!setting && (channel->modes & CHANMODE_SECRET)) {
      channel->modes &= ~CHANMODE_SECRET;
      return true;
   }
   
   return false;
}


/* toggleChanmodeTOPICLOCK
 * Original 30/08/01, Simon Butcher <pickle@austnet.org>
 */
bool toggleChanmodeTOPICLOCK(bool setting, Handler *handler, Channel *channel, ChannelMember *member, String *setter, String *param)
{
   // Check that this user is a channel operator
   if (member && 
       !(member->modes & CHANMEMMODE_OPPED) &&
       !(member->modes & CHANMEMMODE_HALFOPPED)) {
      if (handler) {
	 handler->sendNumeric(TO_DAEMON->myServer(),
			      ERR_CHANOPRIVSNEEDED, 0,
			      channel->name + LNG_ERR_CHANOPRIVSNEEDED);
      }
      return false;
   }
   
   if (setting && !(channel->modes & CHANMODE_TOPICLOCK)) {
      channel->modes |= CHANMODE_TOPICLOCK;
      return true;
   } else if (!setting && (channel->modes & CHANMODE_TOPICLOCK)) {
      channel->modes &= ~CHANMODE_TOPICLOCK;
      return true;
   }
   
   return false;
}


/* toggleChanmodeQUIET
 * Original 22/09/01, Simon Butcher <pickle@austnet.org>
 */
bool toggleChanmodeQUIET(bool setting, Handler *handler, Channel *channel, ChannelMember *member, String *setter, String *param)
{
   if (setting && !(channel->modes & CHANMODE_QUIET)) {
      channel->modes |= CHANMODE_QUIET;
      return true;
   } else if (!setting && (channel->modes & CHANMODE_QUIET)) {
      channel->modes &= ~CHANMODE_QUIET;
      return true;
   }
   
   return false;
}


/* toggleChanmodeREOP
 * Original 22/09/01, Simon Butcher <pickle@austnet.org>
 */
bool toggleChanmodeREOP(bool setting, Handler *handler, Channel *channel, ChannelMember *member, String *setter, String *param)
{
   if (setting && !(channel->modes & CHANMODE_REOP)) {
      channel->modes |= CHANMODE_REOP;
      return true;
   } else if (!setting && (channel->modes & CHANMODE_REOP)) {
      channel->modes &= ~CHANMODE_REOP;
      return true;
   }
   
   return false;
}


/* toggleChanmodeVOICE
 * Original 31/08/01, Simon Butcher <pickle@austnet.org>
 */
bool toggleChanmodeVOICE(bool setting, Handler *handler, Channel *channel, ChannelMember *member, String *setter, String *param)
{
   // If no parameter was given, ignore this
   if (!param->length()) {
      return false;
   }
   
   // If the member is themselves, we can skip a few steps..
   if (member->user->nickname.IRCtoLower() == param->IRCtoLower()) {
      // The user can devoice themselves, but cannot voice themselves.
      if (setting && 
	  !(member->modes & CHANMEMMODE_OPPED) &&
	  !(member->modes & CHANMEMMODE_HALFOPPED)) {
	 handler->sendNumeric(TO_DAEMON->myServer(),
			      ERR_CHANOPRIVSNEEDED, 0,
			      channel->name + LNG_ERR_CHANOPRIVSNEEDED);
	 return false;
      }
   } else {
      // Check that this user is a channel operator
      if (member && 
	  !(member->modes & CHANMEMMODE_OPPED) &&
	  !(member->modes & CHANMEMMODE_HALFOPPED)) {
	 if (handler) {
	    handler->sendNumeric(TO_DAEMON->myServer(),
				 ERR_CHANOPRIVSNEEDED, 0,
				 channel->name + LNG_ERR_CHANOPRIVSNEEDED);
	 }
	 return false;
      }
   }
   
   /* Look up this nickname. We should check that it is valid but most of
    * the time it will be valid so checking would be a waste of time, 
    * perhaps...
    */
   User *u = channel->daemon->getUser(*param);
   
   // Check..
   if (!u) {
      if (handler) {
	 handler->sendNumeric(TO_DAEMON->myServer(),
			      ERR_NOSUCHNICK, 0,
			      *param + LNG_ERR_NOSUCHNICK_NICK);
      }
      return false;
   }
   
   // Grab the channel member record for this user
   ChannelMember *cm = channel->getMember(u);
   
   // Check..
   if (!cm) {
      if (handler) {
	 handler->sendNumeric(TO_DAEMON->myServer(),
			      ERR_USERNOTINCHANNEL, 0,
			      String::printf(LNG_ERR_USERNOTINCHANNEL,
					     (char const *)u->nickname,
					     (char const *)channel->name));
      }
      return false;
   }
   
   // OK!! Are we setting or removing this status?
   if (setting) {
      // Check that this channel member is not already voiced!
      if (cm->modes & CHANMEMMODE_VOICED) {
	 return false;
      }
      
      // Set the mode, finally
      cm->modes |= CHANMEMMODE_VOICED;
   } else {
      // Check that this channel member is already devoiced
      if (!(cm->modes & CHANMEMMODE_VOICED)) {
	 return false;
      }
      
      // Remove the flag
      cm->modes &= ~CHANMEMMODE_VOICED;
   }
   
   // Make the parameter string a little more accurate, for neatness!
   *param = cm->user->nickname;
   
   // We made it!
   return true;
}


/* toggleUsermodeDEAF
 * Original 25/08/01, Simon Butcher <pickle@austnet.org>
 */
bool toggleUsermodeDEAF(bool setting, Handler *handler, Daemon *daemon, User *user, char *modeChr, String *param)
{
   if (setting && !(user->modes & USERMODE_DEAF)) {
      user->modes |= USERMODE_DEAF;
      return true;
   } else if (!setting && (user->modes & USERMODE_DEAF)) {
      user->modes &= ~USERMODE_DEAF;
      return true;
   }
   
   return false;
}


/* toggleUsermodeGOT_IDENTD
 * Original 22/09/01, Simon Butcher <pickle@austnet.org>
 */
bool toggleUsermodeGOT_IDENTD(bool setting, Handler *handler, Daemon *daemon, User *user, char *modeChr, String *param)
{
   if (setting && !(user->modes & USERMODE_GOT_IDENTD)) {
      user->modes |= USERMODE_GOT_IDENTD;
      return true;
   } else if (!setting && (user->modes & USERMODE_GOT_IDENTD)) {
      user->modes &= ~USERMODE_GOT_IDENTD;
      return true;
   }
   
   return false;
}


/* toggleUsermodeHELPER
 * Original 22/09/01, Simon Butcher <pickle@austnet.org>
 */
bool toggleUsermodeHELPER(bool setting, Handler *handler, Daemon *daemon, User *user, char *modeChr, String *param)
{
   // Check if we are turning this on
   if (setting) {
      if (handler) {
	 /* Tell the user they're an idiot :) */
	 if (!(user->modes & USERMODE_HELPER)) {
	    handler->sendNumeric(TO_DAEMON->myServer(),
				 ERR_NOPRIVILEGES, 0,
				 LNG_ERR_NOPRIVILEGES_PLUS_HELPER);
	 }
	 return false;
      } else {
	 // Turn it on
	 user->modes |= USERMODE_HELPER;
	 
	 // Modify the helper counter
	 daemon->numHelpers++;
	 
	 return true;
      }
   }
   
   // If this user is not a helper, this does not have an effect at all
   if (!(user->modes & USERMODE_HELPER)) {
      return false;
   }
   
   // Turn off the flag and return true
   user->modes &= ~USERMODE_HELPER;

   // Modify the helper counter
   daemon->numHelpers--;
	 
   return true;
}


/* toggleUsermodeIDENTIFIED
 * Original 22/09/01, Simon Butcher <pickle@austnet.org>
 */
bool toggleUsermodeIDENTIFIED(bool setting, Handler *handler, Daemon *daemon, User *user, char *modeChr, String *param)
{
   if (setting && !(user->modes & USERMODE_IDENTIFIED)) {
      user->modes |= USERMODE_IDENTIFIED;
      return true;
   } else if (!setting && (user->modes & USERMODE_IDENTIFIED)) {
      user->modes &= ~USERMODE_IDENTIFIED;
      return true;
   }
   
   return false;
}


/* toggleUsermodeIGNORING
 * Original 22/09/01, Simon Butcher <pickle@austnet.org>
 */
bool toggleUsermodeIGNORING(bool setting, Handler *handler, Daemon *daemon, User *user, char *modeChr, String *param)
{
   if (setting && !(user->modes & USERMODE_IGNORING)) {
      user->modes |= USERMODE_IGNORING;
      return true;
   } else if (!setting && (user->modes & USERMODE_IGNORING)) {
      user->modes &= ~USERMODE_IGNORING;
      return true;
   }
   
   return false;
}


/* toggleUsermodeINVISIBLE
 * Original 25/08/01, Simon Butcher <pickle@austnet.org>
 */
bool toggleUsermodeINVISIBLE(bool setting, Handler *handler, Daemon *daemon, User *user, char *modeChr, String *param)
{
   if (setting && !(user->modes & USERMODE_INVISIBLE)) {
      user->modes |= USERMODE_INVISIBLE;
      return true;
   } else if (!setting && (user->modes & USERMODE_INVISIBLE)) {
      user->modes &= ~USERMODE_INVISIBLE;
      return true;
   }
   
   return false;
}


/* toggleUsermodeNONKICKABLE
 * Original 22/09/01, Simon Butcher <pickle@austnet.org>
 */
bool toggleUsermodeNONKICKABLE(bool setting, Handler *handler, Daemon *daemon, User *user, char *modeChr, String *param)
{
   if (setting && !(user->modes & USERMODE_NONKICKABLE)) {
      user->modes |= USERMODE_NONKICKABLE;
      return true;
   } else if (!setting && (user->modes & USERMODE_NONKICKABLE)) {
      user->modes &= ~USERMODE_NONKICKABLE;
      return true;
   }
   
   return false;
}


/* toggleUsermodeOPER
 * Original 25/08/01, Simon Butcher <pickle@austnet.org>
 * Note: This can only be toggled off, not on.
 */
bool toggleUsermodeOPER(bool setting, Handler *handler, Daemon *daemon, User *user, char *modeChr, String *param)
{
   // Check if this user is trying to turn this mode on
   if (setting) {
      if (handler) {
	 if (!(user->modes & USERMODE_GLOBALOPER) ||
	     !(user->modes & USERMODE_LOCALOPER)) {
	    /* Tell the user they're an idiot :) */
	    handler->sendNumeric(TO_DAEMON->myServer(),
				ERR_NOPRIVILEGES, 0,
				 LNG_ERR_NOPRIVILEGES_PLUS_IRCOPER);
	 }
	 return false;
      } else {
	 // If this is setting someone as a global operator, then turn it on.
	 if (*modeChr == 'O') {
	    user->modes |= USERMODE_GLOBALOPER;
	    return true;
	 }
	 
	 // Ignore it, must be a local oper and a mistake.
	 return false;
      }
   }
   
   // If this user is not an oper, this does not have an effect at all
   if (!(user->modes & USERMODE_GLOBALOPER) &&
       !(user->modes & USERMODE_LOCALOPER)) {
      return false;
   }
   
   // Fix up the character so that the client does not get confused.
   if (user->modes & USERMODE_GLOBALOPER) {
      // While we are here, decrement the global operator counter
      daemon->numOpers--;
      
      *modeChr = 'O';
   } else {
      *modeChr = 'o';
   }
   
   // Turn off the oper flags and return true
   user->modes &= ~USERMODE_GLOBALOPER & ~USERMODE_LOCALOPER;

   return true;
}


/* toggleUsermodeREGNICKSMSG
 * Original 30/08/01, Simon Butcher <pickle@austnet.org>
 */
bool toggleUsermodeREGNICKSMSG(bool setting, Handler *handler, Daemon *daemon, User *user, char *modeChr, String *param)
{
   if (setting && !(user->modes & USERMODE_REGNICKSMSG)) {
      user->modes |= USERMODE_REGNICKSMSG;

      // If this user is an oper or a helper they really should not have this
      if (handler && User::isHelper(user)) {
	 handler->sendNumeric(TO_DAEMON->myServer(),
			      RPL_MODECHANGEWARN, 0,
			      LNG_RPL_MODECHANGEWARN_HELPER_PLUS_REGNICKSMSG);
      }
      
      return true;
   } else if (!setting && (user->modes & USERMODE_REGNICKSMSG)) {
      user->modes &= ~USERMODE_REGNICKSMSG;
      return true;
   }
   
   return false;
}


/* toggleUsermodeRESTRICTED
 * Original 22/09/01, Simon Butcher <pickle@austnet.org>
 */
bool toggleUsermodeRESTRICTED(bool setting, Handler *handler, Daemon *daemon, User *user, char *modeChr, String *param)
{
   if (setting && !(user->modes & USERMODE_RESTRICTED)) {
      user->modes |= USERMODE_RESTRICTED;
      return true;
   } else if (!setting && (user->modes & USERMODE_RESTRICTED)) {
      user->modes &= ~USERMODE_RESTRICTED;
      return true;
   }
   
   return false;
}


/* toggleUsermodeSECURE
 * Original 22/09/01, Simon Butcher <pickle@austnet.org>
 */
bool toggleUsermodeSECURE(bool setting, Handler *handler, Daemon *daemon, User *user, char *modeChr, String *param)
{
   if (setting && !(user->modes & USERMODE_SECURE)) {
      user->modes |= USERMODE_SECURE;
      return true;
   } else if (!setting && (user->modes & USERMODE_SECURE)) {
      user->modes &= ~USERMODE_SECURE;
      return true;
   }
   
   return false;
}


/* toggleUsermodeSERVNOTICES
 * Original , Simon Butcher <pickle@austnet.org>
 */
bool toggleUsermodeSERVNOTICES(bool setting, Handler *handler, Daemon *daemon, User *user, char *modeChr, String *param)
{
   // This should not be set remotely
   if (!user->local) {
      return false;
   }
   
   // Check if we are setting this mode
   if (setting) {
      user->local->serverNotices = 0xFFFFFFFF;
      return true; // fix me
   } 

   // Turn off all the modes
   user->local->serverNotices = 0;
   
   // Return true, since the server notices are turned off completely
   return true;
}


/* toggleUsermodeVWORLD
 * Original 25/08/01, Simon Butcher <pickle@austnet.org>
 */
bool toggleUsermodeVWORLD(bool setting, Handler *handler, Daemon *daemon, User *user, char *modeChr, String *param)
{
   if (setting && !(user->modes & USERMODE_VWORLD)) {
      user->modes |= USERMODE_VWORLD;
      return true;
   } else if (!setting && (user->modes & USERMODE_VWORLD)) {
      user->modes &= ~USERMODE_VWORLD;
      
      // Warn this user, if we can, about turning off virtual world
      if (handler) {
	 handler->sendNumeric(TO_DAEMON->myServer(),
			      RPL_MODECHANGEWARN, 0,
			      LNG_RPL_MODECHANGEWARN_MINUS_VWORLD);
      }
      
      return true;
   }

   return false;
}


/* toggleUsermodeWALLOPS
 * Original 25/08/01, Simon Butcher <pickle@austnet.org>
 */
bool toggleUsermodeWALLOPS(bool setting, Handler *handler, Daemon *daemon, User *user, char *modeChr, String *param)
{
   if (setting && !(user->modes & USERMODE_WALLOPS)) {
      user->modes |= USERMODE_WALLOPS;
      return true;
   } else if (!setting && (user->modes & USERMODE_WALLOPS)) {
      user->modes &= ~USERMODE_WALLOPS;
      
      // Check if we need to (and can) warn this user about this command
      if (handler &&
	  ((user->modes & USERMODE_GLOBALOPER) ||
	   (user->modes & USERMODE_LOCALOPER))) {
	 handler->sendNumeric(TO_DAEMON->myServer(),
			      RPL_MODECHANGEWARN, 0,
			      LNG_RPL_MODECHANGEWARN_IRCOPER_MINUS_WALLOPS);
      }
      
      return true;
   }
   
   return false;
}
