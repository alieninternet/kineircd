/* $Id$
 * Signal handlers
 * 
 * Copyright (c) 2001,2002 KineIRCd Development Team
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

#ifndef _INCLUDE_KINEIRCD_SIGNALS_H_
# define _INCLUDE_KINEIRCD_SIGNALS_H_ 1

# include "kineircd/kineircdconf.h"

# ifdef STL_HAS_SLIST
#  include <slist>
# else
#  include <list>
# endif

/* Template for your signal handler..
 * WARNING!! If the signal is called when the daemon has not been initialised
 * then the daemon pointer will be null.
 */
# define SIGNAL_HANDLER_FUNC(x)	\
	void x(const int signal, Kine::Signals::mask_type signalMask)

namespace Kine {
   class Signals {
    public:
      // Signal masks (what you want to receive)
      enum mask_type {
	 SIGNAL_USER_1			= 0x01,
	 SIGNAL_USER_2			= 0x02,
	 SIGNAL_VIOLENT_DEATH		= 0x04,
	 SIGNAL_PEACEFUL_DEATH		= 0x08,
	 SIGNAL_REHASH			= 0x10,
	 SIGNAL_ALL			= 0xFF // Use this for 'ALL signals'
      };
      
      // This defines your signal handler (must be static) and what to receive
      struct handler_call_info_type {
	 const SIGNAL_HANDLER_FUNC((*handler));
	 const mask_type mask;
	 
	 // Is the structure OKAY?
	 bool isOkay(void) const
	   { return ((handler != 0) && (mask != 0)); };
      };

      // Our list of signal handlers
# ifdef STL_HAS_SLIST
      typedef std::slist <handler_call_info_type> handlerList_type;
# else
      typedef std::list <handler_call_info_type> handlerList_type;
# endif
      
    private:
      // The list of handlers we run through to find functions to throw
      handlerList_type handlers;

    public:
      // Constructor
      Signals(void);
      
      // Destructor
      ~Signals(void);
   };
};

#endif // _INCLUDE_KINEIRCD_SIGNALS_H_
