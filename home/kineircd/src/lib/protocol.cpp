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
 * Original 06/11/2003
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
 * Original 06/11/2003
 */
const std::wstring
  Protocol::Input::localiseStr(const std::string& string) const
{
   return Languages::toWideStr(string);
}
