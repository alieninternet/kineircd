[+ AutoGen5 template cpp +]
/* $Id$
 * Configuration definition tables and class constructor/destructors
 *
 * Copyright (c) 2002,2003 Simon Butcher <pickle@alien.net.au>
 * Copyright (c) 2002,2003 KineIRCd Development Team
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
#include "libkineircd/language/config.h"
#include "libkineircd/listener/config.h"

using namespace Kine;

[+
   ;;; Create the variable holding the list thingies
   (define defList '())



   ;;; clearDefList - Clear the definition list
   (define (clearDefList)
      (set! defList '()))



   ;;; addDef - Add a definition to the list for analysis later..
   (define (addDef defName)
      (set! defList
         (cons defName defList)))
      
      
      
   ;;; calcDefChars - Determine the minimum number of necessary characters
   (define (calcDefChars defName)
      (let
         ;; The number of characters. To start with, we presume just the one
	 ;; character is needed for the name to remain an individual..
         ((charCount 1))

	 ;; Iterate through the list..
         (for-each
            (lambda (name)
	       ;; There's no point continuing if the number of characters
	       ;; within this name is greater than the number of characters
	       ;; we currently require for a match. There's also no point
	       ;; comparing this string with itself, since it would serve
	       ;; to be useless. 
	       (if (and
	              (>= (string-length name) charCount)
		      (not (= name defName)))
	          (letrec ((nextChar
		              (lambda ()
			         ;; Increase the char count, and examine the
				 ;; next character..
				 (set! charCount (+ charCount 1))
				 (examineNames)))
				 
		           (examineNames
		              (lambda ()
	                         ;; Check that the first 'n+1' relevant chars,
				 ;; known so far, of both names are equal. If
				 ;; they are, then we have not yet found the
				 ;; minimum number of unique characters
				 ;; required for individuality yet...
				 (if (=
				        (substring name 0 charCount)
					(substring defName 0 charCount))
					
				    ;; Increase the char count, and repeat..
				    (nextChar)))))
				 
		     ;; Start examining these two names..
		     (examineNames))))
	    defList)
	 
	 ;; Return the char count we found..
	 charCount))
      
 +][+DEFINE output-class-definition-table+]const AISutil::ConfigParser::defTable_type Config::[+tableClass+] = {[+(clearDefList)+][+FOR definition+][+(addDef (string-upcase (get "name")))+][+ENDFOR+][+IF
   (string-ci=?
      (get "recurse")
      "yes")+][+FOR definition+]
[+IF (exist? ".condition")+]#ifdef [+condition+]
[+ENDIF+]     {
	"[+(string-upcase (get "name"))+]", [+(calcDefChars (string-upcase (get "name")))+],
	  [+IF (exist? ".hasVariable")+](void*)&Config::def[+IF (exist? ".variable")+][+variable+][+ELSE+][+(getPrefix)+][+name+][+ENDIF+][+ELSE+]0[+ENDIF+],
	  [+IF (exist? ".varHandler")+]&[+varHandler+][+ELSE+]0[+ENDIF+],
	  [+IF (or
	          (exist? ".definition")
		  (shadowTable?))+]&defClass[+(getPrefix)+][+name+][+ELSE+]0[+ENDIF+],
	  [+IF (exist? ".classHandler")+]&[+classHandler+][+ELSE+]0[+ENDIF+]
     },[+IF .condition+]
#endif // [+condition+][+ENDIF+][+ENDFOR+][+ENDIF+]
     {
	0, 0,
	  [+IF (exist? "defaultDefinition.variable")+](void*)&Config::def[+defaultDefinition.variable+][+ELSE+]0[+ENDIF+],
	  [+IF (exist? "defaultDefinition.varHandler")+]&[+defaultDefinition.varHandler+][+ELSE+]0[+ENDIF+],
	  0,
	  0
     }
};

[+IF (string-ci=? 
        (get "recurse")
	"yes")+][+FOR definition+][+IF (or
                                          (exist? ".definition")
			                  (shadowTable?))+]
// "[+(getPrefix)+][+name+]" [+IF (shadowTable?)+]shadow [+ENDIF+]table (nested [+
   (length tablePrefixStack)
 +] level[+
   (if (> (length tablePrefixStack) 1)
      "s")
 +] deep)[+IF (shadowTable?)+][+(pushPrefix (get "name"))+]
[+output-class-definition-table
   tableClass=(string-append
	         "defClass"
		 (getPrefix))
   recurse=no
 +][+(popPrefix)+][+ELSE+][+(pushPrefix (get "name"))+]
[+output-class-definition-table
   tableClass=(string-append
	         "defClass"
		 (getPrefix))
   recurse=yes
 +][+(popPrefix)+][+ENDIF+][+ENDIF+][+ENDFOR+][+ENDIF+][+ENDDEF+]
// Top definitions
[+output-class-definition-table
   tableClass=topDefs
   recurse=yes
 +]

// Configuration class constructor (mainly to load defaults, if any)[+
   ; This is a variable to help us determine what iteration we are at
   (define currentIteration 0)
 +]
Config::Config(void)
  : configuring(false)[+DEFINE output-variable-defaults+][+FOR definition+][+IF (exist? ".hasVariable")+][+IF (exist? ".defaultValue")+],
    [+  
   ; Increase the iteration counter
   (set! currentIteration
      (+ currentIteration 1))
 +]def[+IF (exist? ".variable")+][+variable+][+ELSE+][+(getPrefix)+][+name+][+ENDIF+]([+defaultValue+])[+ENDIF+][+ENDIF+][+IF (exist? ".definition")+][+(pushPrefix (get "name"))+][+output-variable-defaults+][+(popPrefix)+][+ENDIF+][+ENDFOR+][+ENDDEF+][+output-variable-defaults+]
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
