/* $Id$
 * Languages interface (langtags)
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

#include <cctype>

#include "kineircd/languages.h"
#include "kineircd/config.h"
#include "libkineircd/debug.h"

using namespace Kine;
using AISutil::String;


// Our instance...
Languages* Languages::instance = 0;


// Two replacement characters..
const char* const Languages::replacementObjectGlyph = 
  "\357\277\274"; // <=- Unicode U0FFFC; UTF-8 0xEF 0xBF 0xBC
const char* const Languages::replacementCharacterGlyph =
  "\357\277\275"; // <=- Unicode U0FFFD; UTF-8 0xEF 0xBF 0xBD


/* initInstance - Create the single instance of this class
 * Original 13/03/2003 simonb
 * Note: This is separated from the getInstance() since doing the if for every
 *       reference call is damned wasteful, don't you think? :)
 */
void Languages::initInstance(void)
{
#ifdef KINE_DEBUG_ASSERT
   // We should only ever be called to init the first instance..
   assert(instance == 0);
#else
   // Well, since we are not debugging I suppose we should quietly ignore this
   if (instance != 0) {
      return;
   }
#endif
   
   // Create the new instance, then!
   instance = new Languages();
   
#ifdef KINE_DEBUG_PSYCHO
   std::ostringstream debugOut;
   debugOut << "Languages::initInstance() - Created new instance @ " <<
     (void *)instance;
   debug(debugOut.str());
#endif
}


/* registerMap - Register a tag name to tag ID correlation array
 * Original 19/03/2003 simonb
 */
bool Languages::registerMap(tagMap_type map)
{
#ifdef KINE_DEBUG_PSYCHO
   std::ostringstream debugOut;
   debugOut << "Languages::registerMap() - Attempting to register map @ " <<
     (void *)map;
   debug(debugOut.str());
#endif

#ifdef KINE_DEBUG_ASSERT
   // Make sure the map is not invalid..
   assert((void*)map != 0);
#endif
   
   // Firstly, we need to see if this is already registered..
   if (tagMaps.find((tagMap_type*)&map) != tagMaps.end()) {
#ifdef KINE_DEBUG_PSYCHO
      debug("Languages::registerMap() - Map already exists");
#endif

      // Don't actually add it, but act as if we did
      return true;
   }

   // Okay, good enough, add it!
   (void)tagMaps.insert((tagMap_type*)&map);
   
#ifdef KINE_DEBUG_PSYCHO
   debug("Languages::registerMap() - Map array added to the set (" +
	 String::convert(tagMaps.size()) + " maps in the set)");
#endif
   
   return true;
}


/* deregisterMap - Remove a tag map array from our table (ie. module unloading)
 * Original 19/03/2003 simonb
 */
void Languages::deregisterMap(const tagMap_type map)
{
#ifdef KINE_DEBUG_PSYCHO
   std::ostringstream debugOut;
   debugOut << "Languages::registerMap() - Attempting to deregister map @ " <<
     (void *)map;
   debug(debugOut.str());
#endif
   
   /* Just try to erase it.. (We really don't care if it is not actually 
    * registered)
    */
   (void)tagMaps.erase((tagMap_type*)&map);
}


/* get - Return the given language data, from the given language
 * Original 15/03/2003 simonb
 */
const std::string Languages::get(const std::string& languageCode,
				 const tagID_type tagID,
				 const parameterList_type* const parameters)
{
   // Check if the tag ID is 0, or larger than the highest known TID..
   if ((tagID == 0) /*|| (tagID > something)*/) {
      /* Return 'unknown' - we have no idea what to put here.. This is most
       * likely a missing tag from the language file(s), or a programmer has
       * done something nasty..
       */
      return replacementObjectGlyph;
   }
   
   // Find the tag..
//   const std::string& tagData = something;
   const std::string tagData("This is tag data \0N \0p0 \0p1 \0q9", 31);
   
   /* If the null character (used to mark necessary substitutions) is missing
    * from the tag data, then we do not need to do any processing. Is this
    * wise to run over the string potentially twice here? In doing this, I'm
    * presuming the majority of tags would not require substitutions..
    */
   if (tagData.find('\0') == (std::string::size_type)-1) {
      // Simply return the string, no further work is necessary..
      return tagData;
   }
   
   // Our output string; this is what we will return after processing
   std::string output;
   
   // Run through the tag data string and substitute anything we can
   for (std::string::size_type i = 0; i < tagData.length(); i++) {
      // Is this a substitution?
      if (tagData[i] == '\0') {
	 // Make sure there is more on the line..
	 if ((i + 1) >= tagData.length()) {
	    // Just skip it - eek!
	    break;
	 }
	 
	 // What do we do with this substitution?
	 switch (tagData[++i]) {
	  case 'a': // The name of the server's administrator
	    output += config().getAdministratorName();
	    continue;
	    
	  case 'A': // The contact point of the server's administrator (e-mail)
	    output += config().getAdministratorContact();
	    continue;
	    
	  case 'd': // The description of the server
	    output += config().getOptionsDescription();
	    continue;
	    
	  case 'L': // The location of the server
	    output += config().getAdministratorLocation();
	    continue;
	    
	  case 'n': // The name of the server (its hostname)
	    output += config().getOptionsServerName();
	    continue;
	    
	  case 'N': // The name of the network (if there is one)
	    output += config().getNetworkName();
	    continue;
	    
	  case 'p':
	  case 'P':
	  case 'q':
	  case 'Q': // A parameter substitution, from the given parameters list
	    // Next char..
	    i++;
	    
	    /* The next char must be a numeric digit, and we must have been
	     * given parameters to work with! Otherwise, there's no point
	     * working out WHICH parameter we're supposed to look at, is
	     * there?
	     */
	    if ((parameters != 0) &&
		(i <= tagData.length()) &&
		(isdigit(tagData[i]))) {
	       unsigned char paramNumber;
	       
	       // Work out the base parameter number..
	       switch (tagData[i - 1]) {
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
	       paramNumber += (unsigned char)(tagData[i] - 0x30);
	       
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
	    output += replacementCharacterGlyph;
	    continue;
	      
	  default: // NFI what this is supposed to be, use a replacement char
	    output += replacementCharacterGlyph;
	    continue;
	 }
      }
      
      // Copy this char flat
      output += tagData[i];
   }
   
   // Return the processed output
   return output;
}

  
/* get - Functions for lazy people (for one to five parameters)
 * Original 20/03/2003 simonb
 */
const std::string Languages::get(const std::string& languageCode,
				 const tagID_type tagID,
				 const parameter_type& p0)
{
   parameterList_type parameters;
   parameters.push_back(&p0);
   return get(languageCode, tagID, &parameters);
}

const std::string Languages::get(const std::string& languageCode,
				 const tagID_type tagID,
				 const parameter_type& p0,
				 const parameter_type& p1)
{
   parameterList_type parameters;
   parameters.push_back(&p0);
   parameters.push_back(&p1);
   return get(languageCode, tagID, &parameters);
}

const std::string Languages::get(const std::string& languageCode,
				 const tagID_type tagID,
				 const parameter_type& p0,
				 const parameter_type& p1,
				 const parameter_type& p2)
{
   parameterList_type parameters;
   parameters.push_back(&p0);
   parameters.push_back(&p1);
   parameters.push_back(&p2);
   return get(languageCode, tagID, &parameters);
}

const std::string Languages::get(const std::string& languageCode,
				 const tagID_type tagID,
				 const parameter_type& p0,
				 const parameter_type& p1,
				 const parameter_type& p2,
				 const parameter_type& p3)
{
   parameterList_type parameters;
   parameters.push_back(&p0);
   parameters.push_back(&p1);
   parameters.push_back(&p2);
   parameters.push_back(&p3);
   return get(languageCode, tagID, &parameters);
}

const std::string Languages::get(const std::string& languageCode,
				 const tagID_type tagID,
				 const parameter_type& p0,
				 const parameter_type& p1,
				 const parameter_type& p2,
				 const parameter_type& p3,
				 const parameter_type& p4)
{
   parameterList_type parameters;
   parameters.push_back(&p0);
   parameters.push_back(&p1);
   parameters.push_back(&p2);
   parameters.push_back(&p3);
   parameters.push_back(&p4);
   return get(languageCode, tagID, &parameters);
}
