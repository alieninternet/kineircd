/* debug.cpp
 * Debugging support code
 */

#include "autoconf.h"

#include <iostream>

#include "debug.h"
#include "daemon.h"

// Little check to make sure we are being compiled sanely
#ifndef DEBUG
# warning "Compiling without DEBUG defined?!"
#endif

/* debug - Output a line of debugging information (and broadcast)
 * Original 25/09/01 simonb
 */
void debug(String const &line)
{
   // Output it to stderr
   cerr << line << endl;
   
   // Also, broadcast it
   Daemon::serverNotice(ServerNotice::SN_DEBUG, line);
}
