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

#ifndef _INCLUDE_KINEIRCD_PROTOCOL_OUTPUT_H_
# define _INCLUDE_KINEIRCD_PROTOCOL_OUTPUT_H_ 1

# include <kineircd/protocol/base.h>
# include <kineircd/connection.h>
# include <kineircd/errors.h>
# include <iconv.h>

namespace Kine {
   namespace Protocol {
      //! Generic protocol output base
      class Output : virtual public Base {
       private:
	 //! iconv() output conversion descriptor
	 iconv_t outputCharConvDesc;


       protected:
	 //! The specific connection running this instance of the protocol
	 Connection& connection;
	 
	 
	 //! Constructor
	 explicit Output(Connection& _connection)
	   : outputCharConvDesc((iconv_t)(-1)),
	     connection(_connection)
	   {};
	 
       public:
	 //! Destructor
	 virtual ~Output(void)
	   {
	      if (outputCharConvDesc != (iconv_t)(-1)) {
		 (void)iconv_close(outputCharConvDesc);
	      }
	   };

      
	 //! Delocalise the given text into the external character set
	 /*!
	  * \brief Delocalise the given string
	  * 
	  * This will convert the given string (internally encoded) into the
	  * encoding previously configured for the remote connection.
	  * 
	  * Any internationalised internal strings which must be properly
	  * encoded should be converted using this function.
	  * 
	  * \param string The internally encoded wide-string
	  * \return The string, encoded appropriately for the remote connection
	  */
	 const std::string delocaliseStr(const std::wstring& string) const;
	 
	 /*!
	  * \brief Change the output character set
	  * 
	  * \param charset The name of the character set to switch to
	  * \return Whether the given character set is now being used or not
	  * \retval Error::NO_ERROR
	  *    The given character set is now being used
	  * \retval Error::UNKNOWN_CHARSET
	  *    The given character set is unknown/invalid/unsupported
	  */
	 const Error::error_type changeOutputCharset(const char* const charset);
      }; // class Output
   }; // namespace Protocol
}; // namespace Kine

#endif // _INCLUDE_KINEIRCD_PROTOCOL_OUTPUT_H_

