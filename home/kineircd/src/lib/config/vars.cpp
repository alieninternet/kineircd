/* vars.cpp
 * Configuration variable handlers
 */

#include "autoconf.h"

#include "config.h"

// Our constructor
Config::Config(const String &f)
  : parser(&Config::topDefs, f)
{};


// This is temporary.
CONFIG_DEF_HANDLER(Config::defVarNONE)
{
   return true;
}
