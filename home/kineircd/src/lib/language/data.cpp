/* $Id$
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
#include "kineircd/config.h"
#include "kineircd/myserver.h"
#include "kineircd/mynetwork.h"
#include "lib/debug.h"
#include "lib/language/subst.h"

using namespace Kine;
using AIS::Util::String;


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
#ifdef KINE_DEBUG
      debug("Languages::LanguageData::mergeWith() - Resizing data set to " +
	    String::convert(newData.tagData.size()));
#endif
      tagData.resize(newData.tagData.size());
   }
   
   // Iterate over both data sets to see what needs replacing/inserting
   for (unsigned int i = 0; i < newData.tagData.size(); ++i) {
      // Does data exist at this point
      if (newData.tagData[i] != 0) {
#ifdef KINE_DEBUG
	 debug("Languages::LanguageData::mergeWith() - TID #" +
	       String::convert(i) + " has new data");
#endif	
	 
	 // Mmm - new data, increase the real tag counter then
	 ++tagCount;
	 
	 // Okay, it has data. Do we have data at this point?
	 if (tagData[i] != 0) {
#ifdef KINE_DEBUG
	    debug("Languages::LanguageData::mergeWith() - ^^ Old data being "
		  "removed");
#endif
	    
	    // Delete our data, it is being over-written
	    delete tagData[i];
	 }
	 
	 // Make a new string (ick, I don't like this at all)
	 tagData[i] = new std::wstring(*newData.tagData[i]);
      }
   }
   
   // Finally, check the revisions, if the new one is "newer", change ours..
   if (newData.fileRevision > fileRevision) {
#ifdef KINE_DEBUG
      debug("Languages::LanguageData::mergeWith() - Revision being changed "
	    "to " + String::convert(newData.fileRevision));
#endif
      fileRevision = newData.fileRevision;
   }
}


/* process - Process the given string (deal with substitutions and so forth)
 * Original 16/03/2003 simonb
 */
const std::wstring
  Languages::LanguageData::process(const std::wstring& data,
				   const parameterList_type* const
				   parameters) const
{
   /* If the null character (used to mark necessary substitutions) is missing
    * from the tag data, then we do not need to do any processing. Is this
    * wise to run over the string potentially twice here? In doing this, I'm
    * presuming the majority of tags would not require substitutions..
    */
   if (data.find(Internal::LangTags::parameterMarkerChar) ==
       (std::string::size_type)-1) {
      // Simply return the string, no further work is necessary.. easy :)
      return data;
   }
   
   // Our output string; this is what we will return after processing
   std::wstring output;
   
   // Run through the tag data string and substitute anything we can
   for (std::string::size_type i = 0; i < data.length(); ++i) {
      // Is this a substitution?
      if (data[i] == Internal::LangTags::parameterMarkerChar) {
	 // Make sure there is more on the line..
	 if ((i + 1) >= data.length()) {
	    // Just skip it - eek!
	    break;
	 }
	 
	 // What do we do with this substitution?
	 switch (data[++i]) {
#ifdef KINE_DEBUG
# warning "Substitutions from the configuration are not internationalised"
#endif
	  case 'a': // The name of the server's administrator
	    output += toWideStr(config().getAdministratorName());
	    continue;
	    
	  case 'A': // The contact point of the server's administrator (e-mail)
	    output += toWideStr(config().getAdministratorContact());
	    continue;
	    
	  case 'd': // The description of the server
	    output += myServer().getDescription();
	    continue;
	    
	  case 'L': // The location of the server
	    output += toWideStr(config().getAdministratorLocation());
	    continue;
	    
	  case 'n': // The name of the server (its hostname)
	    output += myServer().getName();
	    continue;
	    
	  case 'N': // The name of the network (if there is one)
	    if (myNetwork().isNamed()) {
	       output += myNetwork().getName();
	    } else {
	       // No network name - ewps.. replacement char time!
	       output += Internal::LangTags::replacementObjectGlyph;
	    }
	    continue;
	    
	  case 'p':
	  case 'P':
	  case 'q':
	  case 'Q': // A parameter substitution, from the given parameters list
	    // Next char..
	    ++i;
	    
	    /* The next char must be a numeric digit, and we must have been
	     * given parameters to work with! Otherwise, there's no point
	     * working out WHICH parameter we're supposed to look at, is
	     * there?
	     */
	    if ((parameters != 0) &&
		(i <= data.length()) &&
		(isdigit(data[i]))) {
	       unsigned char paramNumber;
	       
	       // Work out the base parameter number..
	       switch (data[i - 1]) {
		case 'p':
		  paramNumber = 0;
		  break;
		case 'P':
		  paramNumber = 10;
		  break;
		case 'q':
		  paramNumber = 20;
		  break;
		case 'Q':
		  paramNumber = 30;
	       }
	       
	       // Add on the supplementary parameter number
	       paramNumber += (unsigned char)(data[i] - 0x30);
	       
	       // Do we have this parameter in our list??
	       if ((parameterList_type::size_type)(paramNumber + 1) <=
		   parameters->size()) {
		  /* Fantastic, we must have been given this parameter.. Now
		   * all we need to do is append it to out output string
		   * and we're done with it - finally!
		   * 
		   * This look uglier than it really is - honest :(
		   */
		  output += *((*parameters)[paramNumber]);
		  continue;
	       }
	    }
	    
	    // Throw a replacement char in place..
	    output += Internal::LangTags::replacementCharacterGlyph;
	    continue;
	      
	  default: // NFI what this is supposed to be, use a replacement char
	    output += Internal::LangTags::replacementCharacterGlyph;
	    continue;
	 }
      } else if (data[i] == Internal::LangTags::newLineMarkerChar) {
	 // We are not supposed to see this..
	 output += Internal::LangTags::replacementCharacterGlyph;
	 continue;
      }
      
      // Copy this char flat
      output += data[i];
   }
   
   // Return the processed output
   return output;
}
  
  
/* getTag - Look for a given TID's data, and return it if possible
 * Original 16/03/2003 simonb
 */
const std::wstring
  Languages::LanguageData::get(const Languages::tagID_type tagID,
			       const Languages::parameterList_type* const
			       parameters) const
{
#ifdef KINE_DEBUG
   debug("Languages::LanguageData::get() - Requested TID #" +
	 String::convert(tagID) + " from language " + languageCode);
#endif

   // Make sure the tag is valid..
   if ((tagID != 0) && (tagID <= tagData.size())) {
#ifdef KINE_DEBUG
      std::ostringstream out;
      out << "Languages::LanguageData::get() - Data @ " << 
	(void *)tagData[tagID - 1];
      debug(out.str());
#endif
      
      // Remember what is found at this TID..
      const std::wstring* const tag = tagData[tagID - 1];
      
      // Make sure the tag is not null
      if (tag != 0) {
	 // Process the data and return it
	 return process(*tag, parameters);
      }
   }
      
   // Return nothing special
   return std::wstring();
}


/* getTag - Look for a given TID's data, and call the given function as needed
 * Original 16/03/2003 simonb
 */
bool Languages::LanguageData::get(const Languages::tagID_type tagID,
				  callFunction_type& callFunction,
				  const Languages::parameterList_type* const
				  parameters) const
{
#ifdef KINE_DEBUG
   debug("Languages::LanguageData::get() - Requested TID #" +
	 String::convert(tagID) + " from language " + languageCode +
	 " (multi-call)");
#endif

   // Make sure the tag is valid..
   if ((tagID == 0) || (tagID > tagData.size())) {
#ifdef KINE_DEBUG
      debug("Languages::LanguageData::get() - Invalid TID given");
#endif
      return false;
   }
   
#ifdef KINE_DEBUG
   std::ostringstream out;
   out << "Languages::LanguageData::get() - Returning data @ " << 
     (void *)tagData[tagID - 1];
   debug(out.str());
#endif
   
   // Remember what is found at this TID..
   const std::wstring* const tag = tagData[tagID - 1];

   // Our starting and ending positions..
   std::string::size_type startPosition = 0, endPosition;
   
   for (;;) {
      // Find the next segment char..
      endPosition = tag->find(Internal::LangTags::newLineMarkerChar,
			      startPosition);
      
#ifdef KINE_DEBUG
      std::ostringstream out;
      out << "Languages::LanguageData::get() - Call out substring (" <<
	startPosition << " to " << endPosition << " of " << tag->length() <<
	')';
      debug(out.str());
#endif
      
      // Send this string.. (if we get a false, the call out says "no more")
      if (!callFunction(process(tag->substr(startPosition,
					    endPosition - startPosition),
				parameters))) {
	 // The end!
	 return true;
      }

      // Make sure we have something else to send - if not, return..
      if (endPosition >= tag->length()) {
	 return true;
      }
      
      // Remember where to start during the next loop
      startPosition = endPosition + 1;
   }
   
   // Umm, we should never get here.. *shrugs*
   return true;
}
