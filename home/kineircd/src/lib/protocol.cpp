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

#ifdef HAVE_CONFIG_H
# include "autoconf.h"
#endif
#include "kineircd/kineircdconf.h"

#include "kineircd/protocol/input.h"
#include "kineircd/protocol/output.h"
#include "kineircd/languages.h"
#include "lib/debug.h"

using namespace Kine;

// The conversion output buffer size
#define ICONV_BUFFER_SIZE 2048 // 2kB


/* convertChars - Convert the given string using the given iconv descriptor..
 * Original 07/11/2003 pickle
 */
template <class outT, class inT>
  inline static const outT
  convertChars(iconv_t& convDesc, const inT& input)
{
   outT outputString;
   
   // Make sure the conversion descriptor is okay..
   if (convDesc == (iconv_t)(-1)) {
#ifdef KINE_DEBUG
      // Eek!
      abort();
#else
      // Nothing we can do, really.. *sigh*
      return outputString;
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
       case EILSEQ: // An illegal multibyte sequence was found in the input
	 // Copy everything from the output buffer into the data string
	 outputString.append((typename outT::pointer)outputBuffer,
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
	 return outputString;
	 
       default:
	 // Unknown error.. this should not happen
#ifdef KINE_DEBUG
	 abort();
#else
	 return outputString;
#endif
      }
   }
   
   // If there's anything else left in the buffer after all of that, copy it
   register const size_t outputCharCount =
     ((ICONV_BUFFER_SIZE - outputBytes) / sizeof(outT::value_type));
   if (outputCharCount > 0) {
      outputString.append((typename outT::pointer)outputBuffer,
			  outputCharCount);
   }
   
   // Return the (converted) string..
   return outputString;
}


/* delocaliseStr - Convert the string from the internal character set
 * Original 06/11/2003 pickle
 */
const std::string
  Protocol::Output::delocaliseStr(const std::wstring& string) const
{
   return convertChars<std::string, std::wstring>(outputCharConvDesc, string);
}


/* localiseStr - Convert the string into the internal character set
 * Original 06/11/2003 pickle
 */
const std::wstring
  Protocol::Input::localiseStr(const std::string& string) const
{
   return convertChars<std::wstring, std::string>(inputCharConvDesc, string);
}


/* changeConvDesc - Change the given charset conversion descriptor..
 * Original 06/11/2003 pickle
 */
inline static const Error::error_type
  changeConvDesc(iconv_t& convDesc,
		 const char* const charsetTo,
		 const char* const charsetFrom)
{
#ifdef KINE_DEBUG
   std::ostringstream out;
   out << "::changeConvDesc - descriptor @ " << &convDesc << " to " <<
     charsetFrom << " -> " << charsetTo;
   debug(out.str());
#endif

   // Close the previous character set descriptor, if needs be..
   if (convDesc != (iconv_t)(-1)) {
      // Try to close it..
#ifdef KINE_DEBUG_ASSERT
      assert(iconv_close(convDesc) == 0);
#else
      (void)iconv_close(convDesc);
#endif
   }

   // Try to open the new character set conversion descriptor
   if ((convDesc = iconv_open(charsetTo, charsetFrom)) != (iconv_t)(-1)) {
      // Prod the converstion descriptor, since this is sometimes necessary
      (void)iconv(convDesc, NULL, NULL, NULL, NULL);
			   
      // All happy!
      return Error::NO_ERROR;
   }

   // Check if the charset was invalid for some reason..
   if (errno == EINVAL) {
      return Error::UNKNOWN_CHARSET;
   }

   // No idea what happened..
   return Error::UNKNOWN_ERROR;
}


/* changeOutputCharset - Change the output charset conversion descriptor
 * Original 06/11/2003 pickle
 */
const Error::error_type
  Protocol::Output::changeOutputCharset(const char* const charset)
{
   return changeConvDesc(outputCharConvDesc, charset, KINE_INTERNAL_CHARSET);
}


/* changeInputCharset - Change the input charset conversion descriptor
 * Original 06/11/2003 pickle
 */
const Error::error_type
  Protocol::Input::changeInputCharset(const char* const charset)
{
   return changeConvDesc(inputCharConvDesc, KINE_INTERNAL_CHARSET, charset);
}
