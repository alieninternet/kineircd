/* $Id$
 * 
 * Copyright (c) 2001,2002,2003 Simon Butcher <pickle@alien.net.au>
 * Copyright (c) 2001,2002,2003 KineIRCd Development Team
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

#ifndef _INCLUDE_KINEIRCD_CLIENT_H_
# define _INCLUDE_KINEIRCD_CLIENT_H_ 1

# include <string>
# include <map>
# include <aisutil/time.h>

# include <kineircd/denizen.h>
# include <kineircd/sender.h>
# include <kineircd/receiver.h>
# include <kineircd/clientname.h>

namespace Kine {
   class Channel;
   
   //! An abstract class defining a client, since there are a few types
   class Client : public Denizen, public Sender, public Receiver {
    private:
      std::string hostname;			//!< Client's hostname
      
      // The channel list for this client
      typedef std::map < std::string, Channel* const > channels_type;
      channels_type channels;
      
    protected:
      //! Constructor
      Client(const std::string& _hostname,
	     const AISutil::Time& _signonTime)
	: Denizen(_signonTime),
          hostname(_hostname)
	{};

      //! An event called when someone (maybe us) has joined a channel
      virtual void doEventChannelJoin(const Channel& channel,
				      const Client& client)
	{};

      //! An event called when someone (maybe us) has parted a channel
      virtual void doEventChannelPart(const Channel& channel,
				      const Client& client,
				      const std::string* const reason)
	{};

      //! An event called whenever a channel's topic has changed
      virtual void doEventChannelTopicChange(const Channel& channel,
					     const Entity& entity)
	{};
      
      //! An event called when a message was successfully sent to a channel
      virtual void doEventReceiveChannelMessage(Entity& from,
						const Channel& to,
						const std::string& message)
	{};
      
      //! An event called when a notice was successfully sent to a channel
      virtual void doEventReceiveChannelNotice(Entity& from,
					       const Channel& to,
					       const std::string& message)
	{};

    public:
      //! Destructor
      virtual ~Client(void)
	{};
      
      //! Return the client's nickname
      virtual const ClientName& getNickname(void) const = 0;

      //! Return the client's nickname (which is also its unique 'name')
      const std::string& getName(void) const
	{ return getNickname(); };
      
      //! Return the client's user name
      virtual const std::string& getUsername(void) const = 0;

      //! Return the client's hostname
      const std::string& getHostname(void) const
	{ return hostname; };

      
      //! Make a user@host string
      const std::string makeUserHostIdent(void) const
	{ return getUsername() + '@' + getHostname(); };
      
      //! Make a nick!user@host string
      const std::string makeNickUserHostIdent(void) const
	{ return getNickname() + '!' + getUsername() + '@' + getHostname(); };
   }; // class Client
}; // namespace Kine

# include <kineircd/channel.h>

#endif // _INCLUDE_KINEIRCD_CLIENT_H_
