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

extern "C" {
# include <sys/time.h>
}

# include <kineircd/denizen.h>
# include <kineircd/sender.h>
# include <kineircd/receiver.h>
# include <kineircd/name.h>

namespace Kine {
   class Channel;
   
   //! An abstract class defining a client, since there are a few types
   class Client : public Denizen, public Sender, public Receiver {
    private:
      const timeval signonTime;			//!< Time this client connected
      
      // The channel list for this client
      typedef std::map < std::string, Channel* const > channels_type;
      channels_type channels;
      
    protected:
      //! Constructor
      Client(const timeval& _signonTime)
	: signonTime(_signonTime)
	{};
      
    public:
      //! Destructor
      virtual ~Client(void)
	{};
      
      //! Return the client's nickname
      virtual const Name& getNickname(void) const = 0;

      //! Return the 'name'
      const std::string& getName(void) const
	{ return getNickname(); };
      
      //! Return the client's user name
      virtual const std::string& getUsername(void) const = 0;

      //! Return the time this entity initially connected to the network
      const timeval& getSignonTime(void) const
	{ return signonTime; };
   }; // class Client
}; // namespace Kine

# include <kineircd/channel.h>

#endif // _INCLUDE_KINEIRCD_CLIENT_H_
