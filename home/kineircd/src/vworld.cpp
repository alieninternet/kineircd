/* vworld.cc
 * Virtual World routines
 */

#include "config.h"

#include "utils.h"


/* generateVWorld - Generate a virtual world hash on the given hostname
 * Original , Simon Butcher <pickle@austnet.org>
 */
String generateVWorld(String const &hostname)
{
   return String("vw1234.") + hostname;
}
