/* $Id$
 * 
 * Copyright (c) 2003,2004 Simon Butcher <pickle@alien.net.au>
 * Copyright (c) 2003,2004 KineIRCd Development Team
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

#ifndef _SRC_MOD_IRC2USER_USER_H_
# define _SRC_MOD_IRC2USER_USER_H_ 1

# include <string>
# include <aisutil/time.h>
# include <kineircd/localuser.h>
# include <kineircd/registrant.h>
# include <kineircd/libirc2/user.h>

namespace Kine {
   namespace mod_irc2user {
      class Protocol;
      
      // A locally connected user, using the IRC-2 server <-> user protocol
      class User : public Kine::LibIRC2::User, public Kine::LocalUser {
       private:
	 Protocol& protocol;			// Connection we are bound to
	 
	 std::wstring description;		// Description/real name field
	 AIS::Util::Time lastAwake;		// Time user was last awake

	 // Events we want to know about
	 void doEventAwayToggle(const Denizen& changer);
	 void doEventChannelJoin(const Channel& channel,
				 const Client& client);
	 void doEventChannelPart(const Channel& channel,
				 const Client& client,
				 const std::wstring* const reason);
	 void doEventChannelTopicChange(const Channel& channel,
					const Denizen& entity);
	 void doEventLanguageChange(const Denizen& changer);
	 void doEventNicknameChange(const Denizen& changer,
				    const Kine::User& user,
				    const Client::Name& newNickname);
	 void doEventReceiveChannelMessage(Sender& from, const Channel& to,
					   const std::wstring& message);
	 void doEventReceiveChannelNotice(Sender& from, const Channel& to,
					  const std::wstring& message);
	 void doEventReceivePrivateMessage(Sender& from,
					   const std::wstring& message);
	 void doEventReceivePrivateNotice(Sender& from,
					  const std::wstring& message);
	 
       public:
	 // Constructor
	 explicit User(Protocol& _protocol,
		       const Kine::Registrant& _registrant,
		       const std::wstring& _hostname)
	   : Kine::User(_registrant.name,
			_registrant.username,
			_hostname,
			Kine::daemon().getTime()),
	     Kine::LibIRC2::User(reinterpret_cast<Kine::LibIRC2::Output&>(_protocol)),
	     protocol(_protocol),
	     description(_registrant.description),
	     lastAwake(Kine::daemon().getTime())
	   {};
	 
	 // Destructor
	 ~User(void)
	   {};
	 
	 // Return the description string
	 const std::wstring& getDescription(void) const
	   { return description; };
	 
	 // Return the time the user was last 'awake'
	 const AIS::Util::Time& getLastAwake(void) const
	   { return lastAwake; };
	 
	 // Prod the last awake field with the current time
	 void updateLastAwake(void)
	   { lastAwake = Kine::daemon().getTime(); };
      }; // class User
   }; // namespace mod_irc2user
}; // namespace Kine

# include "mod_irc2user/protocol.h"

#endif // _SRC_MOD_IRC2USER_USER_H_
   
