[+ AutoGen5 template conf +]
[+#
   $Id$
   
   Copyright (c) 2002 Simon Butcher <pickle@alien.net.au>
   Copyright (c) 2002 KineIRCd Development Team
   (See DEV-TEAM file for details)
  
   This file is a part of KineIRCd.
  
   KineIRCd is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
  
   KineIRCd is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
  
   You should have received a copy of the GNU General Public License
   along with KineIRCd; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 +]##
## Example configuration file for KineIRCd
## ---------------------------------------
##
## As you can see, this is very 'C' styled, similar to many configuration
## files around these days (bind etc).
##
## Comments can be C style (/* comment */), C++ style (// comment) or
## UNIX style (# comment). If you need to write something which included
## something which may look like a comment such as a channel name, encase it
## within quotes (", ' or `), such as "#channel". Note that the type of quote
## you use must remain consistant for that quote, such as "foo" 'bah', however
## "foo' is invalid.
##
## Values will need to be enclosed within quotes if they include a space, else
## only the first portion or word of a value may be used.
##
## Each class must be enclosed within a pair of parenthesis ('{' and '}').
## Definitions may be of the form "variable = value" or "variable value",
## whichever suits you at the time, or whichever looks neater. Each definition
## MUST be terminated by a semi-colon (';'), however this is optional for
## classes.
##
## This file uses a variety of comment, quote and definition styles on purpose
## to show how they can be used.
##
## You must edit this to suit your server!!!
##[+

   ;;; getIndentLevel - Work out the identation depth (# of chars)
   (define (getIndentLevel)
      (- (* (length tablePrefixStack) 3) 3))



   ;;; lineIndent - Create an indentation appropriate for our nesting level.
   ;;; Note: this should really use modulus to make-string a string of tabs
   ;;;       instead of recursion :(
   (define (lineIndent)
      (letrec ((output "")
               (indenter
	          (lambda (level)
	             ;; Tab or space? (A tab is worth 8 spaces)
	             (if (> level 8)
		        ;; Append the tab
			(set! output
			   (string-append
			   output
			   "\t"))

		        ;; Append the rest of the string
			(set! output
		           (string-append
			      output
		              (make-string level #\ ))))

			;; More?
			(if (> level 8)
			   (indenter (- level 8))))))

         ;; Do we *really* need to run?
	 (if (> (getIndentLevel) 0)
            (indenter (getIndentLevel)))

	 ;; Return the output
         output))



   ;;; formatComment - Format a comment block
   (define (formatComment commentString)
      (letrec ((output "")
               ;; chunkLength = 80 columns - indentation - "## " - 2
               (chunkLength (- 80 (getIndentLevel) 3 2))
	       (cutWhere 0)

	       (findWhereToCut
	          (lambda (tail where)
		     ;; Is this character one we can break at?
		     (if (equal? (string-ref tail (- where 1)) #\ )
		        ;; Yes, return this!
		        where

			;; No, go back one more character
			(findWhereToCut tail (- where 1)))))

	       (split!
	          (lambda (tail)
		     ;; Work out where to cut the string at..
		     (set! cutWhere
		        (findWhereToCut tail chunkLength))

		     ;; Append the bit before the cut
		     (set! output
			(string-append
			   output
			   (substring tail
			      0
			      cutWhere)))

		     ;; Return the but after the cut
		     (substring tail
		        cutWhere
			(string-length tail))))

	       (formatter
	          (lambda (lineTail)
	             ;; Add line's prefix..
		     (set! output
		        (string-append
		           output
			   "\n"
			   (lineIndent)
			   "## "))

		     ;; Is this string going to fit on this line?
		     (if (< (string-length lineTail) chunkLength)
		        ;; Append the rest of the string
		        (set! output
			   (string-append
			      output
			      lineTail))

		     ;; Recurse, breaking the line down some more
		     (formatter (split! lineTail))))))

         ;; Throw the whole line at the formatter
	 (formatter commentString)

	 ;; Return the output
         output))

 +][+DEFINE make-def-name+][+IF .variable+][+variable+][+ELSE+][+(getPrefix)+][+name+][+ENDIF+][+ENDDEF+]
[+DEFINE output-config-stuff+][+FOR .definition+][+IF (not (exist? "hidden"))+][+IF .definition+][+IF (not (first-for?))+]
[+ENDIF+][+IF .comment+][+IF (not (first-for?))+]
[+ENDIF+][+(formatComment (get "comment"))+][+ENDIF+]
[+(lineIndent)+][+make-def-name+] = {[+(pushPrefix (get "name"))+][+output-config-stuff+][+(popPrefix)+]
[+(lineIndent)+]}[+ELSE+][+IF
   (and 
      (exist? "hasVariable")
      (or
         (exist? "exampleValue")
	 (exist? "defaultValue")))
 +][+IF .comment+][+IF (not (first-for?))+]
[+ENDIF+][+(formatComment (get "comment"))+][+ENDIF+]
[+(lineIndent)+][+make-def-name+] = [+IF .exampleValue+][+exampleValue+][+ELSE+][+defaultValue+][+ENDIF+];[+ENDIF+][+ENDIF+][+ENDIF+][+ENDFOR+][+ENDDEF+]
[+output-config-stuff+]
