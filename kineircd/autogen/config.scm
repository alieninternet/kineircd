;;; $Id$
;;;
;;; Copyright (c) 2002,2003 Simon Butcher <pickle@alien.net.au>
;;; Copyright (c) 2002,2003 KineIRCd Development Team
;;; (See DEV-TEAM file for details)
;;;
;;; This file is a part of KineIRCd.
;;;
;;; KineIRCd is free software; you can redistribute it and/or modify
;;; it under the terms of the GNU General Public License as published by
;;; the Free Software Foundation; either version 2 of the License, or
;;; (at your option) any later version.
;;;
;;; KineIRCd is distributed in the hope that it will be useful,
;;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;;; GNU General Public License for more details.
;;;
;;; You should have received a copy of the GNU General Public License
;;; along with KineIRCd; if not, write to the Free Software
;;; Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
;;;



;;; Create the initial (empty) prefix stack list thingy
(define tablePrefixStack (list ""))



;;; getPrefix - Little function to display the prefix properly
(define (getPrefix)
   (letrec ((prefix "")
            (prefixGetter
	       (lambda (stack)
		  ;; Prepend this prefix..
	          (set! prefix
		     (string-append
		        (car stack)
			prefix))

		  ;; More prefixes to go?
		  (if (not (null? (cdr stack)))
		     (prefixGetter (cdr stack))))))

      ;; Bootstrap the prefix-getting-thingy
      (prefixGetter tablePrefixStack)

      ;; Return the result
      prefix))



;;; pushPrefix - Push a string onto the prefix stack
(define (pushPrefix x)
   (set! tablePrefixStack
      (append
	 (list x)
	 tablePrefixStack)))



;;; popPrefix - Pop a string from the prefix stack
(define (popPrefix)
   (set! tablePrefixStack
      (cdr tablePrefixStack)))



;;; topPrefix - Get the top prefix from the prefix stack
(define (topPrefix)
   (car tablePrefixStack))



;;; shadowTable? - Determine if the table in scope is a shadow table
(define (shadowTable?)
   (and
      (not
         (exist? ".definition"))
      (exist? ".defaultDefinition")))
