/* $Id$
 * An abstract class for a channel
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

# include <kineircd/entity.h>
# include <kineircd/receiver.h>
# include <kineircd/client.h>

namespace Kine {
   class User;
   
   class Channel : public Receiver {
    public:
      //! Channel member class
      class Member {
       private:
	 Client& client;			//!< The member (a client)
	 
       public:
	 //! Constructor
	 Member(Client& _client)
	   : client(_client)
	   {};
	 
	 //! Destructor
	 ~Member(void)
	   {};
      };
      
    protected:
      //! Constructor
      Channel(void)
	{};
      
    public:
      //! Destructor
      virtual ~Channel(void)
	{};
      
      //! Return the name of the channel. This does not include the prefix char
      virtual const std::string& getChannelName(void) const = 0;
      
      //! Return the name of the channel, only with the prefix
      const std::string getChannelNameWithPrefix(void) const
	{ return ""; };
      
      //! Return the topic on the channel
      virtual const std::string& getTopic(void) const = 0;
      
      //! Return the number of users on the channel (0 = unknown/unavailable)
      virtual const unsigned long getUserCount(void) const
	{ return 0; };
      
      //! Is this channel hidden from public view (i.e. /whois, /list etc)
      virtual const bool isHidden(void) const
	{ return false; };
      
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

# include <kineircd/user.h>

#endif // _INCLUDE_KINEIRCD_CHANNEL_H_
