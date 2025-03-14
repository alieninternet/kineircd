/* $Id$
 * 
 * Copyright (c) 2001,2002,2003 Simon Butcher <pickle@alien.net.au>
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

#ifndef _INCLUDE_KINEIRCD_LISTENERLIST_H_
# define _INCLUDE_KINEIRCD_LISTENERLIST_H_ 1

#include <kineircd/kineircdconf.h>

# ifdef KINE_STL_HAS_SLIST
#  include <slist>
# else
#  include <list>
# endif

# include <kineircd/listener.h>

namespace Kine {
   //! Handles listener configuration, status and (of course) list management
   class ListenerList {
    public:
# ifdef KINE_STL_HAS_SLIST
      typedef std::slist <Listener*> listeners_type;
# else
      typedef std::list <Listener*> listeners_type;
# endif
      
    private:
      //! The list of listeners
      listeners_type listeners;

    public:
      //! Constructor
      ListenerList(void)
	{};
      
      //! Destructor
      ~ListenerList(void);

      //! Start all listeners listening
      void startAll(void);

      //! Return the listen list (should be temporary)
      const listeners_type& getList(void) const
	{ return listeners; };

      // Friends :)
      friend class ListenerConfig;
   };
};

#endif // _INCLUDE_KINEIRCD_LISTENERLIST_H_
   
