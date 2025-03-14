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

#ifndef _INCLUDE_KINEIRCD_NAME_H_
# define _INCLUDE_KINEIRCD_NAME_H_ 1

# include <aisutil/string/string.h>
# include <kineircd/errors.h>

namespace Kine {
   /*!
    * \brief A generic IRC entity name
    *
    * A Name is a label for an Entity, conforming to specific rules with
    * regards to \e case-folding and \e validity (depending on how the server
    * is compiled and/or configured).
    * 
    * Real Entity names are derived from this abstract class.
    */
   class Name : public std::wstring {
    protected:
      //! Constructor
      Name(void)
	{};
      
      /*!
       * \brief Constructor
       * 
       * \param string A string to initialise this name with
       */
      template <class T>
	Name(const T& string)
	  : std::wstring(string)
	  {};
      
    public:
      //! Destructor
      virtual ~Name(void)
	{};
      
      /*!
       * \brief Convert the name to lower-case
       *
       * Lower-case the name based on compiled in case-mapping rules
       *
       * \return The converted name as a string
       */
      const std::wstring IRCtoLower(void) const;
      
      /*!
       * \brief Check if the name is valid
       * 
       * Check that the name stored is valid, based on whatever rules
       * necessary.
       * 
       * \return The validity of the name
       * \retval Kine::Error::NO_ERROR
       *    The name is considered to be valid
       */
      virtual const Error::error_type checkValidity(void) const = 0;
   }; // class Name
}; // namespace Kine

#endif // _INCLUDE_KINEIRCD_NAME_H_
