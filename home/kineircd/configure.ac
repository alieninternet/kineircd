dnl $Id$
dnl AutoConf configuration script for KineIRCd
dnl
dnl Copyright (c) 2001,2002,2003 KineIRCd Development Team
dnl (See DEV-TEAM file for details)
dnl
dnl This file is a part of KineIRCd.
dnl 
dnl KineIRCd is free software; you can redistribute it and/or modify
dnl it under the terms of the GNU General Public License as published by
dnl the Free Software Foundation; either version 2 of the License, or
dnl (at your option) any later version.
dnl 
dnl KineIRCd is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl GNU General Public License for more details.
dnl 
dnl You should have received a copy of the GNU General Public License
dnl along with KineIRCd; if not, write to the Free Software
dnl Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


dnl -------------------------------------------------------------------------
dnl  Getting ready to configure...
dnl -------------------------------------------------------------------------
dnl Initialise the thingamebobs
AC_INIT([KineIRCd], [ ], [kineircd@alien.net.au])
AM_INIT_AUTOMAKE([1.7 foreign no-installinfo no-installman nostdinc subdir-objects])
AC_COPYRIGHT([Portions Copyright (c) 2001,2002 KineIRCd Development Team])
AC_PREREQ(2.57)
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_SRCDIR(src/ircd/ircd.cpp)
AM_CONFIG_HEADER(include/autoconf.h)
AC_REVISION([$Revision$])

dnl Determine our version information.
dnl These should not be touched as they define the version of this software
dnl from an 'official' standpoint - only the developers who should touch 
dnl them .. uhh.. should touch them :)
VERSION_MAJOR=0
VERSION_MINOR=0
VERSION_EXTRA=.prealpha

LIBKINEIRCD_VERSION=0
LIBKINEIRCD_REVISION=0
LIBKINEIRCD_AGE=0

LIBKINEIRCD_CLBP_VERSION=0
LIBKINEIRCD_CLBP_REVISION=0
LIBKINEIRCD_CLBP_AGE=0

LIBKINEIRCD_IRC2_VERSION=0
LIBKINEIRCD_IRC2_REVISION=0
LIBKINEIRCD_IRC2_AGE=0

dnl Export our version information
AC_DEFINE_UNQUOTED(KINEIRCD_VERSION_MAJOR, $VERSION_MAJOR,
                   [Major version number])
AC_DEFINE_UNQUOTED(KINEIRCD_VERSION_MINOR, $VERSION_MINOR,
                   [Minor version number])
AC_DEFINE_UNQUOTED(KINEIRCD_VERSION_EXTRA, "$VERSION_EXTRA",
                   [Optional additional version info])
AC_DEFINE(KINEIRCD_VERSION_NAME_FULL, "Kine IRC Daemon",
          [Full program name])
AC_DEFINE_UNQUOTED(KINEIRCD_VERSION_STRING,
                   "$VERSION_MAJOR.$VERSION_MINOR$VERSION_EXTRA",
                   [Compiled version string for convenience sake])
AC_SUBST(LIBKINEIRCD_VERSION)
AC_DEFINE_UNQUOTED(KINEIRCD_LIB_VERSION, $LIBKINEIRCD_VERSION,
                   [Library version number])
AC_SUBST(LIBKINEIRCD_REVISION)
AC_DEFINE_UNQUOTED(KINEIRCD_LIB_REVISION, $LIBKINEIRCD_REVISION,
                   [Library revision number])
AC_SUBST(LIBKINEIRCD_AGE)
AC_DEFINE_UNQUOTED(KINEIRCD_LIB_AGE, $LIBKINEIRCD_AGE,
                   [Library age number])
AC_SUBST(LIBKINEIRCD_CLBP_VERSION)
AC_SUBST(LIBKINEIRCD_CLBP_REVISION)
AC_SUBST(LIBKINEIRCD_CLBP_AGE)
AC_SUBST(LIBKINEIRCD_IRC2_VERSION)
AC_SUBST(LIBKINEIRCD_IRC2_REVISION)
AC_SUBST(LIBKINEIRCD_IRC2_AGE)

dnl Save LDFLAGS since we will be making a mess of it soon..
LDFLAGS_SAVE="$LDFLAGS"
INCDIRS=""

dnl I am lazy, this should be configurable but for now they are 'constants'
AC_DEFINE(KINE_STRICT_CHANNEL_NAMES, 1,
          [
    Define this if you want strict channel names to be allowed. This means
    channel names are not allowed to have any control characters in them, nor
    the character 0xA0, which is a blank character in many standard character
    sets. The result of this is nobody can fake (on appearances) channels
    without the fake being obvious.
	  ])


dnl -------------------------------------------------------------------------
dnl  Checking 'default' definitions
dnl -------------------------------------------------------------------------
AC_MSG_CHECKING([what to set CXXFLAGS to])
if test "$CXXFLAGS" = ""; then
#  CXXFLAGS="-O2"
  CXXFLAGS="-O0 -g"
  AC_MSG_RESULT(['$CXXFLAGS' (default)])
else
  AC_MSG_RESULT(['$CXXFLAGS' (given)])
fi


dnl -------------------------------------------------------------------------
dnl  Checking for programs we need
dnl -------------------------------------------------------------------------
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_CXX
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_PROG_LN_S
AC_CACHE_SAVE


dnl -------------------------------------------------------------------------
dnl  Checking compiler and system characteristics
dnl -------------------------------------------------------------------------
dnl AC_AIX
dnl AC_MINIX
dnl AC_ISC_POSIX
AC_SYS_LONG_FILE_NAMES
AC_SYS_RESTARTABLE_SYSCALLS
AC_EXEEXT
AC_OBJEXT
AC_C_BIGENDIAN
AC_C_CONST
AC_C_INLINE
AC_C_CHAR_UNSIGNED
AC_CHECK_SIZEOF(int)

AC_CHECK_SIZEOF(char, 1)
if test "$ac_cv_sizeof_char" -eq 0; then
  AC_MSG_ERROR([No size of 'char' type found! Check your compiler/libraries!])
fi
  
AC_CHECK_SIZEOF(short, 2)
if test "$ac_cv_sizeof_short" -eq 0; then
  AC_MSG_ERROR([No size of 'short' type found!])
fi
  
AC_CHECK_SIZEOF(long, 4)
if test "$ac_cv_sizeof_long" -eq 0; then
  AC_MSG_ERROR([No size of 'long' found!])
fi

AC_CHECK_SIZEOF(long long, 8)
if test "$ac_cv_sizeof_long_long" -eq 0; then
  AC_MSG_ERROR([No size of 'long long' found - We need a 'long long' here])
fi

AC_CACHE_CHECK([current select() file fd_set bit-size],
 kineircd_cv_sys_fdset_numfds,[
  AC_LANG_C
  AC_TRY_RUN([
	#include <sys/time.h>
	#include <sys/types.h>
	#include <unistd.h>
	#include <stdio.h>
	
	int main(void) {
	   FILE *outfd;
	   fd_set testset;

	   outfd = fopen("conftestval", "w");

	   if (!outfd) {
	      exit(1);
	   }

	   /* Calculate the size of the fdset and output the value we got */
	   fprintf(outfd, "%d\n", (sizeof(testset) * 8));
	   
	   exit(0);
	}
   ],[
     kineircd_cv_sys_fdset_numfds=`cat conftestval`
   ],[
     kineircd_cv_sys_fdset_numfds=256
     AC_MSG_WARN([Making the assumption of 256 bits could be wrong.])
     echo "  It is recommended that you edit autoconf.h manually to correct this"
   ])
 ])
AC_DEFINE_UNQUOTED(KINE_SELECT_FDSET_NUMFDS, $kineircd_cv_sys_fdset_numfds,
                   [Number of file descriptors safely usable with select()])

AC_CACHE_SAVE


dnl -------------------------------------------------------------------------
dnl  Configure libtool utility
dnl -------------------------------------------------------------------------
AC_LIBTOOL_DLOPEN
AC_LIBLTDL_INSTALLABLE
dnl AC_LIBTOOL_WIN32_DLL
AC_PROG_LIBTOOL
AC_SUBST(LTDLINCL)
AC_SUBST(LIBLTDL)
AC_CONFIG_SUBDIRS(libltdl)


dnl -------------------------------------------------------------------------
dnl  Configure libopts library
dnl -------------------------------------------------------------------------
LIBOPTS_CHECK


dnl -------------------------------------------------------------------------
dnl  Checking structures and types
dnl -------------------------------------------------------------------------
AC_STRUCT_TM
AC_STRUCT_TIMEZONE
AC_TYPE_SIZE_T
AC_TYPE_PID_T
AC_TYPE_MODE_T
AC_TYPE_UID_T
AC_TYPE_SIGNAL
AC_CHECK_DECLS( \
	sys_siglist \
		)
AC_CACHE_SAVE


dnl -------------------------------------------------------------------------
dnl  Checking for header files
dnl -------------------------------------------------------------------------
AC_TIME_WITH_SYS_TIME
AC_HEADER_STDC
AC_HEADER_STDBOOL
AC_HEADER_STAT
AC_HEADER_TIME
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS( \
	errno.h \
	fcntl.h \
	iconv.h \
	langinfo.h \
	limits.h \
	netdb.h \
	pthread.h \
	stdlib.h \
	string.h \
	syslog.h \
	termios.h \
	unistd.h \
	utmp.h \
	utmpx.h \
	wchar.h \
	arpa/inet.h \
	netinet/in.h \
	sys/resource.h \
	sys/socket.h \
	sys/sysinfo.h \
	sys/time.h \
	sys/timeb.h \
	sys/utsname.h \
		)
AC_CACHE_SAVE


dnl -------------------------------------------------------------------------
dnl  Checking for libraries and specific library functions
dnl -------------------------------------------------------------------------
dnl AC_CHECK_LIB(aisutil, something????)
AC_CHECK_LIB(opts, optionProcess)
AC_FUNC_ERROR_AT_LINE
AC_FUNC_FORK
dnl AC_FUNC_GETLOADAVG
AC_FUNC_SELECT_ARGTYPES
AC_FUNC_STRFTIME
AM_ICONV
AC_CHECK_FUNCS(
	getrusage \
	getutent \
	getutxent \
	memset \
	select \
	socket \
	strcasecmp \
	strchr \
	strerror \
	strtol \
	sysinfo \
	tcgetattr \
	tzset \
	uname \
		)

GUILE_FLAGS
if test -n "$ac_cv_path_GUILE_CONFIG"; then
  kineircd_cv_libguile_ok=yes
else
  kineircd_cv_libguile_ok=no
fi

AC_MSG_CHECKING([for OpenSSL library/include path])
AC_ARG_WITH(openssl,
            AC_HELP_STRING([--with-openssl=DIR],
                           [location of OpenSSL library/include files]),
 [
   dnl Just run with the given path.. we will check it soon
   ssl_dir="$withval"
   ssl_libs="$withval/lib"
 ])
dnl Check that the 'ssl_dir' environment variable is not empty
if test -z "$ssl_dir"; then
  dnl Check for files in numerous different paths :-/
  for dirA in /usr/local /usr/lib /var/lib /usr /var /opt .. . ~; do
    for dirB in $dirA $dirA/openssl $dirA/ssl; do
      if test -f "$dirB/include/openssl/ssl.h"; then
        ssl_dir="$dirB"
        ssl_libs="$dirB/lib"
        break 2
      fi
    done
  done
fi
dnl Check that the 'ssl_dir' environment variable is not still empty
if test -n "$ssl_dir"; then
  dnl Check that the 'ssl_libs' environment is a directory
  if test -d "$ssl_libs"; then
    AC_MSG_RESULT($ssl_dir)
    INCDIRS="$INCDIRS -I$ssl_dir/include"
    ssl_lib="-lssl -lcrypto"
    ssl_ldflags="-L$ssl_libs"
    old_LDFLAGS="$LDFLAGS"
    LDFLAGS="$LDFLAGS $ssl_ldflags -lcrypto"
dnl    AC_CHECK_LIB(ssl,SSL_accept,[
      AC_DEFINE(KINE_HAVE_OPENSSL, 1,
	        [Define if the OpenSSL library is available])
      AC_DEFINE(KINE_WITH_SSL, 1,
	        [Define if SSL support is requested in the daemon])
      AC_SUBST(ssl_lib)
      AC_SUBST(ssl_ldflags)
dnl    ],[
dnl      AC_MSG_WARN([Your OpenSSL installation seems incomplete!])
dnl      kineircd_test_openssl_broken=yes
dnl     ])
    LDFLAGS="$old_ldflags"
  else
    AC_MSG_RESULT(unknown)
    AC_MSG_WARN([Only found half of an OpenSSL installation!])
    echo "  It appears your OpenSSL installation does not contain a library directory!"
    echo "  Use --with-openssl=DIRECTORY to corret this if you beleive this is wrong."
    echo "  Without this library you will not be able to use secured SSL connections."
    kineircd_test_openssl_broken=yes
  fi
else
  AC_MSG_RESULT(unknown)
  AC_MSG_WARN([Could not find your OpenSSL installation])
  echo "  Use --with-openssl=DIRECTORY to correct this if you beleive this is wrong."
  echo "  Without this library you will not be able to use secured SSL connections."
  kineircd_test_openssl_broken=yes
fi

AC_CACHE_CHECK([for a working C++ Standard Template Library],
 kineircd_cv_lib_stl_ok,[
  AC_LANG_CPLUSPLUS
  AC_TRY_RUN([
	#include <string>
	#include <list>
	#include <map>
	
	int main(void)
	{
	   std::string foo;
	   foo = "Hello";
	
	   std::list <int> l;
	   l.clear();
	   
	   std::map <int, int> m;
	   m.clear();
	}
   ],[
     kineircd_cv_lib_stl_ok=yes
   ],[
     kineircd_cv_lib_stl_ok=no
   ])
 ])
if test "$kineircd_cv_lib_stl_ok" = "no"; then
  AC_MSG_ERROR([Sorry, you need a working Standard Template Library])
fi

AC_CACHE_CHECK([for an SGI compliant slist<> in your STL],
 kineircd_cv_lib_stl_sgi_slist,[
  AC_LANG_CPLUSPLUS
  AC_TRY_RUN([
	#include <slist>

	int main(void)
 	{
	   std::slist <int> sl;
	   sl.clear();
	   
	   exit(0);
	}
   ],[
     kineircd_cv_lib_stl_sgi_slist=yes
   ],[
     kineircd_cv_lib_stl_sgi_slist=no
   ])
 ])
if test "$kineircd_cv_lib_stl_sgi_slist" = "yes"; then
  AC_SUBST(KINEIRCD_DEFINE__KINE_STL_HAS_SLIST,
           ["# define KINE_STL_HAS_SLIST 1"])
else	  
  AC_SUBST(KINEIRCD_DEFINE__KINE_STL_HAS_SLIST,
           ["/* # define KINE_STL_HAS_SLIST */"])
fi

AC_CACHE_SAVE


dnl -------------------------------------------------------------------------
dnl  Determining compilation configuration parameters
dnl -------------------------------------------------------------------------
dnl
dnl Should any developer pick up the old-style ircd.conf configuration
dnl sub-parser code and complete it (please please please!!) this would
dnl need to be uncommented properly, fixed up and committed.
dnl HOWEVER if you are that developer, only uncomment and commit this if
dnl the code works to a decent extend (determine that yourself, but it should
dnl at least parse in the basics required for a server to run in the event
dnl someone may want it to configure their server from completely).
dnl
dnl AC_MSG_CHECKING([old-config-subparser parameter])
dnl AC_ARG_ENABLE(old-config-subparser,[
dnl   --enable-old-config-subparser              (Default is disabled)
dnl     Enable the old-style ircd.conf configuration format to be used via
dnl     an include line ("includeOldConfig") in the main configuration file.
dnl     This may be used for upgrading the server from another or older version
dnl     of an IRC server using the traditional ircd.conf line-by-line file format.
dnl  ],[
dnl    if test "$enableval" = "no"; then
dnl      AC_MSG_RESULT(no)
dnl      kineircd_test_old_config_subparser=no
dnl    else
dnl      AC_MSG_RESULT(yes)
dnl      kineircd_test_old_config_subparser=yes
dnl    fi
dnl  ],[
dnl    AC_MSG_RESULT([no (default)])
   kineircd_test_old_config_subparser=no
dnl  ])
AM_CONDITIONAL(KINEIRCD_COMPILE_LIB_CONFIG_OLDCONFIG,
	       [test x$kineircd_test_old_config_subparser = xyes])
if test "$kineircd_test_old_config_subparser" = "yes"; then
  AC_SUBST(KINEIRCD_DEFINE__KINE_OLD_CONFIG_SUBPARSER,
           ["# define KINE_OLD_CONFIG_SUBPARSER 1"])
else	  
  AC_SUBST(KINEIRCD_DEFINE__KINE_OLD_CONFIG_SUBPARSER,
           ["/* # define KINE_OLD_CONFIG_SUBPARSER */"])
fi

AC_MSG_CHECKING([security-conscious parameter])
AC_ARG_ENABLE(security-conscious,
              AC_HELP_STRING([--disable-security-conscious],
                             [Disable security conscious checking]),
 [
   if test "$enableval" \!= "yes"; then
     AC_MSG_RESULT(yes)
     kineircd_test_security_conscious=yes
   else
     kineircd_test_security_conscious=no
   fi
 ],[
   AC_MSG_RESULT([yes (default)])
   kineircd_test_security_conscious=yes
 ])
if test "$kineircd_test_security_conscious" = "yes"; then
  AC_DEFINE(KINE_SECURITY_CONSCIOUS, 1,
            [Define if you want to have anal security checks upon start-up])
fi


dnl
dnl Developer stuff
dnl
AC_MSG_CHECKING([debugging parameter])
AC_ARG_ENABLE(debugging,
              AC_HELP_STRING([--enable-debugging],
                             [Enable debugging (for developers)]),
 [
   if test "$enableval" = "no"; then
     AC_MSG_RESULT(no)
     kineircd_test_have_debugging=no
   else
     AC_MSG_RESULT(yes)
     AC_DEFINE(KINE_DEBUG,1,[debugging?])
     AC_DEFINE(KINE_DEBUG_ASSERT,1,[do assert() calls for sanity?])
     kineircd_test_debugging=yes
   fi
 ],[
   AC_MSG_RESULT([no (default)])
   kineircd_test_debugging=no
 ])
AM_CONDITIONAL(KINEIRCD_COMPILE_LIB_DEBUG,
	       [test x$kineircd_test_debugging = xyes])

AM_MAINTAINER_MODE


dnl
dnl Which modules shall we compile?
dnl
AC_MSG_CHECKING([which modules to compile])
echo

dnl This macro will help us reduce repeated code..
dnl use: AC_KINE_MODULE_CHECK(mod_name, [the name of the module])
AC_DEFUN([AC_KINE_MODULE_CHECK],[
  echo $ECHO_N "        $1 $ECHO_C"
  AC_ARG_ENABLE($1,
                AC_HELP_STRING([--disable-$1],
                               [Disable $2]),
   [
    if test "$enableval" \!= "no"; then
      AC_MSG_RESULT([ (yes)$ECHO_C])
      AS_TR_SH(kineircd_test_compile_$1)=yes
    else
      AC_MSG_RESULT([(no)])
    fi
   ],[
    AC_MSG_RESULT([(yes)])
    AS_TR_SH(kineircd_test_compile_$1)=yes
   ])
  AM_CONDITIONAL(AS_TR_CPP(KINEIRCD_COMPILE_$1),
	         [test x$AS_TR_SH(kineircd_test_compile_$1) = xyes])
 ])

dnl mod_channellog
AC_KINE_MODULE_CHECK(mod-channellog, [the channel based logging module])

dnl mod_filelog
AC_KINE_MODULE_CHECK(mod-filelog, [the simple file based logging module])

dnl mod_irc2registrar
AC_KINE_MODULE_CHECK(mod-irc2registrar, [the IRC/2 based registrar module])

dnl mod_irc2user
AC_KINE_MODULE_CHECK(mod-irc2user, [the IRC/2 user protocol module])
if test "$kineircd_test_compile_mod_irc2user" = "yes"; then
  echo $ECHO_N "          + Scheme support$ECHO_C"
  
  dnl If guile is not installed, the answer is obviously no
  if test "$kineircd_cv_libguile_ok" = no; then
    AC_MSG_RESULT([ (no, libguile missing or bad)])
    kineircd_test_mod_irc2user_with_scheme="no"
  else
    dnl Check if they wanted it or not
    AC_ARG_ENABLE(irc2user-scheme,
                  AC_HELP_STRING([--enable-irc2user-scheme],
                                 [Enable scheme extentions for mod_irc2user]),
     [
      if test "$enableval" \!= "no"; then
        AC_MSG_RESULT([ (yes)])
        kineircd_test_mod_irc2user_with_scheme="yes"
      else
        AC_MSG_RESULT([ (no)])
        kineircd_test_mod_irc2user_with_scheme="no"
      fi
     ],[
      AC_MSG_RESULT([ (no)])
      kineircd_test_mod_irc2user_with_scheme="no"
     ])
  fi
  
  dnl If the answer was yes, make a #define to let the code know :)
  if test "$kineircd_test_mod_irc2user_with_scheme" = "yes"; then
    AC_DEFINE(KINE_MOD_IRC2USER_SCHEME_ENABLED, 1,
              [Should mod_irc2user, include scheme (guile) scripting support?])
  fi
fi
AM_CONDITIONAL(KINEIRCD_MOD_IRC2USER_WITH_SCHEME,
               [test "x$kineircd_test_mod_irc2user_with_scheme" = "xyes"])

dnl mod_syslog
if test "$ac_cv_header_syslog_h" = "yes"; then
  AC_KINE_MODULE_CHECK(mod-syslog, [the syslog logging module])
else
  AM_CONDITIONAL(KINEIRCD_COMPILE_MOD_SYSLOG, [false])
fi

dnl Best not to change these..
AC_DEFINE(KINE_INTERNAL_CHARSET, "UCS-4",
          [Internal representation of unicode])
AC_DEFINE(KINE_LANGFILE_CHARSET, "UTF-8",
          [Representation of unicode within the language files])


dnl -------------------------------------------------------------------------
dnl  Creating files
dnl -------------------------------------------------------------------------
LDFLAGS="$LDFLAGS_SAVE"
AC_SUBST(INCDIRS)
AC_OUTPUT([
	Makefile
	include/kineircd/kineircdconf.h
	])
