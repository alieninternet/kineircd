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
 */

#ifndef _SRC_LIB_CHARCONV_H_
# define _SRC_LIB_CHARCONV_H_ 1

# ifdef HAVE_CONFIG_H
#  include "autoconf.h"
# endif
# ifdef HAVE_ICONV_H
#  include "iconv.h"
# endif

# ifndef HAVE_ICONV
#  error "No iconv() support on this system: charset conversion cannot be done"
# endif

// The conversion output buffer size
# define ICONV_BUFFER_SIZE 2048

namespace Kine {
   namespace Internal {
      /* convertChars - Convert the given string using the given descriptor
       * Original 07/11/2003 pickle
       */
      template <class inT, class outT>
	inline const bool
	convertChars(iconv_t& convDesc, const inT& input, outT& output)
	  {
	     // Make sure the conversion descriptor is okay..
	     if (convDesc == (iconv_t)(-1)) {
#ifdef KINE_DEBUG
		// Eek!
		abort();
#else
		// Nothing we can do, really.. *sigh*
		return false;
#endif
	     }
	     
	     // Stuff we need for the conversion
	     char outputBuffer[ICONV_BUFFER_SIZE];
	     size_t inputBytes = (input.length() * sizeof(inT::value_type));
	     size_t outputBytes = ICONV_BUFFER_SIZE;
	     char* inputPtr = (char*)input.data();
	     char* outputPtr = outputBuffer;
	     
	     // Poke iconv() - reset the output buffer
	     (void)iconv(convDesc, NULL, NULL, &outputPtr, &outputBytes);
	     
	     // Convert the data..
	     for (;;) {
		const size_t convVal = iconv(convDesc,
					     &inputPtr, &inputBytes,
					     &outputPtr, &outputBytes);
		
		// Check to see if iconv() is happy or not..
		if (convVal != (size_t)(-1)) {
		   // Break the loop, we're done here
		   break;
		}
	     
		// An error occurred.. Check what it was..
		switch (errno) {
		 case E2BIG: // No more room left in the output buffer
		 case EILSEQ: // An illegal multibyte sequence was found
		   // Copy what is in the output buffer to the output string
		   output.append((reinterpret_cast<typename outT::pointer>
				  (outputBuffer)),
				 ((ICONV_BUFFER_SIZE - outputBytes) /
				  sizeof(outT::value_type)));
		   
		   // Reset the output buffer stuff
		   outputBytes = ICONV_BUFFER_SIZE;
		   outputPtr = outputBuffer;
		   continue;
		   
		 case EINVAL: // An incomplete multibyte sequence was found..
		   // We should deal with this better..
#ifdef KINE_DEBUG
# warning "Poor support for illegal multibyte sequences.."
#endif
		   return false;
		}
		
		// If we got here, something broke..
		return false;
	     }
	     
	     // If there's anything else left in the buffer, copy that too
	     register const size_t outputCharCount =
	       ((ICONV_BUFFER_SIZE - outputBytes) / sizeof(outT::value_type));
	     if (outputCharCount > 0) {
		output.append((reinterpret_cast<typename outT::pointer>
			       (outputBuffer)),
			      outputCharCount);
	     }
	     
	     // Return happy!
	     return true;
	  }
   }; // namespace Internal
}; // namespace Kine

// Undefine the buffer size thingy.. it is no longer needed..
# undef ICONV_BUFFER_SIZE

#endif // _SRC_LIB_CHARCONV_H_
