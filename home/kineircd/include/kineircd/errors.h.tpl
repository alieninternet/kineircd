[+ AutoGen5 template h +]
/* $Id$
 *
 * Copyright (c) 2000,2002,2003 Simon Butcher <pickle@alien.net.au>
 * Copyright (c) 2000,2002,2003 Alien Internet Services
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
 *
 *
[+(dne " * ")+]
 */

[+(make-header-guard "kine")+]

namespace Kine {
   /*!
    * \brief Error status returns
    *
    * Functions internally within the KineIRCd core often return operational
    * status using the type Error::error_type.
    */
   namespace Error {
      /*!
       * \brief Error returns from functions
       * 
       * Each function which returns an Error::error_type are returning
       * details on how the operation went.
       *
       * For convenience, this enumeration is designed so that errors are
       * \e negative values (below \c 0) and successful replies are \e positive
       * values (above \c 0). Unknown results are given a value of \e zero 
       * (\c 0).
       * 
       * In each function's documentation, a list of possible error returns
       * will be given. In your software, if relevant, you should try to
       * handle all, or at least as many of these replies as you can. You
       * \e should anticipate replies outside of those listed, however, as
       * future versions may incorporate additional responces, or unknown
       * errors may trigger a Kine::Error::UNKNOWN_ERROR responce.
       */
      enum error_type {
      	 //! No error, but no data returned either
	 NO_ERROR_OR_DATA = 2,
      
         //! No error
	 NO_ERROR = 1,
	 
	 //! Unknown error
	 UNKNOWN_ERROR = 0,[+FOR errors ","+]
	 
	 //! [+reason+]
	 [+name+] = -[+(+ (for-index) 1)+][+ENDFOR errors+]
      }; // enum error_type
   }; // namespace Error
}; // namespace Kine

#endif // [+(. header-guard)+]

