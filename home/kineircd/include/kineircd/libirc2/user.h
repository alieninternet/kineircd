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

#ifndef _INCLUDE_KINEIRCD_LIBIRC2_USER_H_
# define _INCLUDE_KINEIRCD_LIBIRC2_USER_H_ 1

# include <kineircd/user.h>
# include <kineircd/libirc2/output.h>

namespace Kine {
   namespace LibIRC2 {
      //! An IRC-2 user (refinement of a user with IRC-2 specific output)
      class User : virtual public Kine::User {
       private:
	 // The protocol output class to use
	 Output& protocol;

       protected:
	 //! Constructor
	 User(Output& _protocol)
	   : protocol(_protocol)
	   {};

       public:
	 //! Destructor
	 virtual ~User(void)
	   {};
      }; // class User
   }; // namespace LibIRC2
}; // namespace Kine

#endif // _INCLUDE_KINEIRCD_LIBIRC2_USER_H_
