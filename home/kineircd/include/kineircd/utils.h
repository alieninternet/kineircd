/* utils.h
 * Utilities
 */

#ifndef __UTILS_H_
# define __UTILS_H_

# include "str.h"
# include "daemon.h"
# include "channel.h"
# include "user.h"

// Macros
# define octStr(x)	baseXStr(x, 8)		// Convert decimal to octal
# define hexStr(x)	baseXStr(x, 16)		// Convert decimal to hex


// utils.cc
extern bool isIRCspecial(char);

extern bool isChannel(String const &);

extern bool okNickname(String const &);
extern bool okChannel(String const &);

extern String baseXStr(unsigned long, 
		       unsigned short);		// Convert to another base

extern unsigned long getRandom(unsigned long);	// Generate a random number

extern bool toBool(String const &, bool);	// Convert a string to boolean

extern String makeOperPassword(String const &, 
			       String const &);	// Generate an IRC Op password

extern StringMask 
  fixToIdentityMask(String const &);		// Fix a mask into an id mask

// sha1.cc
extern String generateSHA1(String const &);	// SHA1 from as string
   

// vworld.cc
extern String generateVWorld(String const &);	// vworld host from a hostname


#endif

