/* $Id$
 * 
 * Copyright (c) 2002,2003 Simon Butcher <pickle@alien.net.au>
 * Copyright (c) 2002,2003 KineIRCd Development Team
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

#ifndef _INCLUDE_KINEIRCD_TODO_H_
# define _INCLUDE_KINEIRCD_TODO_H_ 1

# include <map>
# include <aisutil/time.h>

namespace Kine {
   /*! To-do task list - schedule tasks to be done at a later time
    * 
    * When scheduling a task, you must be aware of a few things. Firstly,
    * you're responsible for the creation and destruction of your task. This
    * may means creating a self-destructing task (i.e. using 'delete this').
    * 
    * Tasks are scheduled once, but may be scheduled as many times are you
    * require. If your task is supposed to be recurring, it's up to you to
    * reschedule it as needs be.
    */
   class ToDo {
    public:
      //! A "task". Inherit your task from this.
      class Task {
       protected:
	 //! Constructor. This can only be constructed by children
	 Task(void)
	   {};
	 
       public:
	 //! Destructor
	 virtual ~Task(void)
	   {};
	 
	 //! The function called when your schedule task is called
	 virtual void operator()(void) = 0;
      };
      
    private:
      //! The list of pending tasks (the map is to sort it by its due time)
      typedef std::map < const unsigned long, Task* > tasks_type;
      tasks_type tasks;
      
      //! Our instance
      static ToDo* instance;
      
      //! Constructor
      ToDo(void);
      
    public:
      //! Destructor
      ~ToDo(void)
	{};

      //! Create the single instance of this class
      static void initInstance(void);
      
      //! Get the single instance of this class
      static ToDo& getInstance(void)
	{ return *instance; };
      
      //! Schedule a task to be executed at the given time ('when')
//      bool schedule(Task& task, const AISutil::Time when);
      
      //! Schedule a task to be executed 'seconds' seconds from now
//      bool schedule(Task& task, const unsigned int second);
   }; // class ToDo
   

   //! Return the current instance of the ToDo:: class
   inline static ToDo& todo(void)
     { return ToDo::getInstance(); };
}; // namespace Kine

#endif // _INCLUDE_KINEIRCD_TODO_H_
