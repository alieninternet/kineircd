/* debug.h
 * Debugging... stuff...
 */

#ifdef DEBUG
# ifndef __DEBUG_H_
#  define __DEBUG_H_

#  ifdef DEBUG_ASSERT
#   include <cassert>
#  endif

#  include "str.h"

// Output/Broadcast debug line
extern void debug(String const &);

# endif
#endif
