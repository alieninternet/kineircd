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
#include <aisutil/string/string.h>

#include "kineircd/languages.h"
#include "libkineircd/debug.h"

using namespace Kine;
using AISutil::String;


/* ~LanguageData - Destructor
 * Original 20/03/2003 simonb
 */
Languages::LanguageData::~LanguageData(void)
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


/* mergeWith - Merge the given language data "over" our existing known data
 * Original 20/03/2003 simonb
 */
void Languages::LanguageData::mergeWith(const Languages::LanguageData& newData)
{
   // Firstly, if the given data has more tags than we know of, resize ourself
   if (newData.tagData.size() > tagData.size()) {
#ifdef KINE_DEBUG_PSYCHO
      debug("Languages::LanguageData::mergeWith() - Resizing data set to " +
	    String::convert(newData.tagData.size()));
#endif
      tagData.resize(newData.tagData.size());
   }
   
   // Iterate over both data sets to see what needs replacing/inserting
   for (unsigned int i = 0; i < newData.tagData.size(); i++) {
      // Does data exist at this point
      if (newData.tagData[i] != 0) {
#ifdef KINE_DEBUG_PSYCHO
	 debug("Languages::LanguageData::mergeWith() - TID #" +
	       String::convert(i) + " has new data");
#endif	
 
	 // Okay, it has data. Do we have data at this point?
	 if (tagData[i] != 0) {
#ifdef KINE_DEBUG_PSYCHO
	    debug("Languages::LanguageData::mergeWith() - ^^ Old data being "
		  "removed");
#endif
	    
	    // Delete our data, it is being over-written
	    delete tagData[i];
	 }
	 
	 // Make a new string (ick, I don't like this at all)
	 tagData[i] = new std::string(*newData.tagData[i]);
      }
   }
   
   // Finally, check the revisions, if the new one is "newer", change ours..
   if (newData.fileRevision > fileRevision) {
#ifdef KINE_DEBUG_PSYCHO
      debug("Languages::LanguageData::mergeWith() - Revision being changed "
	    "to " + String::convert(newData.fileRevision));
#endif
      fileRevision = newData.fileRevision;
   }
}

/* findTag - Look for a given TID's data, and return it if possible
 * Original 16/03/2003 simonb
 */
const std::string* const 
  Languages::LanguageData::findTag(const Languages::tagID_type tagID) const
{
   // Make sure the tag is valid..
   if ((tagID == 0) || (tagID > tagData.size())) {
#ifdef KINE_DEBUG_PSYCHO
      debug("Languages::LanguageData::findTag() - Invalid TID given");
#endif
      return 0;
   }
   
#ifdef KINE_DEBUG_PSYCHO
   std::ostringstream out;
   out << "Languages::LanguageData::findTag() - Returning data @ " << 
     (void *)tagData[tagID - 1];
   debug(out.str());
#endif
   
   // Return whatever is found at the given TID
   return tagData[tagID - 1];
}
