/* $Id$
 * 
 * Copyright (c) 2001,2003 Simon Butcher <pickle@alien.net.au>
 * Copyright (c) 2001,2003 KineIRCd Development Team
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

#ifndef _INCLUDE_KINEIRCD_IRC2_UTILITY_H_
# define _INCLUDE_KINEIRCD_IRC2_UTILITY_H_ 1

# include <string>
# include <kineircd/user.h>
# include <kineircd/receiver.h>

namespace Kine {
   namespace LibIRC2 {
      /*!
       * \brief IRC-2 protocol related utilities
       * 
       * \ingroup LibIRC2
       */
      namespace Utility {
	 //! Find a message's destination target based on the given string
	 extern Receiver* const
	   findMessageTarget(const std::string& target,
			     Receiver::Directivity& directivity,
			     const bool includeServices = false);
	 
	 //! Make a user\@host string
	 inline static const std::string
	   makeUserHostIdent(const Kine::Client& client)
	   {
	      return (client.getUsername() + '@' +
		      client.getHostname());
	   };
	 
	 //! Make a nick!user\@host string
	 inline static const std::string
	   makeNickUserHostIdent(const Kine::Client& client)
	   {
	      return (client.getNickname() + '!' +
		      client.getUsername() + '@' +
		      client.getHostname());
	   };
      }; // namespace Utility
   }; // namespace LibIRC2
}; // namespace Kine

#endif // _INCLUDE_KINEIRCD_IRC2_UTILITY_H_
   
