[+ AutoGen5 template cpp +]
/* $Id$
 * 
 * Copyright (c) 2003 Simon Butcher <pickle@alien.net.au>
 * Copyright (c) 2003 KineIRCd Development Team
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
 *
 *
[+(dne " * ")+]
 */

#include "kineircd/errors.h"
#include "lib/language.h"
 
using namespace Kine;


/* offsetError - Create an array offset
 * Original 20/08/2003 pickle
 * Note: UNKNOWN_ERROR is first, successes, then errors
 */
inline const unsigned int offsetError(const Error::error_type error)
{[+
   ;; We start the count at 1..
   (define count 1)
 +][+FOR successes+][+
   ;; Increase the count
   (set! count
      (+ count 1))
 +][+ENDFOR+]
   static const unsigned int offset = [+(. count)+];
   
   // An error?
   if (error <= 0) {
      return ((-error) + offset);
   }
   
   // It was an unknown error, or a success - just return it
   return error;
}


// A list of short names to use
struct {
   const char* const name;
   const Languages::tagID_type& tagID;
} static const errorsInfoList[] = {
    {
       "UNKNOWN_ERROR",
       Internal::Language::Kine_Error_UNKNOWN_ERROR
    },[+FOR successes+]
    {
       "[+name+]",
       Internal::Language::Kine_Error_[+name+]
    },[+ENDFOR+][+FOR errors ","+]
    {
       "[+name+]",
       Internal::Language::Kine_Error_[+name+]
    }[+ENDFOR+]
};

 
/* getErrorName - Return the short name of the error
 * Original 20/08/2003 pickle
 */
const char* const Error::getErrorName(const Error::error_type error)
{
   /* Return the error's name. Note how we look up the error using the
    * array with the error number, except with the sign simply reversed.
    */
   return errorsInfoList[offsetError(error)].name;
}

 
/* getErrorLanguageTag - Find an error reason language tag
 * Original 20/08/2003 pickle
 */
const Kine::Languages::tagID_type
  Error::getErrorLanguageTag(const Error::error_type error)
{
   // Return the appropriate tag ID
   return
     Internal::Language::tagMap[errorsInfoList[offsetError(error)].tagID].tagID;
}
