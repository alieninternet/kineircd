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

extern bool isChannel(String *);

extern bool okNickname(String *);
extern bool okChannel(String *);

extern String baseXStr(unsigned long, 
		       unsigned short);		// Convert to another base

extern unsigned long getRandom(unsigned long);	// Generate a random number

extern bool toBool(String *, bool);		// Convert a string to boolean

extern String makeOperPassword(String *, 
			       String *);	// Generate an IRC Op password

extern StringMask fixToIdentityMask(String *);	// Fix a mask into an id mask

// sha1.cc
extern String generateSHA1(String *);		// SHA1 from as string
   

// vworld.cc
extern String generateVWorld(String *);		// vworld host from a hostname


#endif

