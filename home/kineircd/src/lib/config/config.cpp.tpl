[+ AutoGen5 template cpp +]
/* $Id$
 * Configuration definition tables and class constructor/destructors
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
 *
 *
[+(dne " * ")+]
 */

#ifdef HAVE_CONFIG_H
# include "autoconf.h"
#endif
#include "kineircd/kineircdconf.h"

#include "kineircd/config.h"
#include "libkineircd/listener/config.h"

using namespace Kine;

[+DEFINE output-class-definition-table+]const AISutil::ConfigParser::defTable_type Config::[+tableClass+] = {[+FOR definition+]
[+IF condition+]#ifdef [+condition+]
[+ENDIF+]     {
	"[+(string-upcase (get "name"))+]", (sizeof("[+name+]") - 1),
	  [+IF hasVariable+](void*)&Config::def[+IF variable+][+variable+][+ELSE+][+(getPrefix)+][+name+][+ENDIF+][+ELSE+]0[+ENDIF+],
	  [+IF varHandler+]&[+varHandler+][+ELSE+]0[+ENDIF+],
	  [+IF .definition+]&defClass[+(getPrefix)+][+name+][+ELSE+]0[+ENDIF+],
	  [+IF classHandler+]&[+classHandler+][+ELSE+]0[+ENDIF+]
     },[+IF condition+]
#endif // [+condition+][+ENDIF+][+ENDFOR+]
     {
	0, 0,
	  [+IF defaultDefinition.variable+](void*)&Config::[+defaultDefinition.variable+][+ELSE+]0[+ENDIF+],
	  0,
	  0,
	  0
     }
};

[+FOR definition+][+IF .definition+]
// "[+(getPrefix)+][+name+]" section (nested [+
   (length tablePrefixStack)
 +] level[+
   (if (> (length tablePrefixStack) 1)
      "s")
 +] deep)[+
   ; Push this prefix to the start of our prefix stack
   (set! tablePrefixStack
      (append
         (list (get "name"))
         tablePrefixStack))
 +]
[+output-class-definition-table
   tableClass=(string-append
	         "defClass"
		 (getPrefix))
 +][+
   ; Pop our definition from the end of the list
   (set! tablePrefixStack
      (cdr tablePrefixStack))
 +][+ENDIF+][+ENDFOR+][+ENDDEF+]
// Top definitions
[+output-class-definition-table tableClass=topDefs+]

// Configuration class constructor (mainly to load defaults, if any)[+
   ; This is a variable to help us determine what iteration we are at
   (define currentIteration 0)
 +]
Config::Config(void)
[+DEFINE output-variable-defaults+][+FOR definition+][+IF hasVariable+][+IF defaultValue+][+
   ; Is this is the first iteration?
   (if (= currentIteration 0)
      ; Output the start..
      "  : "

      ; Output a comma, a new line, and the indentation
      ",\n    ")
 +][+  
   ; Increase the iteration counter
   (set! currentIteration
      (+ currentIteration 1))
 +]def[+IF variable+][+variable+][+ELSE+][+(getPrefix)+][+name+][+ENDIF+]([+defaultValue+])[+ENDIF+][+ENDIF+][+IF .definition+][+
   ; Push this prefix to the start of our prefix stack
   (set! tablePrefixStack
      (append
         (list (get "name"))
         tablePrefixStack))
 +][+output-variable-defaults+][+
   ; Pop our definition from the end of the list
   (set! tablePrefixStack
      (cdr tablePrefixStack))
 +][+ENDIF+][+ENDFOR+][+ENDDEF+][+output-variable-defaults+]
{
#ifdef KINE_WITH_SSL
   // Fire up the SSL component
   SSL_load_error_strings();
   
   if (SSL_library_init()) {
      //
      // SEED PRNG HERE IF THE OPENSSL LIBRARY WONT!!! :( :( :(
      // 
      
      /* Fire up the context using SSLv2 and SSLv3 methods.
       * I think this should be configurable, but at this point I don't care!
       * :(
       */
      defSSLContext = SSL_CTX_new(SSLv23_method());
   }
#endif
}
 

// Destructor
Config::~Config(void)
{
#ifdef KINE_WITH_SSL
   // Clean up the SSL component
   if (defSSLContext != 0) {
      SSL_CTX_free(defSSLContext);
   }
   
   EVP_cleanup();
#endif
}
