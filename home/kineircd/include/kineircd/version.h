/* version.h
 * Version and Compile information
 */

#ifndef __VERSION_H_
# define __VERSION_H_

class Version {
 private:
   Version(void) {};				// Constructor (cannot be run)
   
 public:
   static char const *build;
   static char const *version;
   static char const *versionChars;
};

#endif
