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
     instance;
   debug(debugOut.str());
#endif
}
