/* version.h
 * Version and Compile information
 */

#ifndef __VERSION_H_
# define __VERSION_H_

class Version {
 private:
   // This cannot be constructed.
   Version(void) {};
   
 public:
   static char const *build;			// Build information
   static char const *programName;		// Program name (simple)
   static unsigned short const versionMajor;	// Program version major number
   static unsigned short const versionMinor;	// Program version minor number
   static char const *version;			// Version identity
   static char const *versionFull;		// Full version identity
   static char const *bugsEmail;		// Bug report e-mail address
   static char const *versionInfo[];		// Information (for INFO)
   static char const *versionChars;		// Version chars (for VERSION)
};

#endif
