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

#include <libguile.h>
#include <aisutil/string/string.h>
#include <kineircd/registry.h>

#include "mod_irc2user/scheme.h"
#include "mod_irc2user/schemesubs.h"

// Make sure we are sane
#ifndef KINE_MOD_IRC2USER_SCHEME_ENABLED
# error "Cannot compile scheme support without libguile!"
#endif

using namespace Kine::mod_irc2user;
using AISutil::String;


/* initScheme - Initialise the scheme stuff
 * Original 10/02/2003 simonb
 */
void Scheme::initScheme(void)
{
   /* Initialise libguile. Guile needs to know where the base of our stack is,
    * but unfortunately we do not have a nice way to do this without being
    * able to initialise ourself within the original main() - ick. Since guile
    * is hell-bent on garbage collection (and the stack is a key to it doing
    * garbage collection), we are stuck with using this 'non-portable' method
    * for initialising guile. While not being portable, we have no other
    * option!
    */
   ::scm_init_guile();
   
   // Advertise that our features are available..
   ::scm_add_feature("kineircd_mod_irc2user");
   
   // Tell guile what scheme functions we have
   for (unsigned int i = 0; Scheme::Subs::subrInfoTable[i].name != 0; ++i) {
      (void)::scm_c_define_gsubr(Scheme::Subs::subrInfoTable[i].name,
				 Scheme::Subs::subrInfoTable[i].requiredParams,
				 Scheme::Subs::subrInfoTable[i].optionalParams,
				 Scheme::Subs::subrInfoTable[i].restParams,
				 Scheme::Subs::subrInfoTable[i].function);
   }
}


#ifdef KINE_MOD_IRC2USER_HAVE_SCM_GETNUMERIC
/* (getNumeric) scheme function
 * Original 10/02/2003 simonb
 */
SCM Scheme::Subs::scm_getNumeric(SCM numeric)
{
   // Is the parameter a string?
   if (SCM_STRINGP(numeric) != 0) {
      // Make sure the name is upper-cased
      String name = SCM_STRING_CHARS(numeric);
      name = name.toUpper();
      
      // Run over the numeric name list to try to find a match
      for (unsigned int i = 0; numericNames[i].name != 0; ++i) {
	 // Check if this is a match or not
	 if (name == numericNames[i].name) {
	    // Return this numeric's number
	    return SCM_MAKINUM(numericNames[i].numeric);
	 }
      }
      
      // We did not find a match
      return SCM_UNDEFINED;
   }
   
   // If the parameter was a number, return it as it was given
   if (SCM_INUMP(numeric)) {
      return numeric;
   }

   // We were not given the appropriate object, return an undefined object
   return SCM_UNDEFINED;
}
#endif // KINE_MOD_IRC2USER_HAVE_SCM_ISCHANNELONLINE_P


#ifdef KINE_MOD_IRC2USER_HAVE_SCM_ISCHANNELONLINE_P
/* (isChannelOnline?) scheme function
 * Original 10/02/2003 simonb
 */
SCM Scheme::Subs::scm_isChannelOnline_p(SCM name)
{
   // Make sure the given parameter is a string
   if (SCM_STRINGP(name) != 0) {
      // Check if the channel is online
      if (Kine::registry().findChannel(SCM_STRING_CHARS(name)) != 0) {
	 // We found the channel, return #t
	 return SCM_BOOL_T;
      }
      
      // We did not find the channel, return #f
      return SCM_BOOL_F;
   }

   // We were not given the appropriate object, return an undefined object
   return SCM_UNDEFINED;
}
#endif // KINE_MOD_IRC2USER_HAVE_SCM_ISCHANNELONLINE_P


#ifdef KINE_MOD_IRC2USER_HAVE_SCM_ISCLIENTONLINE_P
/* (isClientOnline?) scheme function
 * Original 10/02/2003 simonb
 * Note: Since users and services are 'clients', this, for the most part,
 *       could be defined as the following scheme code snippet:
 * 
 *    (define (isClientOnline? name)
 *       (or
 *          (isUserOnline? name)
 *          (isServiceOnline? name)))
 */
SCM Scheme::Subs::scm_isClientOnline_p(SCM name)
{
   // Make sure the given parameter is a string
   if (SCM_STRINGP(name) != 0) {
      // Check if the client is online
      if (Kine::registry().findClient(SCM_STRING_CHARS(name)) != 0) {
	 // We found the client, return #t
	 return SCM_BOOL_T;
      }
      
      // We did not find the client, return #f
      return SCM_BOOL_F;
   }

   // We were not given the appropriate object, return an undefined object
   return SCM_UNDEFINED;
}
#endif // KINE_MOD_IRC2USER_HAVE_SCM_ISCLIENTONLINE_P


#ifdef KINE_MOD_IRC2USER_HAVE_SCM_ISNETWORKONLINE_P
/* (isNetworkOnline?) scheme function
 * Original 10/02/2003 simonb
 */
SCM Scheme::Subs::scm_isNetworkOnline_p(SCM name)
{
   // Make sure the given parameter is a string
   if (SCM_STRINGP(name) != 0) {
      // Check if the network is online
      if (Kine::registry().findNetwork(SCM_STRING_CHARS(name)) != 0) {
	 // We found the network, return #t
	 return SCM_BOOL_T;
      }
      
      // We did not find the network, return #f
      return SCM_BOOL_F;
   }

   // We were not given the appropriate object, return an undefined object
   return SCM_UNDEFINED;
}
#endif // KINE_MOD_IRC2USER_HAVE_SCM_ISNETWORKONLINE_P


#ifdef KINE_MOD_IRC2USER_HAVE_SCM_ISSERVERONLINE_P
/* (isServerOnline?) scheme function
 * Original 10/02/2003 simonb
 */
SCM Scheme::Subs::scm_isServerOnline_p(SCM name)
{
   // Make sure the given parameter is a string
   if (SCM_STRINGP(name) != 0) {
      // Check if the server is online
      if (Kine::registry().findServer(SCM_STRING_CHARS(name)) != 0) {
	 // We found the server, return #t
	 return SCM_BOOL_T;
      }
      
      // We did not find the server, return #f
      return SCM_BOOL_F;
   }

   // We were not given the appropriate object, return an undefined object
   return SCM_UNDEFINED;
}
#endif // KINE_MOD_IRC2USER_HAVE_SCM_ISSERVERONLINE_P


#ifdef KINE_MOD_IRC2USER_HAVE_SCM_ISSERVICEONLINE_P
/* (isServiceOnline?) scheme function
 * Original 10/02/2003 simonb
 */
SCM Scheme::Subs::scm_isServiceOnline_p(SCM name)
{
   // Make sure the given parameter is a string
   if (SCM_STRINGP(name) != 0) {
      // Check if the service is online
      if (Kine::registry().findService(SCM_STRING_CHARS(name)) != 0) {
	 // We found the service, return #t
	 return SCM_BOOL_T;
      }
      
      // We did not find the service, return #f
      return SCM_BOOL_F;
   }

   // We were not given the appropriate object, return an undefined object
   return SCM_UNDEFINED;
}
#endif // KINE_MOD_IRC2USER_HAVE_SCM_ISSERVICEONLINE_P


#ifdef KINE_MOD_IRC2USER_HAVE_SCM_ISUSERONLINE_P
/* (isUserOnline?) scheme function
 * Original 10/02/2003 simonb
 */
SCM Scheme::Subs::scm_isUserOnline_p(SCM name)
{
   // Make sure the given parameter is a string
   if (SCM_STRINGP(name) != 0) {
      // Check if the user is online
      if (Kine::registry().findUser(SCM_STRING_CHARS(name)) != 0) {
	 // We found the user, return #t
	 return SCM_BOOL_T;
      }
      
      // We did not find the user, return #f
      return SCM_BOOL_F;
   }

   // We were not given the appropriate object, return an undefined object
   return SCM_UNDEFINED;
}
#endif // KINE_MOD_IRC2USER_HAVE_SCM_ISUSERONLINE_P
