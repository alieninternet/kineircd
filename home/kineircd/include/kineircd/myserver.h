/* $Id$
 * 
 * Copyright (c) 2003 Simon Butcher <pickle@alien.net.au>
 * Copyright (c) 2003 KineIRCd Development Team
 * (See DEV-TEAM file for details)
 *
 * This file is a part of KineIRCd.
 * 
 * KineIRCd is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * KineIRCd is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with KineIRCd; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _INCLUDE_KINEIRCD_MYSERVER_H_
# define _INCLUDE_KINEIRCD_MYSERVER_H_ 1

# include <kineircd/server.h>
# include <kineircd/config.h>

namespace Kine {
   /*!
    * \brief Our server instance
    * 
    * This is a special instance of a Server, which is the specific server
    * this daemon represents.
    *
    * Unlike an ordinary Server, though, this also provides access to special
    * functions, such as extended information or sending \e wallops messages.
    * 
    * You should reference it through Kine::myServer().
    */
   class MyServer : public Server {
    private:
      //! Our instance
      static MyServer* instance;
      
      //! Constructor
      MyServer(void);
      
    public:
      //! Destructor
      ~MyServer(void)
	{};
      
      // Create the single instance of this class
      static void initInstance(void);
      
      // Return out instance
      static MyServer& getInstance(void)
	{ return *instance; };
      
      
      // Return the server's description
      const std::string& getDescription(void) const
	{ return config().getServerDescription(); };
      
      // Return the server we're "connected to" (technically, that's us)
      Server& getServer(void) const
	{ return const_cast<MyServer&>(*this); };
   }; // class MyServer


   /*!
    * \brief Return the MyServer instance
    *
    * Since MyServer is a singleton, this is how you should reference it in
    * your code.
    *
    * \return The current instance of MyServer
    * \relates Kine::MyServer
    */
   inline static MyServer& myServer(void)
     { return MyServer::getInstance(); };
};

#endif // _INCLUDE_KINEIRCD_MYSERVER_H_
