/* src/common/vworld.cc
 * Virtual World routines
 */

#include "config.h"

#include "utils.h"


/* generateVWorld - Generate a virtual world hash on the given hostname
 * Original 22/08/01, Simon Butcher <pickle@austnet.org>
 */
String generateVWorld(String *hostname)
{
   return String("vw1234.") + *hostname;
}
