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

#ifndef _INCLUDE_KINEIRCD_CHANNEL_H_
# define _INCLUDE_KINEIRCD_CHANNEL_H_ 1

# include <map>
# include <aisutil/time.h>

# include <kineircd/entity.h>
# include <kineircd/receiver.h>
# include <kineircd/errors.h>

namespace Kine {
   class Client;
   class User;
   
   //! An abstract class for a channel
   class Channel : public Receiver {
    public:
      //! Channel member class
      class Member {
       public:
	 struct Status { // <=- namespace?
	    enum flag_type {
	       SERVICE		= 0x01, // +.
	       CREATOR		= 0x02, // +O
	       OPERATOR		= 0x04, // +o
	       HALF_OPERATOR	= 0x08, // +h
	       VOICED		= 0x10  // +v
	    };
	    
	    typedef int lazy_type;
	 };
	 
       private:
	 Client& client;			//!< The member (a client)
	 Status::lazy_type status;		//!< Membership status
	 
       public:
	 //! Constructor
	 Member(Client& _client, Status::lazy_type _status = 0)
	   : client(_client),
	     status(_status)
	   {};
	 
	 //! Destructor
	 ~Member(void)
	   {};
	 
	 //! Return the client object associated with this member
	 Client& getClient(void) const
	   { return client; };
	 
	 //! Return the membership status of this client
	 const Status::lazy_type getStatus(void) const
	   { return status; };
      };
      
      // The channel member list type
      typedef std::map < std::string, Member* const > members_type;
      
      //! The 'scopes' a channel may have
      enum scope_type {
	 LOCAL,				//!< Channel only exists on this server
	 NETWORK,			//!< Channel exists on this network
	 GLOBAL				//!< Channel is 'global' via IIRC links
      };

    private:
      std::string channelName;			//!< The channel name
      std::string channelNameWithPrefix;	//!< Name with prefix
      const scope_type scope;			//!< Channel's 'scope'
      const AISutil::Time creationTime;		//!< Channel creation time
      std::string topic;			//!< Channel topic
      std::string topicChanger;			//!< Who changed the topic
      AISutil::Time topicChangeTime;		//!< Time the topic changed

      // Channel members list
      members_type members;
      
    protected:
      //! Constructor
      Channel(const scope_type _scope,
	      const AISutil::Time& _creationTime)
	: scope(_scope),
          creationTime(_creationTime)
	{}
      
    public:
      //! Destructor
      virtual ~Channel(void)
	{};
      
      
      //! Return the name of the channel. This does not include the prefix char
      const std::string& getChannelName(void) const
	{ return channelName; };

      //! Return the name of the channel, only with the prefix
      virtual const std::string& getChannelNameWithPrefix(void) const
	{ return channelNameWithPrefix; };

      //! Return the 'name'
      const std::string& getName(void) const
	{ return getChannelNameWithPrefix(); };
      
      
      //! Return the channel's scope
      const scope_type getScope(void) const
	{ return scope; };

      
      //! Return the time the channel was created
      const AISutil::Time& getCreationTime(void) const
	{ return creationTime; };
      
      
      //! Return the topic on the channel (empty string for no topic)
      const std::string& getTopic(void) const
	{ return topic; };
      
      //! Return the identify of the person who changed the topic last
      const std::string& getTopicChanger(void) const
	{ return topicChanger; };

      //! Return the time the topic was changed
      const AISutil::Time& getTopicChangeTime(void) const
	{ return topicChangeTime; };
      
      //! Does this channel have a topic?
      const bool hasTopic(void) const
	{ return (!topic.empty()); };
      
      //! Change the topic on the channel
      const Error::error_type changeTopic(const Entity& who,
					  const std::string& newTopic)
	{ return Error::UNKNOWN_ERROR; };
      
      //! Remove the topic on the channel (same as setting an empty topic)
      const Error::error_type removeTopic(const Entity& who)
	{ return Error::UNKNOWN_ERROR; };

      
      //! Return the channel's URL, if it has one
      virtual const std::string* const getURL(void) const
	{ return 0; };

      //! Does this channel have a URL?
      virtual const bool hasURL(void) const
	{ return (getURL() != 0); };
      

      //! Return the channel members list (read only)
      const members_type& getMembers(void) const
	{ return members; };
      
      //! Return the number of users on the channel (0 = unknown/unavailable)
      const unsigned long getUserCount(void) const
	{ return members.size(); };

      
      //! Is this channel secret?
      const bool isSecret(void) const
	{ return false; };
      
      //! Is this channel private?
      const bool isPrivate(void) const
	{ return false; };
      
      //! Is this channel hidden from public view (i.e. /whois, /list etc)
      virtual const bool isHidden(void) const
	{ return (isSecret() || isPrivate()); };
      
      
      //! Is the given user banned? (+b list)
      virtual const bool isBanned(const User& who) const
	{ return false; };
      
      //! Is the given user exempt from being banned? (+e list)
      virtual const bool isBanExempt(const User& who) const
	{ return false; };
      
      //! Is the given user automatically invited? (+I list)
      virtual const bool isAutomaticallyInvited(const User& who) const
	{ return false; };
      
      //! Is the given user currently invited to the channel?
      virtual const bool isInvited(const User& who) const
	{ return false; };
   }; // class Channel
}; // namespace Kine

# include <kineircd/client.h>
# include <kineircd/user.h>

#endif // _INCLUDE_KINEIRCD_CHANNEL_H_
