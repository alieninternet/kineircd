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

namespace Kine {
   class Channel : public Entity, public Receiver {
    private:
      const std::string name;			//!< Non-prefixed channel name
      
    protected:
      //! Constructor
      Channel(void)
	{};
      
    public:
      //! Destructor
      virtual ~Channel(void)
	{};
      
      // Return the name of the channel. This does not include the prefix char
      const std::string& getChannelName(void) const
	{ return name; };
      
      // Return the name of the channel, only with the prefix
      const std::string getChannelNameWithPrefix(void) const;
   }; // class Channel
}; // namespace Kine

#endif // _INCLUDE_KINEIRCD_CHANNEL_H_
