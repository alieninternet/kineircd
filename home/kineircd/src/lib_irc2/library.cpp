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

#include "kineircd/irc2/library.h"
#include "libkineircd_irc2/language.h"

namespace {
   // This is the number of time we have been 'referenced'
   static unsigned int references = 0;
};


/* init - Initialise the IRC-2 library
 * Original 16/04/2003 simonb
 */
bool Kine::LibIRC2::init(void)
{
   // Do we really want to initialise?
   if (references == 0) {
      // Register the language map
      if (!Kine::languages().registerMap(Kine::LibIRC2::Language::tagMap)) {
	 return false;
      }
   }
   
   // Increase the reference counter
   ++references;
}


/* deinit - Deinitialise the IRC-2 library
 * Original 16/04/2003 simonb
 */
void Kine::LibIRC2::deinit(void)
{
   // Do we really want to deinitialise?
   if (references == 1) {
      // Deregister the language map
      Kine::languages().deregisterMap(Kine::LibIRC2::Language::tagMap);
   }
   
   // Decrease the reference count
   --references;
}
