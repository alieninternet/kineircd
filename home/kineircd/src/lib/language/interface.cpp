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

#include "kineircd/languages.h"
#include "libkineircd/debug.h"

using namespace Kine;
using AISutil::String;


// Our instance...
Languages* Languages::instance = 0;


// Two replacement characters..
const char* const Languages::replacementObjectGlyph = 
  "\357\227\274"; // <=- Unicode U0FFFC; UTF-8 0xEF 0xBF 0xBC
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
   // Return 'unknown' - we have no idea what to put here..
   return replacementObjectGlyph;
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
