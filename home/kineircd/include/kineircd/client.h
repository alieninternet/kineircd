/* $Id$
 * An abstract class defining a client, since there are a few types
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

# include <kineircd/entity.h>
# include <kineircd/sender.h>
# include <kineircd/receiver.h>
# include <kineircd/server.h>
# include <kineircd/channel.h>

namespace Kine {
   class Client : public Entity, public Sender, public Receiver {
    private:
      std::string nickname;			// Nickname
      std::string description;			// Description/GECOS/Real name
      Server* server;				// Server this client is on
      
      // The channel list for this client
      typedef std::map < std::string, Channel* const > channels_type;
      channels_type channels;
      
      
    protected:
      // Constructor
      Client(void)
	{};
      
    public:
      // Destructor
      virtual ~Client(void)
	{};
      
      // Return variables
      const std::string& getNickname(void) const
	{ return nickname; };
      const std::string& getDescription(void) const
	{ return description; };
   };
};

#endif // _INCLUDE_KINEIRCD_CLIENT_H_
