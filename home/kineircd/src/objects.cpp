/* objects.cpp
 * Channel class
 */

#include "config.h"

#include "daemon.h"
#include "user.h"
#include "channel.h"
#include "numerics.h"
#include "language.h"
#include "utils.h"
#include "modes.h"


/* ~Channel - Channel class destructor
 * Original 30/08/01, Simon Butcher <pickle@austnet.org>
 */
Channel::~Channel(void)
{
   // Run through the mask lists to kill them off
   ChannelMask *mask;
   
   while (!bans.empty()) {
      mask = *bans.begin();
      bans.erase(bans.begin());
      delete mask;
   }
   
   while (!exceptions.empty()) {
      mask = *exceptions.begin();
      exceptions.erase(exceptions.begin());
      delete mask;
   }

   while (!invites.empty()) {
      mask = *invites.begin();
      invites.erase(invites.begin());
      delete mask;
   }
   
   // Wipe out the members list for safety
   members.clear();
}


/* getMember - Grab a channel member record
 * Original 24/08/01, Simon Butcher <pickle@austnet.org>
 */
ChannelMember *Channel::getMember(User *u)
{
   String nick = u->nickname.IRCtoLower();
   ChannelMember *cm = 0;
   
   // Grab the record if we can
   cm = members[nick];
   
   // Make sure we got it
   if (cm) {
      return cm;
   }
   
   // Else we gotta delete this empty record we just make and return nothing
   members.erase(nick);
   
   return 0;
}


/* processModes - Grab a channel member record
 * Original 30/08/01, Simon Butcher <pickle@austnet.org>
 */
void Channel::processModes(Handler *handler, User *user,
			   String &modes, StringTokens *tokens)
{
   // This channel must support modes... check this first
   if (name[0] == '+') {
      if (handler) {
	 handler->sendNumeric(TO_DAEMON->myServer(),
			      ERR_NOCHANMODES, 0,
			      name + LNG_ERR_NOCHANMODES);
      }
      return;
   }
   
   /* Notice how we COPY the channel member record. This is so that multiple
    * changes where an access removal might occur, eg. user with the nickname
    * 'nick1' is setting the mode to deop 'nick1' and op 'nick2'. Without this
    * a /mode #channel -o+o nick1 nick2 would result in nick1 being deopped
    * but nick2 not being opped because of nick1 now being deopped. This is
    * incorrect as the calling user's modes need to remain static over the
    * entire mode string
    */
   ChannelMember cm;
   
   // If this is a user request, grab the user record
   if (handler) {
      ChannelMember *cmTemp = getMember(user);
      
      // Check we got this user...
      if (!cmTemp) {
	 handler->sendNumeric(TO_DAEMON->myServer(),
			      ERR_NOTONCHANNEL, 0,
			      name + LNG_ERR_NOTONCHANNEL);
	 return;
      }
      
      // Copy the data into our real channel member record
      cm = *cmTemp;
   }
   
   bool toggle = true;
   int numModes = 0;
   String param = "";
   String toggleOnStr = "";
   String toggleOffStr = "";
   String toggleParamsOn = "";
   String toggleParamsOff = "";
   
   for (String::length_t i = 0;
	((i < modes.length()) && (numModes < MAX_MODES_PER_COMMAND));
	i++) {
      switch (modes[i]) {
       case '+':
	 toggle = true;
	 continue;
       case '-':
	 toggle = false;
	 continue;
       default:
	 bool gotModeChar = false;
	 numModes++;
	 
	 // Run though the channel mode list
	 for (int ii = 0; channelModeTable[ii].letter != 0; ii++) {
	    if (channelModeTable[ii].letter == modes[i]) {
	       // Can we modify this mode?
	       if ((handler && channelModeTable[ii].userToggle) ||
		   !handler) {
		  // If this mode needs a parameter, grab the next token...
		  if ((toggle && channelModeTable[ii].hasParamOn) ||
		      (!toggle && channelModeTable[ii].hasParamOff)) {
		     param = tokens->nextToken();
		  } else {
		     // Reset the parameter thing
		     param = "";
		  }
		  
		  // Run the toggler for this mode, checking if it worked
		  if (channelModeTable[ii].toggler(toggle, handler, this, &cm,
						   &user->nickname, &param)) {
		     // Check which toggle string to add this to
		     if (toggle) {
			toggleOnStr = toggleOnStr + String(modes[i]);
			
			// If this mode had a parameter, add it to the list
			if (param.length()) {
			   toggleParamsOn = toggleParamsOn + " " + param;
			}
		     } else {
			toggleOffStr = toggleOffStr + String(modes[i]);
			
			// If this mode had a parameter, add it to the list
			if (param.length()) {
			   toggleParamsOff = toggleParamsOff + " " + param;
			}
		     }
		  }
	       } else {
		  // Complain to the user that they cannot change this mode
		  if (handler) {
		     handler->sendNumeric(TO_DAEMON->myServer(),
					  ERR_CANNOTCHANGECHANMODE, 0,
					  String::printf(LNG_ERR_CANNOTCHANGECHANMODE,
							 modes[i]));
		  }
	       }
	       
	       gotModeChar = true;
	    }
	 }
	 
	 // Check if we found a valid char. If not, complain about it if we can
	 if (!gotModeChar && handler) {
	    handler->sendNumeric(TO_DAEMON->myServer(),
				 ERR_UNKNOWNMODE, 0,
				 String::printf(LNG_ERR_UNKNOWNMODE,
						modes[i], 
						(char const *)name));
	 }
      }
   }

   // Assemble the mode change string
   String modeString = "";
   
   if (toggleOnStr.length()) {
      modeString = String('+') + toggleOnStr;
   }
   if (toggleOffStr.length()) {
      modeString = modeString + String('-') + toggleOffStr;
   }
   if (toggleParamsOn.length()) {
      modeString = modeString + toggleParamsOn;
   }
   if (toggleParamsOff.length()) {
      modeString = modeString + toggleParamsOff;
   }

   if (modeString.length()) {
      // Broadcast the change to local clients on this channel
      for (Channel::member_map_t::iterator it = members.begin();
	   it != members.end(); it++) {
	 if ((*it).second->user->local) {
	    (*it).second->user->local->handler->sendChannelMode(this, user, 
								modeString);
	 }
      }
      
      // Broadcast this change to the network
   }
}


/* isBan - Check if a user is on the channel ban list
 * Original 30/08/01, Simon Butcher <pickle@austnet.org>
 */
bool Channel::onBan(User *u)
{
   for (Channel::mask_list_t::iterator it = bans.begin();
	it != bans.end(); it++) {
      if ((*it)->mask.matches(u->getVWAddress().toLower()) || 
	  (*it)->mask.matches(u->getAddress().toLower())) {
	 return true;
      }
   }
   
   return false;
}


/* isBanExcept - Check if a user is on the channel ban exception list
 * Original 30/08/01, Simon Butcher <pickle@austnet.org>
 */
bool Channel::onBanExcept(User *u)
{
   for (Channel::mask_list_t::iterator it = exceptions.begin();
	it != exceptions.end(); it++) {
      if ((*it)->mask.matches(u->getVWAddress().toLower()) ||
	  (*it)->mask.matches(u->getAddress().toLower())) {
	 return true;
      }
   }
   
   return false;
}


/* isInvite - Check if a user is on the channel invite list
 * Original 30/08/01, Simon Butcher <pickle@austnet.org>
 */
bool Channel::onInvite(User *u)
{ 
   for (Channel::mask_list_t::iterator it = invites.begin();
	it != invites.end(); it++) {
      if ((*it)->mask.matches(u->getVWAddress().toLower()) ||
	  (*it)->mask.matches(u->getAddress().toLower())) {
	 return true;
      }
   }
   
  return false;
}


/* sendNotice - Send a NOTICE to this channel
 * Original 01/09/01, Simon Butcher <pickle@austnet.org>
 */
void Channel::sendNotice(User *from, String const &message)
{
   for (Channel::member_map_t::iterator it = members.begin();
	it != members.end(); it++) {
      // Check if this is the user who sent the message (we do not echo)
      if ((*it).second->user == from) {
	 continue;
      }
      
      // If this user is local, then this is very easy!
      if ((*it).second->user->local) {
	 // Real users?
	 if (!(modes & Channel::MODE_ANONYMOUS)) {
	    (*it).second->user->local->handler->sendNotice(from, this,
							   message);
	    continue;
	 }
	 
	 // Send the message under anonymous mask instead
	 (*it).second->user->local->handler->sendNoticeAnon(this, message);
	 continue;
      } 
      
      // stuff to route this message here.
   }
}


/* sendPrivmsg - Send a PRIVMSG to this channel
 * Original 01/09/01, Simon Butcher <pickle@austnet.org>
 */
void Channel::sendPrivmsg(User *from, String const &message)
{
   for (Channel::member_map_t::iterator it = members.begin();
	it != members.end(); it++) {
      // Check if this is the user who sent the message (we do not echo)
      if ((*it).second->user == from) {
	 continue;
      }
      
      // If this user is local, then this is very easy!
      if ((*it).second->user->local) {
	 // Real users?
	 if (!(modes & Channel::MODE_ANONYMOUS)) {
	    (*it).second->user->local->handler->sendPrivmsg(from, this, 
							    message);
	    continue;
	 }
	 
	 // Send the message under anonymous mask instead
	 (*it).second->user->local->handler->sendPrivmsgAnon(this, message);
	 continue;
      }
      
      // stuff to route this message here.
   }
}


/* getChannel - Look through our local channel list for a channel
 * Original 15/08/01, Simon Butcher <pickle@austnet.org>
 * Note: Minature version of what appears for the main channel list, why
 *       we do this is for speed purposes mainly
 */
Channel *User::getChannel(String &channel)
{
   // Look for this channel
   String chan = channel.IRCtoLower();
   Channel *c = channels[chan];
   
   // Make sure we got this user
   if (c) {
      return c;
   };
   
   // Else we gotta delete this empty record we just made and return nothing
   channels.erase(chan);

   return 0;
}


/* markAway - Toggle the user's away 
 * Original 11/09/01, Simon Butcher <pickle@austnet.org
 */
void User::markAway(String &message)
{
   // Set the away message
#ifdef DEBUG
   if (message.length()) {
#endif
      awayMessage = message;
#ifdef DEBUG
   } else {
      // Sanity, but we should never get here honestly...
      debug(String::printf("Null away message on %s - very wrong!!",
			   (char const *)nickname));
   }
#endif
   
   // Broadcast the change
   
}


/* isSilencing - Check if the given user matches a mask on our silence list
 * Original 24/09/01, Simon Butcher <pickle@austnet.org>
 */
bool User::isSilencing(User *target)
{
   // Run through the list to find the mask
   for (silence_list_t::iterator it = silences.begin();
	it != silences.end(); it++) {
      // Check for a match on either the real or virtual world addresses
      if (it->matches(target->getAddress()) ||
	  it->matches(target->getVWAddress())) {
	 return true;
      }
   }
   
   // Must not be on the list..
   return false;
}
