[+ AutoGen5 template lang +][+
   ;;; Our tag counting variable
   (define tagCount 0)

   ;;; Make the variable we are looking for, in the form of data_<langcode>
   (define (makeDataVariable code)
      (sprintf "data_%s"
         code))
 +][+ DEFINE generate-file +][+
   ;; Tell the console what we're up to
   (for-each
      display
      (list
         "-=> Generating langtags file for '"
         (get "langName")
         "' ("
         (get "code")
         ")..."))

   ;; Switch over to a new language file, using the language code in its name
   (out-push-new
      (sprintf "%s%s.lang" 
         (base-name)
	 (get "code")))

   ;; Reset the tag count
   (set! tagCount 0)
 +]# $Id$
# Language file for the KineIRCd package [+langName+]
#
# Copyright (c) 2003 Simon Butcher <pickle@alien.net.au>
# Copyright (c) 2003 KineIRCd Development Team
# (See DEV-TEAM file for details)
#
# Please see ../[+(def-file)+] for full copyright information.
#
# This file is a part of KineIRCd.
#
# KineIRCd is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# KineIRCd is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with KineIRCd; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
#
[+(dne "# ")+]
#
.LangCode	= [+code+]
.LangName	= [+langName+][+ IF (exist? "dialect_name") +]
.LangNote	= [+dialect_name+][+ ELIF (exist? "langNote") +]
.LangNote	= [+langNote+][+ ENDIF +][+ IF (exist? "langMaintainer") +]
.Maintainer	= [+langMaintainer+][+ ENDIF +]
.Revision	= [+
   ;; Generate a revision based on the current time (cropped ISO 8601 style)
   (strftime "%Y%m%d%H"
      (gmtime 
         (current-time)))
 +]
[+ FOR tag +][+ IF
   ;; Make sure there is data for this tag
   (or
      (exist? "data")
      (exist? 
         (makeDataVariable
	    (get "code")))
      (and
         (exist? "fallback")
	 (exist?
	    (makeDataVariable
	       (get "fallback")))))
 +][+
   ;; Increase the tag count
   (set! tagCount
      (+ tagCount 1))
 +][+tagPrefix+][+name+] = [+
   ;; Grab the right data
   (if
      (exist? "data")
      (get "data")
      (if
         (exist? "fallback")
	 (if
	    (exist?
	       (makeDataVariable
	          (get "code")))
	    (get
	       (makeDataVariable
	          (get "code")))
	    (get
	       (makeDataVariable
	          (get "fallback"))))
         (get
            (makeDataVariable
	       (get "code")))))
 +]
[+ ENDIF +][+ ENDFOR +][+
   ;; Output the number of tags generated and end the line on the console
   (for-each
      display
      (list
         #\space
	 tagCount
         " tags"
	 #\nl))

   ;; Write to this language file
   (out-pop)
 +][+ ENDDEF +][+ FOR languages +][+ INVOKE
   generate-file
     code=(get "langCode")
 +][+ FOR dialects +][+ INVOKE
   generate-file
     code=(sprintf "%s-%s"
             (get "langCode")
	     (get "dialect"))
     dialect_name=(get "name")
     fallback=(get "langCode")
 +][+ ENDFOR +][+ ENDFOR +]
