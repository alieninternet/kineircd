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


/* delocaliseStr - Convert the string from the internal character set
 * Original 06/11/2003 pickle
 */
const std::string
  Protocol::Output::delocaliseStr(const std::wstring& string) const
{
   std::string out;
   for (std::wstring::const_iterator it = string.begin();
	it != string.end();
	++it) {
      out += (char)((unsigned long)(*it) % 0xFF);
   }
   return out;
}


/* localiseStr - Convert the string into the internal character set
 * Original 06/11/2003 pickle
 */
const std::wstring
  Protocol::Input::localiseStr(const std::string& string) const
{
   return Languages::toWideStr(string);
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
