/* utils.h
 * Utilities
 */

#ifndef __UTILS_H_
# define __UTILS_H_

# include <stdlib.h>
# include <netinet/in.h>

# include "str.h"

// Macros
# define octStr(x)	baseXStr(x, 8)		// Convert decimal to octal
# define hexStr(x)	baseXStr(x, 16)		// Convert decimal to hex


// Utility class
class Utils {
 private:
   Utils(void) {};				// Constructor (cannot be run)
   
 public:
   // utils.cc
   static String baseXStr(unsigned long, 
			  unsigned short);	// Convert to another base
   static bool toBool(String const &, bool);	// Convert a string to boolean
   static StringMask 
     fixToIdentityMask(String const &);		// Fix a mask into an id mask

   // sha1.cc
   static String generateSHA1(String const &);	// SHA1 from as string
};


#endif

