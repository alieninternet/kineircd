/* $Id$
 * Handles listener status of the listeners list over-all
 * 
 * Copyright (c) 2002 Simon Butcher <pickle@alien.net.au>
 * Copyright (c) 2002 KineIRCd Development Team
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

#include <aisutil/string/string.h>

#include "kineircd/listenerlist.h"
#include "lib/debug.h"

using namespace Kine;
using AISutil::String;


/* ~ListenerList - Say bye bye to the existing listeners - go on, wave!
 * Original 07/08/2002 simonb
 */
ListenerList::~ListenerList(void)
{
   // Run through the list and delete all the entries
   while (!listeners.empty()) {
      delete listeners.front();
      listeners.pop_front();
   }
}


/* startAll - Start all listeners listening
 * Original 07/08/2002 simonb
 */
void ListenerList::startAll(void)
{
#ifdef KINE_DEBUG_EXTENDED
   debug("ListenerList::startAll() - Starting " + 
	 String::convert(listeners.size()) + " listening sockets");
#endif
   
   // Run through the list of modules and call their start functions
   for (listeners_type::iterator it = listeners.begin(); 
	it != listeners.end(); it++) {
      if (!(*it)->listen()) {
#ifdef KINE_DEBUG
	 debug("ListenerList::startAll() - oops");
#endif
      }
   }
}
