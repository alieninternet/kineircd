/* modes.cpp
 * Server/Channel/User mode tables and toggling routines
 */

#include "config.h"

#include "modes.h"
#include "language.h"
#include "numerics.h"
#include "channel.h"
#include "utils.h"


/* toggleServermodeAUTOTBS
 * Original 21/09/01, Simon Butcher <pickle@austnet.org>
 */
bool toggleServermodeAUTOTBS(bool setting, Server *server, String *param)
{
   if (setting && !(server->modes & Server::MODE_AUTOTBS)) {
      server->modes |= Server::MODE_AUTOTBS;
      return true;
   } else if (!setting && (server->modes & Server::MODE_AUTOTBS)) {
      server->modes &= ~Server::MODE_AUTOTBS;
      return true;
   }
   
   return false;
}


/* toggleServermodeHIDDEN
 * Original 21/09/01, Simon Butcher <pickle@austnet.org>
 */
bool toggleServermodeHIDDEN(bool setting, Server *server, String *param)
{
   if (setting && !(server->modes & Server::MODE_HIDDEN)) {
      server->modes |= Server::MODE_HIDDEN;
      return true;
   } else if (!setting && (server->modes & Server::MODE_HIDDEN)) {
      server->modes &= ~Server::MODE_HIDDEN;
      return true;
   }
   
   return false;
}


/* toggleServermodeMODELOCK
 * Original 23/09/01, Simon Butcher <pickle@austnet.org>
 */
bool toggleServermodeMODELOCK(bool setting, Server *server, String *param)
{
   if (setting && !(server->modes & Server::MODE_MODELOCK)) {
      server->modes |= Server::MODE_MODELOCK;
      return true;
   } else if (!setting && (server->modes & Server::MODE_MODELOCK)) {
      server->modes &= ~Server::MODE_MODELOCK;
      return true;
   }
   
   return false;
}


/* toggleServermodeAUTONOOP
 * Original 21/09/01, Simon Butcher <pickle@austnet.org>
 */
bool toggleServermodeNOOP(bool setting, Server *server, String *param)
{
   if (setting && !(server->modes & Server::MODE_NOOP)) {
      server->modes |= Server::MODE_NOOP;
      return true;
   } else if (!setting && (server->modes & Server::MODE_NOOP)) {
      server->modes &= ~Server::MODE_NOOP;
      return true;
   }
   
   return false;
}


/* toggleServermodeREMOTEMOTD
 * Original 23/09/01, Simon Butcher <pickle@austnet.org>
 */
bool toggleServermodeREMOTEMOTD(bool setting, Server *server, String *param)
{
   if (setting && !(server->modes & Server::MODE_REMOTEMOTD)) {
      server->modes |= Server::MODE_REMOTEMOTD;
      return true;
   } else if (!setting && (server->modes & Server::MODE_REMOTEMOTD)) {
      server->modes &= ~Server::MODE_REMOTEMOTD;
      return true;
   }
   
   return false;
}


/* toggleServermodeSHORTMOTD
 * Original 23/09/01, Simon Butcher <pickle@austnet.org>
 */
bool toggleServermodeSHORTMOTD(bool setting, Server *server, String *param)
{
   if (setting && !(server->modes & Server::MODE_SHORTMOTD)) {
      server->modes |= Server::MODE_SHORTMOTD;
      return true;
   } else if (!setting && (server->modes & Server::MODE_SHORTMOTD)) {
      server->modes &= ~Server::MODE_SHORTMOTD;
      return true;
   }
   
   return false;
}


/* toggleServermodeUSERSFULL
 * Original 23/09/01, Simon Butcher <pickle@austnet.org>
 */
bool toggleServermodeUSERSFULL(bool setting, Server *server, String *param)
{
   if (setting && !(server->modes & Server::MODE_USERSFULL)) {
      server->modes |= Server::MODE_USERSFULL;
      return true;
   } else if (!setting && (server->modes & Server::MODE_USERSFULL)) {
      server->modes &= ~Server::MODE_USERSFULL;
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
       !(member->modes & ChannelMember::MODE_OPPED) &&
       !(member->modes & ChannelMember::MODE_HALFOPPED)) {
      if (handler) {
	 handler->sendNumeric(TO_DAEMON->myServer(),
			      ERR_CHANOPRIVSNEEDED, 0,
			      channel->name + LNG_ERR_CHANOPRIVSNEEDED);
      }
      return false;
   }
   
   if (setting && !(channel->modes & Channel::MODE_ANONYMOUS)) {
      channel->modes |= Channel::MODE_ANONYMOUS;
      return true;
   } else if (!setting && (channel->modes & Channel::MODE_ANONYMOUS)) {
      channel->modes &= ~Channel::MODE_ANONYMOUS;
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
   if (member && !(member->modes & ChannelMember::MODE_OPPED)) {
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
      channel->bans.push_front(new ChannelMask(fixToIdentityMask(*param),
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
   if (member && !(member->modes & ChannelMember::MODE_OPPED)) {
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
      channel->exceptions.push_front(new ChannelMask(fixToIdentityMask(*param),
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
   if (member && !(member->modes & ChannelMember::MODE_OPPED)) {
      if (handler) {
	 // If they are a half oper, give them a better explanation
	 if (member->modes & ChannelMember::MODE_HALFOPPED) {
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
   if (u->modes & User::MODE_NONKICKABLE) {
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
      if (cm->modes & ChannelMember::MODE_OPPED) {
	 return false;
      }
      
      // Set the mode, finally
      cm->modes |= ChannelMember::MODE_OPPED;
   } else {
      // Check that this channel member is already deopped
      if (!(cm->modes & ChannelMember::MODE_OPPED)) {
	 return false;
      }
      
      // Remove the flag
      cm->modes &= ~ChannelMember::MODE_OPPED;
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
       !(member->modes & ChannelMember::MODE_OPPED) &&
       !(member->modes & ChannelMember::MODE_HALFOPPED)) {
      if (handler) {
	 handler->sendNumeric(TO_DAEMON->myServer(),
			      ERR_CHANOPRIVSNEEDED, 0,
			      channel->name + LNG_ERR_CHANOPRIVSNEEDED);
      }
      return false;
   }
   
   if (setting && !(channel->modes & Channel::MODE_EVENT)) {
      channel->modes |= Channel::MODE_EVENT;
      return true;
   } else if (!setting && (channel->modes & Channel::MODE_EVENT)) {
      channel->modes &= ~Channel::MODE_EVENT;
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
      if (!(member->modes & ChannelMember::MODE_HALFOPPED) &&
	  !(member->modes & ChannelMember::MODE_OPPED)) {
	 handler->sendNumeric(TO_DAEMON->myServer(),
			      ERR_CHANOPRIVSNEEDED, 0,
			      channel->name + LNG_ERR_CHANOPRIVSNEEDED);
	 return false;
      }
   } else {
      // Check that this user is a channel operator
      if (member && 
	  !(member->modes & ChannelMember::MODE_OPPED) &&
	  !(member->modes & ChannelMember::MODE_HALFOPPED)) {
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
      if (cm->modes & ChannelMember::MODE_HALFOPPED) {
	 return false;
      }
      
      // Set the mode, finally
      cm->modes |= ChannelMember::MODE_HALFOPPED;
   } else {
      // Check that this channel member is already deopped
      if (!(cm->modes & ChannelMember::MODE_HALFOPPED)) {
	 return false;
      }
      
      // Remove the flag
      cm->modes &= ~ChannelMember::MODE_HALFOPPED;
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
       !(member->modes & ChannelMember::MODE_OPPED) &&
       !(member->modes & ChannelMember::MODE_HALFOPPED)) {
      if (handler) {
	 handler->sendNumeric(TO_DAEMON->myServer(),
			      ERR_CHANOPRIVSNEEDED, 0,
			      channel->name + LNG_ERR_CHANOPRIVSNEEDED);
      }
      return false;
   }
   
   if (setting && !(channel->modes & Channel::MODE_INVITE)) {
      channel->modes |= Channel::MODE_INVITE;
      return true;
   } else if (!setting && (channel->modes & Channel::MODE_INVITE)) {
      channel->modes &= ~Channel::MODE_INVITE;
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
   if (member && !(member->modes & ChannelMember::MODE_OPPED)) {
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
      channel->invites.push_front(new ChannelMask(fixToIdentityMask(*param),
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
       !(member->modes & ChannelMember::MODE_OPPED) &&
       !(member->modes & ChannelMember::MODE_HALFOPPED)) {
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
       !(member->modes & ChannelMember::MODE_OPPED) &&
       !(member->modes & ChannelMember::MODE_HALFOPPED)) {
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
       !(member->modes & ChannelMember::MODE_OPPED) &&
       !(member->modes & ChannelMember::MODE_HALFOPPED)) {
      if (handler) {
	 handler->sendNumeric(TO_DAEMON->myServer(),
			      ERR_CHANOPRIVSNEEDED, 0,
			      channel->name + LNG_ERR_CHANOPRIVSNEEDED);
      }
      return false;
   }
   
   if (setting && !(channel->modes & Channel::MODE_MODERATED)) {
      channel->modes |= Channel::MODE_MODERATED;
      return true;
   } else if (!setting && (channel->modes & Channel::MODE_MODERATED)) {
      channel->modes &= ~Channel::MODE_MODERATED;
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
       !(member->modes & ChannelMember::MODE_OPPED) &&
       !(member->modes & ChannelMember::MODE_HALFOPPED)) {
      if (handler) {
	 handler->sendNumeric(TO_DAEMON->myServer(),
			      ERR_CHANOPRIVSNEEDED, 0,
			      channel->name + LNG_ERR_CHANOPRIVSNEEDED);
      }
      return false;
   }
   
   if (setting && !(channel->modes & Channel::MODE_NOOUTSIDEMSG)) {
      channel->modes |= Channel::MODE_NOOUTSIDEMSG;
      return true;
   } else if (!setting && (channel->modes & Channel::MODE_NOOUTSIDEMSG)) {
      channel->modes &= ~Channel::MODE_NOOUTSIDEMSG;
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
       !(member->modes & ChannelMember::MODE_OPPED) &&
       !(member->modes & ChannelMember::MODE_HALFOPPED)) {
      if (handler) {
	 handler->sendNumeric(TO_DAEMON->myServer(),
			      ERR_CHANOPRIVSNEEDED, 0,
			      channel->name + LNG_ERR_CHANOPRIVSNEEDED);
      }
      return false;
   }
   
   if (setting && !(channel->modes & Channel::MODE_PRIVATE)) {
      channel->modes |= Channel::MODE_PRIVATE;
      return true;
   } else if (!setting && (channel->modes & Channel::MODE_PRIVATE)) {
      channel->modes &= ~Channel::MODE_PRIVATE;
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
       !(member->modes & ChannelMember::MODE_OPPED) &&
       !(member->modes & ChannelMember::MODE_HALFOPPED)) {
      if (handler) {
	 handler->sendNumeric(TO_DAEMON->myServer(),
			      ERR_CHANOPRIVSNEEDED, 0,
			      channel->name + LNG_ERR_CHANOPRIVSNEEDED);
      }
      return false;
   }
   
   if (setting && !(channel->modes & Channel::MODE_REGNICKSONLY)) {
      channel->modes |= Channel::MODE_REGNICKSONLY;
      return true;
   } else if (!setting && (channel->modes & Channel::MODE_REGNICKSONLY)) {
      channel->modes &= ~Channel::MODE_REGNICKSONLY;
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
       !(member->modes & ChannelMember::MODE_OPPED) &&
       !(member->modes & ChannelMember::MODE_HALFOPPED)) {
      if (handler) {
	 handler->sendNumeric(TO_DAEMON->myServer(),
			      ERR_CHANOPRIVSNEEDED, 0,
			      channel->name + LNG_ERR_CHANOPRIVSNEEDED);
      }
      return false;
   }
   
   if (setting && !(channel->modes & Channel::MODE_SECRET)) {
      channel->modes |= Channel::MODE_SECRET;
      return true;
   } else if (!setting && (channel->modes & Channel::MODE_SECRET)) {
      channel->modes &= ~Channel::MODE_SECRET;
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
       !(member->modes & ChannelMember::MODE_OPPED) &&
       !(member->modes & ChannelMember::MODE_HALFOPPED)) {
      if (handler) {
	 handler->sendNumeric(TO_DAEMON->myServer(),
			      ERR_CHANOPRIVSNEEDED, 0,
			      channel->name + LNG_ERR_CHANOPRIVSNEEDED);
      }
      return false;
   }
   
   if (setting && !(channel->modes & Channel::MODE_TOPICLOCK)) {
      channel->modes |= Channel::MODE_TOPICLOCK;
      return true;
   } else if (!setting && (channel->modes & Channel::MODE_TOPICLOCK)) {
      channel->modes &= ~Channel::MODE_TOPICLOCK;
      return true;
   }
   
   return false;
}


/* toggleChanmodeQUIET
 * Original 22/09/01, Simon Butcher <pickle@austnet.org>
 */
bool toggleChanmodeQUIET(bool setting, Handler *handler, Channel *channel, ChannelMember *member, String *setter, String *param)
{
   if (setting && !(channel->modes & Channel::MODE_QUIET)) {
      channel->modes |= Channel::MODE_QUIET;
      return true;
   } else if (!setting && (channel->modes & Channel::MODE_QUIET)) {
      channel->modes &= ~Channel::MODE_QUIET;
      return true;
   }
   
   return false;
}


/* toggleChanmodeREOP
 * Original 22/09/01, Simon Butcher <pickle@austnet.org>
 */
bool toggleChanmodeREOP(bool setting, Handler *handler, Channel *channel, ChannelMember *member, String *setter, String *param)
{
   if (setting && !(channel->modes & Channel::MODE_REOP)) {
      channel->modes |= Channel::MODE_REOP;
      return true;
   } else if (!setting && (channel->modes & Channel::MODE_REOP)) {
      channel->modes &= ~Channel::MODE_REOP;
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
	  !(member->modes & ChannelMember::MODE_OPPED) &&
	  !(member->modes & ChannelMember::MODE_HALFOPPED)) {
	 handler->sendNumeric(TO_DAEMON->myServer(),
			      ERR_CHANOPRIVSNEEDED, 0,
			      channel->name + LNG_ERR_CHANOPRIVSNEEDED);
	 return false;
      }
   } else {
      // Check that this user is a channel operator
      if (member && 
	  !(member->modes & ChannelMember::MODE_OPPED) &&
	  !(member->modes & ChannelMember::MODE_HALFOPPED)) {
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
      if (cm->modes & ChannelMember::MODE_VOICED) {
	 return false;
      }
      
      // Set the mode, finally
      cm->modes |= ChannelMember::MODE_VOICED;
   } else {
      // Check that this channel member is already devoiced
      if (!(cm->modes & ChannelMember::MODE_VOICED)) {
	 return false;
      }
      
      // Remove the flag
      cm->modes &= ~ChannelMember::MODE_VOICED;
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
   if (setting && !(user->modes & User::MODE_DEAF)) {
      user->modes |= User::MODE_DEAF;
      return true;
   } else if (!setting && (user->modes & User::MODE_DEAF)) {
      user->modes &= ~User::MODE_DEAF;
      return true;
   }
   
   return false;
}


/* toggleUsermodeGOT_IDENTD
 * Original 22/09/01, Simon Butcher <pickle@austnet.org>
 */
bool toggleUsermodeGOT_IDENTD(bool setting, Handler *handler, Daemon *daemon, User *user, char *modeChr, String *param)
{
   if (setting && !(user->modes & User::MODE_GOT_IDENTD)) {
      user->modes |= User::MODE_GOT_IDENTD;
      return true;
   } else if (!setting && (user->modes & User::MODE_GOT_IDENTD)) {
      user->modes &= ~User::MODE_GOT_IDENTD;
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
	 if (!(user->modes & User::MODE_HELPER)) {
	    handler->sendNumeric(TO_DAEMON->myServer(),
				 ERR_NOPRIVILEGES, 0,
				 LNG_ERR_NOPRIVILEGES_PLUS_HELPER);
	 }
	 return false;
      } else {
	 // Turn it on
	 user->modes |= User::MODE_HELPER;
	 
	 // Modify the helper counter
	 daemon->numHelpers++;
	 
	 return true;
      }
   }
   
   // If this user is not a helper, this does not have an effect at all
   if (!(user->modes & User::MODE_HELPER)) {
      return false;
   }
   
   // Turn off the flag and return true
   user->modes &= ~User::MODE_HELPER;

   // Modify the helper counter
   daemon->numHelpers--;
	 
   return true;
}


/* toggleUsermodeIDENTIFIED
 * Original 22/09/01, Simon Butcher <pickle@austnet.org>
 */
bool toggleUsermodeIDENTIFIED(bool setting, Handler *handler, Daemon *daemon, User *user, char *modeChr, String *param)
{
   if (setting && !(user->modes & User::MODE_IDENTIFIED)) {
      user->modes |= User::MODE_IDENTIFIED;
      return true;
   } else if (!setting && (user->modes & User::MODE_IDENTIFIED)) {
      user->modes &= ~User::MODE_IDENTIFIED;
      return true;
   }
   
   return false;
}


/* toggleUsermodeIGNORING
 * Original 22/09/01, Simon Butcher <pickle@austnet.org>
 */
bool toggleUsermodeIGNORING(bool setting, Handler *handler, Daemon *daemon, User *user, char *modeChr, String *param)
{
   if (setting && !(user->modes & User::MODE_IGNORING)) {
      user->modes |= User::MODE_IGNORING;
      return true;
   } else if (!setting && (user->modes & User::MODE_IGNORING)) {
      user->modes &= ~User::MODE_IGNORING;
      return true;
   }
   
   return false;
}


/* toggleUsermodeINVISIBLE
 * Original 25/08/01, Simon Butcher <pickle@austnet.org>
 */
bool toggleUsermodeINVISIBLE(bool setting, Handler *handler, Daemon *daemon, User *user, char *modeChr, String *param)
{
   if (setting && !(user->modes & User::MODE_INVISIBLE)) {
      user->modes |= User::MODE_INVISIBLE;
      return true;
   } else if (!setting && (user->modes & User::MODE_INVISIBLE)) {
      user->modes &= ~User::MODE_INVISIBLE;
      return true;
   }
   
   return false;
}


/* toggleUsermodeNONKICKABLE
 * Original 22/09/01, Simon Butcher <pickle@austnet.org>
 */
bool toggleUsermodeNONKICKABLE(bool setting, Handler *handler, Daemon *daemon, User *user, char *modeChr, String *param)
{
   if (setting && !(user->modes & User::MODE_NONKICKABLE)) {
      user->modes |= User::MODE_NONKICKABLE;
      return true;
   } else if (!setting && (user->modes & User::MODE_NONKICKABLE)) {
      user->modes &= ~User::MODE_NONKICKABLE;
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
	 if (!(user->modes & User::MODE_GLOBALOPER) ||
	     !(user->modes & User::MODE_LOCALOPER)) {
	    /* Tell the user they're an idiot :) */
	    handler->sendNumeric(TO_DAEMON->myServer(),
				ERR_NOPRIVILEGES, 0,
				 LNG_ERR_NOPRIVILEGES_PLUS_IRCOPER);
	 }
	 return false;
      } else {
	 // If this is setting someone as a global operator, then turn it on.
	 if (*modeChr == 'O') {
	    user->modes |= User::MODE_GLOBALOPER;
	    return true;
	 }
	 
	 // Ignore it, must be a local oper and a mistake.
	 return false;
      }
   }
   
   // If this user is not an oper, this does not have an effect at all
   if (!(user->modes & User::MODE_GLOBALOPER) &&
       !(user->modes & User::MODE_LOCALOPER)) {
      return false;
   }
   
   // Fix up the character so that the client does not get confused.
   if (user->modes & User::MODE_GLOBALOPER) {
      // While we are here, decrement the global operator counter
      daemon->numOpers--;
      
      *modeChr = 'O';
   } else {
      *modeChr = 'o';
   }
   
   // Turn off the oper flags and return true
   user->modes &= ~User::MODE_GLOBALOPER & ~User::MODE_LOCALOPER;

   return true;
}


/* toggleUsermodeREGNICKSMSG
 * Original 30/08/01, Simon Butcher <pickle@austnet.org>
 */
bool toggleUsermodeREGNICKSMSG(bool setting, Handler *handler, Daemon *daemon, User *user, char *modeChr, String *param)
{
   if (setting && !(user->modes & User::MODE_REGNICKSMSG)) {
      user->modes |= User::MODE_REGNICKSMSG;

      // If this user is an oper or a helper they really should not have this
      if (handler && User::isHelper(user)) {
	 handler->sendNumeric(TO_DAEMON->myServer(),
			      RPL_MODECHANGEWARN, 0,
			      LNG_RPL_MODECHANGEWARN_HELPER_PLUS_REGNICKSMSG);
      }
      
      return true;
   } else if (!setting && (user->modes & User::MODE_REGNICKSMSG)) {
      user->modes &= ~User::MODE_REGNICKSMSG;
      return true;
   }
   
   return false;
}


/* toggleUsermodeRESTRICTED
 * Original 22/09/01, Simon Butcher <pickle@austnet.org>
 */
bool toggleUsermodeRESTRICTED(bool setting, Handler *handler, Daemon *daemon, User *user, char *modeChr, String *param)
{
   if (setting && !(user->modes & User::MODE_RESTRICTED)) {
      user->modes |= User::MODE_RESTRICTED;
      return true;
   } else if (!setting && (user->modes & User::MODE_RESTRICTED)) {
      user->modes &= ~User::MODE_RESTRICTED;
      return true;
   }
   
   return false;
}


/* toggleUsermodeSECURE
 * Original 22/09/01, Simon Butcher <pickle@austnet.org>
 */
bool toggleUsermodeSECURE(bool setting, Handler *handler, Daemon *daemon, User *user, char *modeChr, String *param)
{
   if (setting && !(user->modes & User::MODE_SECURE)) {
      user->modes |= User::MODE_SECURE;
      return true;
   } else if (!setting && (user->modes & User::MODE_SECURE)) {
      user->modes &= ~User::MODE_SECURE;
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
   if (setting && !(user->modes & User::MODE_VWORLD)) {
      user->modes |= User::MODE_VWORLD;
      return true;
   } else if (!setting && (user->modes & User::MODE_VWORLD)) {
      user->modes &= ~User::MODE_VWORLD;
      
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
   if (setting && !(user->modes & User::MODE_WALLOPS)) {
      user->modes |= User::MODE_WALLOPS;
      return true;
   } else if (!setting && (user->modes & User::MODE_WALLOPS)) {
      user->modes &= ~User::MODE_WALLOPS;
      
      // Check if we need to (and can) warn this user about this command
      if (handler &&
	  ((user->modes & User::MODE_GLOBALOPER) ||
	   (user->modes & User::MODE_LOCALOPER))) {
	 handler->sendNumeric(TO_DAEMON->myServer(),
			      RPL_MODECHANGEWARN, 0,
			      LNG_RPL_MODECHANGEWARN_IRCOPER_MINUS_WALLOPS);
      }
      
      return true;
   }
   
   return false;
}
