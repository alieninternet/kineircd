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

#ifndef _INCLUDE_KINEIRCD_LANGUAGES_H_
# define _INCLUDE_KINEIRCD_LANGUAGES_H_ 1

# include <kineircd/sender.h>

namespace Kine {
   class Languages {
    private:
      // Our single instance (we exist once, and only once)
      static Languages* instance;
      
      // Constructor
      Languages(void)
	{};
      
    public:
      // Destructor
      ~Languages(void)
	{};
      
      // Create the single instance of this class
      static void initInstance(void);
      
      // Return the single instance of this class (hopefully it exists ;)
      static Languages& getInstance(void)
	{ return *instance; };
   }; // class Languages
   
   
   /* Lazy reference thingy for lazy coders.. like me.. but shh, don't tell
    * anyone I'm lazy and use lazy things like this lazy reference function :(
    */
   inline static Languages& languages(void)
     { return Languages::getInstance(); };
   
   
   // Oh dear, another lazy function for the truely pathetic lazy sods ;)
   inline static Languages& langs(void)
     { return Languages::getInstance(); };
}; // namespace Kine

#endif // _INCLUDE_KINEIRCD_LANGUAGES_H_
