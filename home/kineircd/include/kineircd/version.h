/* version.h
 * Version and Compile information
 */

#ifndef __VERSION_H_
# define __VERSION_H_

class Version {
 private:
   Version(void) {};				// Constructor (cannot be run)
   
 public:
   static char const *build;			// Build information
   static char const *version;			// Program name and version
   static char const *versionInfo[];		// Information (for INFO)
   static char const *versionChars;		// Version chars (for VERSION)
};

#endif
