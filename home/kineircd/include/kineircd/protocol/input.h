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

#ifndef _INCLUDE_KINEIRCD_PROTOCOL_INPUT_H_
# define _INCLUDE_KINEIRCD_PROTOCOL_INPUT_H_ 1

# include <kineircd/protocol/base.h>

namespace Kine {
   namespace Protocol {
      //! Generic protocol input base
      class Input : virtual public Base {
       protected:
	 //! Constructor
	 explicit Input(void)
	   {};
	 
       public:
	 //! Destructor
	 virtual ~Input(void)
	   {};


	 /*!
	  * \brief Localise the given string
	  * 
	  * This will convert the given string (encoded in the previously
	  * configured character set) into a wide-string using the internal
	  * encoding.
	  * 
	  * Any data which is encoded and must be handled internally should
	  * be converted using this.
	  * 
	  * \param string The string (in the remote character set) to localise
	  * \return The string, in the internal character set
	  */
	 virtual const std::wstring
	   localiseStr(const std::string& string) const;
      }; // class Input
   }; // namespace Protocol
}; // namespace Kine

#endif // _INCLUDE_KINEIRCD_PROTOCOL_INPUT_H_

