/* $Id$
 * Language data container (one per language)
 *
 * Copyright (c) 2002,2003 Simon Butcher <pickle@alien.net.au>
 * Copyright (c) 2002,2003 KineIRCd Development Team
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

#ifdef KINE_DEBUG
# include <sstream>
#endif

#include "kineircd/languagedata.h"
#include "libkineircd/debug.h"

using namespace Kine;


/* ~LanguageData - Destructor
 * Original 20/03/2003 simonb
 */
LanguageData::~LanguageData(void)
{
   // Erase the language data vector
   while (!tagData.empty()) {
      // If this is a valid entry, delete it
      if (tagData.back() != 0) {
	 delete tagData.back();
      }
      
      // Erase this entry
      tagData.pop_back();
   }
}


/* findTag - Look for a given TID's data, and return it if possible
 * Original 16/03/2003 simonb
 */
const std::string* const 
  LanguageData::findTag(const Languages::tagID_type tagID) const
{
   // Make sure the tag is valid..
   if ((tagID == 0) || (tagID > tagData.size())) {
#ifdef KINE_DEBUG_PSYCHO
      debug("LanguageData::findTag - Invalid TID given");
#endif
      return 0;
   }
   
#ifdef KINE_DEBUG_PSYCHO
   std::ostringstream out;
   out << "LanguageData::findTag - Returning data @ " << 
     (void *)tagData[tagID - 1];
   debug(out.str());
#endif
   
   // Return whatever is found at the given TID
   return tagData[tagID - 1];
}
