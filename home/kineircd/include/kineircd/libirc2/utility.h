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
# include <kineircd/server.h>

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
	   findMessageTarget(const std::wstring& target,
			     Receiver::Directivity& directivity,
			     const bool includeServices = false);


	 /*!
	  * \brief Find a server as a target
	  * 
	  * The IRC-2 protocol contains a querk where you can specify a
	  * client's name as a server, and the software will use the server
	  * that client is connected to. Also, you can specify a partial
	  * server name in the form of a mask, and the software will use the
	  * first match it finds.
	  * 
	  * If the mask given is not a mask, the server will first look for a
	  * server name, then a client's name.
	  * 
	  * If the mask given is a mask (it contains '*' or '?') then the
	  * server name list will be checked and the first matching server
	  * whose name matches the mask given will be returned.
	  * 
	  * \param name The mask of the entity to look for
	  * \return A pointer to a server record, or a null pointer
	  * \retval 0 A server was not found matching the name given
	  */
	 extern Server* const findServerTarget(const std::wstring& mask);


	 //! Make a user\@host string
	 inline static const std::wstring
	   makeUserHostIdent(const Kine::Client& client)
	   {
	      return (client.getUsername() + L'@' +
		      client.getHostname());
	   };
	 
	 //! Make a nick!user\@host string
	 inline static const std::wstring
	   makeNickUserHostIdent(const Kine::Client& client)
	   {
	      return (client.getNickname() + L'!' +
		      client.getUsername() + L'@' +
		      client.getHostname());
	   };
      }; // namespace Utility
   }; // namespace LibIRC2
}; // namespace Kine

#endif // _INCLUDE_KINEIRCD_IRC2_UTILITY_H_
   
