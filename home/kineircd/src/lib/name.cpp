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

#ifdef HAVE_CONFIG_H
# include "autoconf.h"
#endif
#include "kineircd/kineircdconf.h"

#include "kineircd/name.h"

using namespace Kine;
using AISutil::String;


/* IRCtoLower - Convert to lowercase while considering irc chars (eg {} and [])
 * Original 12/08/2001 pickle
 * 26/02/2002 pickle - Converted for use with std::string / AISutil::String
 */
const AISutil::String Name::IRCtoLower(void) const
{
   char *temp = new char[length() + 1];
   
   for (register size_type i = (length() + 1); i--;) {
      char ch = (*this)[i];
      
      switch (ch) {
       case '[':
	 temp[i] = '{';
	 continue;
	 
       case ']':
	 temp[i] = '}';
	 continue;
	 
       case '\\':
	 temp[i] = '|';
	 continue;
	 
       case '~':
	 temp[i] = '^';
	 continue;
	 
       default:
	 temp[i] = tolower(ch);
      }
   }
   
   String result(temp);
   delete temp;
   
   return result;
}
