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

# include <kineircd/receiver.h>
# include <kineircd/denizen.h>
# include <kineircd/channelname.h>
# include <kineircd/errors.h>

namespace Kine {
   class Client;
   class User;
   
   /*!
    * \brief A channel
    *
    * An abstract class for a channel object. This doesn't specify the type
    * of the channel, though, or any specific scope (such whether the channel
    * is local to this server only, or is propagated over all servers on
    * this network).
    */
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
	 explicit Member(Client& _client,
			 Status::lazy_type _status = 0)
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
      const ChannelName name;			//!< The channel name
      const scope_type scope;			//!< Channel's 'scope'
      std::string topic;			//!< Channel topic
      std::string topicChanger;			//!< Who changed the topic
      AIS::Util::Time topicChangeTime;		//!< Time the topic changed

      // Channel members list
      members_type members;
      
    protected:
      //! Constructor
      explicit Channel(const ChannelName& _name,
		       const scope_type _scope,
		       const AIS::Util::Time& _creationTime)
	: Entity(_creationTime),
	  name(_name),
          scope(_scope)
	{}
      
    public:
      //! Destructor
      virtual ~Channel(void)
	{};
      
      
      /*!
       * \brief Return the channel's name
       * 
       * \return The channel's name
       */
      const ChannelName& getChannelName(void) const
	{ return name; };

      // Return the 'name' (generic form)
      const std::string& getName(void) const
	{ return getChannelName(); };

      //! Return the name of the channel, without the prefix character
      const std::string getNameWithoutPrefix(void) const
	{ return name.substr(1); };
      
      //! Return the channel prefix char
      const char getPrefix(void) const
	{ return name[0]; };
      
      
      //! Return the channel's scope
      const scope_type getScope(void) const
	{ return scope; };

      
      //! Return the time the channel was created (same as the 'signon' time)
      const AIS::Util::Time& getCreationTime(void) const
	{ return getSignonTime(); };
      
      
      //! Return the topic on the channel (empty string for no topic)
      const std::string& getTopic(void) const
	{ return topic; };
      
      //! Return the identify of the person who changed the topic last
      const std::string& getTopicChanger(void) const
	{ return topicChanger; };

      //! Return the time the topic was changed
      const AIS::Util::Time& getTopicChangeTime(void) const
	{ return topicChangeTime; };
      
      //! Does this channel have a topic?
      const bool hasTopic(void) const
	{ return (!topic.empty()); };
      
      //! Change the topic on the channel
      const Error::error_type changeTopic(const Denizen& who,
					  const std::string& newTopic)
	{ return Error::UNKNOWN_ERROR; };
      
      //! Remove the topic on the channel (same as setting an empty topic)
      const Error::error_type removeTopic(const Denizen& who)
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
      
      /*!
       * \brief Return the number of clients on the channel
       *
       * This will return the number of clients currently listed as \e members
       * on this channel. Depending on the channel type and the flags set,
       * this may or may not be the number of people which have access to the
       * channel. 
       * 
       * \retrun The number of clients listed as members of the channel
       */
      const unsigned long getUserCount(void) const
	{ return members.size(); };

      
      //! Is this channel secret?
      const bool isSecret(void) const
	{ return false; };
      
      //! Is this channel private?
      const bool isPrivate(void) const
	{ return false; };

      // Is this channel hidden from public view (i.e. /whois, /list etc)
      virtual const bool isHiddenFrom(const User& user) const
	{ return (isSecret() || isPrivate()); };


      //! Is the given client on the channel?
      const bool hasClient(const Client& who) const
	{ return false; /* temporary ? */ };

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
