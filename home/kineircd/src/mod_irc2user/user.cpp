/* $Id$
 * 
 * Copyright (c) 2003 Simon Butcher <pickle@alien.net.au>
 * Copyright (c) 2003 KineIRCd Development Team
 * (See DEV-TEAM file for details)
 *
 * This file is a part of KineIRCd.
 * 
 * KineIRCd is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * KineIRCd is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with KineIRCd; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifdef HAVE_CONFIG_H
# include "autoconf.h"
#endif

#include "mod_irc2user/user.h"
#include "mod_irc2user/language.h"

using namespace Kine::mod_irc2user;


// Alternative GETLANG() macro (since the getLanguageList() call is local)
# define GETLANG(n,...) \
   protocol.delocaliseStr(Kine::languages().get(getLanguageList(), \
			       Language::tagMap[Language::n].tagID, \
                               ##__VA_ARGS__))



/* doEventAwayToggle - Called when we are going away/returning away
 * Original 16/04/2003 simonb
 */
void User::doEventAwayToggle(const Denizen& changer)
{
   // Have we gone away or are we returning?
   if (isAway()) {
      // Should we return the away message back to the user?
      if (true /* configurable */) {
	 // Tell the user they are away, and the message they gave us
	 protocol.sendNumeric(LibIRC2::Numerics::RPL_NOWAWAY,
			      GETLANG(irc2_RPL_NOWAWAY_WITH_MESSAGE,
				      getAwayMessage()));
      } else {
	 // Tell the user they are now known to be away
	 protocol.sendNumeric(LibIRC2::Numerics::RPL_NOWAWAY,
			      GETLANG(irc2_RPL_NOWAWAY));
      }
      return;
   }
   
   // Tell the user they are now back, as far as we know
   protocol.sendNumeric(LibIRC2::Numerics::RPL_UNAWAY,
			GETLANG(irc2_RPL_UNAWAY));
}


/* doEventChannelJoin - Called when someone (maybe us) is joining a channel
 * Original 05/05/2003 simonb
 */
void User::doEventChannelJoin(const Channel& channel,
			      const Client& client)
{
   static const char* const commandName = "JOIN";
   
   protocol.sendMessageFrom(client, commandName,
			    protocol.delocaliseStr(channel.getName()));
   
   // If this is us, then we need to send information about the channel
   if (&client == this) {
      // Send the names on the channel...
      protocol.sendNames(channel);
      
      // Send the end of names numeric (most important as sendNAMES does not!)
      protocol.sendNumeric(LibIRC2::Numerics::RPL_ENDOFNAMES,
			   protocol.delocaliseStr(channel.getName()),
			   GETLANG(irc2_RPL_ENDOFNAMES));

      // Send the channel mode information
      protocol.sendNumeric(LibIRC2::Numerics::RPL_CHANNELMODEIS,
			   protocol.delocaliseStr(channel.getName()),
			   "+" /* <=- evil; needs to be fixed */);

      // Does this channel have a topic set?
      if (channel.hasTopic()) {
	 // Send the channel topic
	 protocol.sendNumeric(LibIRC2::Numerics::RPL_TOPIC,
			      protocol.delocaliseStr(channel.getName()),
			      protocol.delocaliseStr(channel.getTopic()));
	 
	 // Send the person who set the topic, and the time it was set
	 protocol.sendNumeric(LibIRC2::Numerics::RPL_TOPICWHOTIME,
			      protocol.delocaliseStr(channel.getName()),
			      protocol.delocaliseStr(channel.getTopicChanger()),
			      channel.getTopicChangeTime().seconds);
      }
      
      // Send the channel creation time
      protocol.sendNumeric(LibIRC2::Numerics::RPL_CREATIONTIME,
			   protocol.delocaliseStr(channel.getName()),
			   channel.getCreationTime().seconds);
      
      // Does this channel have a URL?
      if (channel.hasURL()) {
	 // Send the channel's URL
	 protocol.sendNumeric(LibIRC2::Numerics::RPL_CHANNEL_URL,
			      protocol.delocaliseStr(channel.getName()),
			      protocol.delocaliseStr(*channel.getURL()));
      }
   }
}


/* doEventChannelPart - Called when someone (maybe us) is parting a channel
 * Original 05/05/2003 simonb
 */
void User::doEventChannelPart(const Channel& channel,
			      const Client& client,
			      const std::wstring* const reason)
{
   static const char* const commandName = "PART";
   
   // Send the a part message, depending on whether or not a reason was given
   if (reason == 0) {
      protocol.sendMessageFrom(client, commandName,
			       protocol.delocaliseStr(channel.getName()));
   } else {
      protocol.sendMessageFrom(client, commandName,
			       protocol.delocaliseStr(channel.getName()),
			       protocol.delocaliseStr(*reason));
   }
}


/* doEventChannelTopicChange - Called when the topic on a channel has changed
 * Original 05/05/2003 simonb
 */
void User::doEventChannelTopicChange(const Channel& channel,
				     const Denizen& entity)
{
   static const char* const commandName = "TOPIC";

   protocol.sendMessageFrom(protocol.delocaliseStr(entity.getName()),
			    commandName,
			    protocol.delocaliseStr(channel.getName()),
			    protocol.delocaliseStr(channel.getTopic()));
}


/* doEventLanguageChange - Called when the language list has been modified
 * Original 16/04/2003 simonb
 */
void User::doEventLanguageChange(const Denizen& changer)
{
   protocol.sendLanguageList();
}


/* doEventNicknameChange - Called when a nickname has been changed
 * Original 16/05/2003 simonb
 */
void User::doEventNicknameChange(const Denizen& changer,
				 const Kine::User& user,
				 const ClientName& newNickname)
{
   static const char* const commandName = "NICK";
   
   // Tell the user their nickname has changed
   protocol.sendMessageFrom(static_cast<const Client&>(user), commandName,
			    protocol.delocaliseStr(newNickname));
}


/* doEventReceiveChannelMessage - Called when a channel has received a message
 * Original 01/08/2001 simonb
 */
void User::doEventReceiveChannelMessage(Sender& from, const Channel& to,
					const std::wstring& message)
{
   static const char* const commandName = "PRIVMSG";
   
   // Try to cast this as a client..
   Client* const client = dynamic_cast<Client* const>(&from);
   
   // Is it a client?
   if (client != 0) {
      protocol.sendMessageFrom(*client, commandName,
			       protocol.delocaliseStr(to.getName()),
			       protocol.delocaliseStr(message));
      return;
   }
   
   // It was not a client, use the entity's generic name
   protocol.sendMessageFrom(protocol.delocaliseStr(from.getName()),
			    commandName,
			    protocol.delocaliseStr(to.getName()),
			    protocol.delocaliseStr(message));
}


/* doEventReceiveChannelNotice - Called when a channel has received a notice
 * Original 01/08/2001 simonb
 */
void User::doEventReceiveChannelNotice(Sender& from, const Channel& to,
				       const std::wstring& message)
{
   static const char* const commandName = "NOTICE";
   
   // Try to cast this as a client..
   Client* const client = dynamic_cast<Client* const>(&from);
   
   // Is it a client?
   if (client != 0) {
      protocol.sendMessageFrom(*client, commandName,
			       protocol.delocaliseStr(to.getName()),
			       protocol.delocaliseStr(message));
      return;
   }
   
   // It was not a client, use the entity's generic name
   protocol.sendMessageFrom(protocol.delocaliseStr(from.getName()),
			    commandName,
			    protocol.delocaliseStr(to.getName()),
			    protocol.delocaliseStr(message));
}


/* doEventReceivePrivateMessage - Called when we have received a message
 * Original 01/08/2001 simonb
 */
void User::doEventReceivePrivateMessage(Sender& from,
					const std::wstring& message)
{
   static const char* const commandName = "PRIVMSG";
   
   // Try to cast this as a client..
   Client* const client = dynamic_cast<Client* const>(&from);
   
   // Is it a client?
   if (client != 0) {
      protocol.sendMessageFrom(*client, commandName,
			       protocol.delocaliseStr(getName()),
			       protocol.delocaliseStr(message));
      return;
   }
   
   // It was not a client, use the entity's generic name
   protocol.sendMessageFrom(protocol.delocaliseStr(from.getName()),
			    commandName,
			    protocol.delocaliseStr(getName()),
			    protocol.delocaliseStr(message));
}


/* doEventReceivePrivateNotice - Called when we have received a notice
 * Original 01/08/2001 simonb
 */
void User::doEventReceivePrivateNotice(Sender& from,
				       const std::wstring& message)
{
   static const char* const commandName = "NOTICE";
   
   // Try to cast this as a client..
   Client* const client = dynamic_cast<Client* const>(&from);
   
   // Is it a client?
   if (client != 0) {
      protocol.sendMessageFrom(*client, commandName,
			       protocol.delocaliseStr(getName()),
			       protocol.delocaliseStr(message));
      return;
   }
   
   // It was not a client, use the entity's generic name
   protocol.sendMessageFrom(protocol.delocaliseStr(from.getName()),
			    commandName,
			    protocol.delocaliseStr(getName()),
			    protocol.delocaliseStr(message));
}
