dnl $Id$
dnl AutoConf configuration script for KineIRCd
dnl
dnl Copyright (c) 2001,2002 KineIRCd Development Team
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
AC_PREREQ(2.53)
AC_INIT(src/Makefile.in)
AC_CONFIG_AUX_DIR(autoconf)
AC_CONFIG_HEADER(include/kineircd/kineircdconf.h)

dnl Determine our version information.
dnl These should not be touched as they define the version of this software
dnl from an 'official' standpoint - only the developers who should touch 
dnl them .. uhh.. should touch them :)
VERSION_MAJOR=0
VERSION_MINOR=0
VERSION_EXTRA=.prealpha
PACKAGE_NAME=kineircd
PACKAGE_TARNAME=
PACKAGE_VERSION=$VERSION_MAJOR.$VERSION_MINOR$VERSION_EXTRA
PACKAGE_STRING=
PACKAGE_BUGREPORT=kineircd@alien.net.au
LIB_VERSION=0
LIB_REVISION=0
LIB_AGE=0

dnl Export our version information
AC_SUBST(VERSION_MAJOR)
AC_SUBST(VERSION_MINOR)
AC_SUBST(VERSION_EXTRA)
AC_SUBST(PACKAGE_NAME)
AC_SUBST(PACKAGE_TARNAME)
AC_SUBST(PACKAGE_VERSION)
AC_SUBST(PACKAGE_STRING)
AC_SUBST(PACKAGE_BUGREPORT)
AC_SUBST(LIB_VERSION)
AC_SUBST(LIB_REVISION)
AC_SUBST(LIB_AGE)

dnl Save LDFLAGS since we will be making a mess of it soon..
LDFLAGS_SAVE="$LDFLAGS"
INCDIRS=""

dnl I am lazy, these should be configurable but for now they are constants
AC_DEFINE(CONFIG_FILE_ENV_VAR, "KINEIRCD_CONF",
          [Environment variable to check for a location of the config file])
AC_DEFINE(STRICT_CHANNEL_NAMES, 1,
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
  CXXFLAGS="-O2"
  AC_MSG_RESULT(['$CXXFLAGS' (default)])
else
  AC_MSG_RESULT(['$CXXFLAGS' (given)])
fi


dnl -------------------------------------------------------------------------
dnl  Checking for programs we need
dnl -------------------------------------------------------------------------
AC_PROG_CC
AC_PROG_GCC_TRADITIONAL
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_PROG_LN_S
AC_CHECK_PROG(RM, rm, rm -f, del)
AC_CHECK_PROG(LD, ld, ld)
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

AC_CACHE_CHECK([whether signal SIGCHLD is brain-damaged],
 kineircd_cv_sys_sigchld_braindamaged,[
  AC_LANG_C
  AC_TRY_RUN([
	#include <signal.h>

	#ifndef SIGCHLD
	# define SIGCHLD SIGCLD
	#endif
	
	int level = 0;
	
	/* Note - void input because it varies and we don't need it anyway */
	void *handler(void)
	{
	   int status;
	   
	   if (level++ > 2) {
	      exit(1);
	   }
	   
	   signal(SIGCHLD, handler);
	   wait(&status);
	}
	
	int main(void)
	{
	   signal(SIGCHLD, handler);
	   if (fork()) {
	      sleep(10); 
	   } else {
	      sleep(2);
	      exit(1); 
	   }
	   
	   exit(0);
	}
   ],[
     kineircd_cv_sys_sigchld_braindamaged=no
   ],[
     kineircd_cv_sys_sigchld_braindamaged=yes
   ])
 ])
if test "$kineircd_cv_sys_sigchld_braindamaged" = "yes"; then
  AC_DEFINE(SIGCHLD_BRAIN_DAMAGE, 1,
            [Define if the signal SIGCHLD is dodgey])
fi

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
AC_DEFINE_UNQUOTED(SELECT_FDSET_NUMFDS, $kineircd_cv_sys_fdset_numfds,
                   [Number of file descriptors safely usable with select()])

AC_CACHE_CHECK([for tzset() and timezone/daytime variables],
 kineircd_cv_sys_tzset,[
  AC_LANG_CPLUSPLUS
  AC_TRY_RUN([
	#include <time.h>

	int main(void)
	{
	   // Fire up the timezone information
	   tzset();
	   
	   // Try to assign some variables
	   long mytz = timezone;
	   int mydst = daylight;
	   
	   // If that worked, woopee
	   exit(0);
	}
   ],[
     kineircd_cv_sys_tzset=yes
   ],[
     kineircd_cv_sys_tzset=no
   ])
 ])
if test "$kineircd_cv_sys_tzset" = "yes"; then
  AC_DEFINE(HAVE_TZSET, 1,
            [Define if the function tzset() and its variables exist])
fi

AC_CACHE_SAVE


dnl -------------------------------------------------------------------------
dnl  Configure libtool utility
dnl -------------------------------------------------------------------------
AC_DISABLE_FAST_INSTALL
AC_LIBTOOL_DLOPEN
dnl AC_LIBTOOL_WIN32_DLL
AC_PROG_LIBTOOL


dnl -------------------------------------------------------------------------
dnl  Checking structures and types
dnl -------------------------------------------------------------------------
AC_DECL_SYS_SIGLIST
AC_STRUCT_TM
AC_STRUCT_TIMEZONE
AC_TYPE_SIZE_T
AC_TYPE_PID_T
AC_TYPE_MODE_T
AC_TYPE_UID_T
AC_TYPE_SIGNAL
AC_CACHE_SAVE


dnl -------------------------------------------------------------------------
dnl  Checking for header files
dnl -------------------------------------------------------------------------
AC_TIME_WITH_SYS_TIME
AC_HEADER_STDC
AC_HEADER_STAT
AC_HEADER_TIME
AC_HEADER_SYS_WAIT
AC_HEADER_DIRENT
AC_CHECK_HEADERS( \
	arpa/inet.h \
	errno.h \
	fcntl.h \
	limits.h \
	netinet/in.h \
	stdlib.h \
	string.h \
	sys/socket.h \
	sys/time.h \
	sys/timeb.h \
	syslog.h \
	unistd.h \
	pthread.h
		)
AC_CACHE_SAVE


dnl -------------------------------------------------------------------------
dnl  Checking for libraries and specific library functions
dnl -------------------------------------------------------------------------
AC_CHECK_LIB(resolv, res_init)
AC_FUNC_ERROR_AT_LINE
AC_FUNC_FORK
AC_FUNC_FNMATCH
AC_FUNC_VPRINTF
AC_FUNC_GETLOADAVG
AC_FUNC_SELECT_ARGTYPES
AC_FUNC_STRFTIME
AC_CHECK_FUNCS(
	alarm \
	gettimeofday \
	inet_ntoa \
	memset \
	select \
	socket \
	strchr \
	strerror \
		)

AC_MSG_CHECKING([for Net-SNMP library/include path])
AC_ARG_WITH(net-snmp,
                AC_HELP_STRING([--with-net-snmp=DIR],
		               [location of Net-SNMP library/include files]),
 [
   dnl Just run with the given path.. we will check it soon
   snmp_dir="$withval"
 ])
dnl Check that the 'snmp_dir' environment variable is not empty
if test -z "$snmp_dir"; then
  dnl Check for files in numerous different paths :-/
  for dirA in /usr/local /usr/lib /var/lib /usr /var /opt .. . ~; do
    for dirB in $dirA $dirA/net-snmp; do
      if test -f "$dirB/include/net-snmp/net-snmp-config.h"; then
        snmp_dir="$dirB"
        break 2
      fi
    done
  done
fi
dnl Check that we have an snmp_dir variable, and the lib directory exist
if test -n "$snmp_dir" -a -d "$snmp_dir/lib"; then
    dnl Blindly agree on that basis. How stupid is that..
    AC_MSG_RESULT($snmp_dir)
    MOD_SNMP_INCDIRS="-I$snmp_dir/include"
    MOD_SNMP_LIBS="-L$snmp_dir/libs -lnetsnmpagent -lnetsnmpmibs -lsnmp"
    kineircd_test_netsnmp_broken=no
else
  AC_MSG_RESULT(not found)
  kineircd_test_netsnmp_broken=yes
fi
AC_SUBST(MOD_SNMP_INCDIRS)
AC_SUBST(MOD_SNMP_LIBS)

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
      AC_DEFINE(HAVE_OPENSSL, 1,
	        [Define if the OpenSSL library is available])
      AC_DEFINE(WITH_SSL, 1,
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
	#include <list>
	#include <map>
	
	int main(void)
	{
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

AC_CACHE_CHECK([for an SGI compliant hash capability in your STL],
 kineircd_cv_lib_stl_sgi_hash,[
  AC_LANG_CPLUSPLUS
  AC_TRY_RUN([
	#include <hash_map>

	int main(void)
 	{
	   std::hash_map <int, int> hm;
	   hm.clear();
	}
   ],[
     kineircd_cv_lib_stl_sgi_hash=yes
   ],[
     kineircd_cv_lib_stl_sgi_hash=no
   ])
 ])
if test "$kineircd_cv_lib_stl_sgi_hash" = "yes"; then
  AC_DEFINE(STL_HAS_HASH, 1,
            [Define if your C++ STL has the SGI-compatible hashing extensions])
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
  AC_DEFINE(STL_HAS_SLIST, 1,
            [Define if your C++ STL has an SGI-compatible slist<> template])
fi

AC_CACHE_SAVE


dnl -------------------------------------------------------------------------
dnl  Determining compilation configuration parameters
dnl -------------------------------------------------------------------------
dnl
dnl Misc feature settings
dnl
AC_MSG_CHECKING([default-config-file parameter])
AC_ARG_ENABLE(default-config-file,[
  --enable-default-config-file=file          (Default is "ircd.conf")
    This is the default configuration file to load at runtime. It can be
    overridden on the command line. This option cannot be disabled.

 ],[
   if test "$enableval" = "no"; then
     AC_MSG_RESULT([ircd.conf (default, fixed)])
     kineircd_test_default_config_file="ircd.conf"
   else
     AC_MSG_RESULT(yes)
     kineircd_test_default_config_file=$enableval
   fi
 ],[
   AC_MSG_RESULT([ircd.conf (default)])
   kineircd_test_default_config_file="ircd.conf"
 ])
AC_DEFINE_UNQUOTED(DEFAULT_CONFIG_FILE, "$kineircd_test_default_config_file",
                   [The default file to use when loading the config file])

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
AC_SUBST(kineircd_test_old_config_subparser)
dnl if test "$kineircd_test_old_config_subparser" = "yes"; then
dnl   AC_DEFINE(OLD_CONFIG_SUBPARSER, 1,
dnl             [Define if you also want to use old-style configuration files])
dnl fi

AC_MSG_CHECKING([pid-file parameter])
AC_ARG_ENABLE(pid-file,[
  --enable-pid-file[=default]                (Default is disabled)
    When enabled, this makes the IRC daemon write the PID number to a file
    as it forks out. This can aid control scripts to send signals to the
    IRC Daemon's process, eg. SIGHUP to reload the configuration. If a
    parameter is specified when enabling this option, it will be the default
    file name to write the PID number to. This file can be overrided by
    the configuration file.
 ],[
   if test "$enableval" = "no"; then
     AC_MSG_RESULT(no)
   else
     AC_MSG_RESULT($enableval)
     AC_DEFINE_UNQUOTED(PID_FILE, "$enableval",
               [If defined, the daemon will write its PID to this file])
   fi
 ],[
   AC_MSG_RESULT([no (default)])
 ])

dnl this really should check if syslog is AVAILABLE!! .. right? :(
AC_MSG_CHECKING([syslog-logging parameter])
AC_ARG_ENABLE(syslog-logging,[
  --enable-syslog-logging[=identity]         (Default is on, 'ircd')
    When enabled, this enables logging to the syslog device (if it is
    available on your system). If an optional identity is specified, this
    will be used to identify the process in the log file (eg. you may wish
    to use 'kineircd' instead of 'ircd'). When disabled, no logging will
    occur. This software does not output much to the log, so it won't hurt
    if you leave this on.
 ],[
   if test "$enableval" = "no"; then
     AC_MSG_RESULT(no)
   elif test "$enableval" = "yes"; then
     AC_MSG_RESULT([yes, identity: ircd])
     kineircd_test_syslog_ident = "ircd"
   else
     AC_MSG_RESULT([yes, identity: $enableval])
     kineircd_test_syslog_ident = $enableval
   fi
 ],[
   AC_MSG_RESULT([yes, identity: ircd (default)])
   kineircd_test_syslog_ident = "ircd"
 ])
AC_DEFINE_UNQUOTED(SYSLOG_IDENT, "$kineircd_test_syslog_ident",
                   [The identity to use when sending messages to syslog])

AC_MSG_CHECKING([transport-tcp-ipv4 parameter])
AC_ARG_ENABLE(transport-tcp-ipv4,[
  --enable-transport-tcp-ipv4                (Default is enabled)
    Enable TCP/IPv4 connectivity to the server.
 ],[
   if test "$enableval" = "no"; then
     AC_MSG_RESULT(no)
   else
     AC_MSG_RESULT(yes)
     kineircd_test_compile_socket_ipv4_tcp=yes
     kineircd_test_compile_socket_domain_ipv4=yes
     kineircd_test_compile_socket_type_stream=yes
   fi
 ],[
   AC_MSG_RESULT([yes (default)])
   kineircd_test_compile_socket_ipv4_tcp=yes
   kineircd_test_compile_socket_domain_ipv4=yes
   kineircd_test_compile_socket_type_stream=yes
 ])
AC_SUBST(kineircd_test_compile_socket_ipv4_tcp)
AC_SUBST(kineircd_test_compile_socket_domain_ipv4)
if test "$kineircd_test_compile_socket_ipv4_tcp" = "yes"; then
  AC_DEFINE(HAVE_SOCKET_IPV4_TCP, 1,
            [Define if you want TCP/IPv4 transport])
fi

AC_MSG_CHECKING([transport-tcp-ipv6 parameter])
AC_ARG_ENABLE(transport-tcp-ipv6,[
  --enable-transport-tcp-ipv6                (Default is disabled)
    Enable TCP/IPv6 connectivity to the server.
 ],[
   if test "$enableval" = "no"; then
     AC_MSG_RESULT(no)
   else
     AC_MSG_RESULT(yes)
     kineircd_test_compile_socket_ipv6_tcp=yes
     kineircd_test_compile_socket_domain_ipv6=yes
     kineircd_test_compile_socket_type_stream=yes
   fi
 ],[
   AC_MSG_RESULT([no (default)])
 ])
AC_SUBST(kineircd_test_compile_socket_ipv6_tcp)
AC_SUBST(kineircd_test_compile_socket_domain_ipv6)
if test "$kineircd_test_compile_socket_ipv6_tcp" = "yes"; then
  AC_DEFINE(HAVE_SOCKET_IPV6_TCP, 1,
            [Define if you want TCP/IPv6 transport])
fi

AC_MSG_CHECKING([transport-ipx-spx parameter])
AC_ARG_ENABLE(transport-ipx-spx,[
  --enable-transport-ipx-spx                 (Default is disabled)
    Enable IPX/SPX connectivity to the server.
 ],[
   if test "$enableval" = "no"; then
     AC_MSG_RESULT(no)
   else
     AC_MSG_RESULT(yes)
     kineircd_test_compile_socket_ipx_spx=yes
     kineircd_test_compile_socket_domain_ipx=yes
     kineircd_test_compile_socket_type_seqpacket=yes
   fi
 ],[
   AC_MSG_RESULT([no (default)])
 ])
AC_SUBST(kineircd_test_compile_socket_ipx_spx)
AC_SUBST(kineircd_test_compile_socket_domain_ipx)
if test "$kineircd_test_compile_socket_ipx_spx" = "yes"; then
  AC_DEFINE(HAVE_SOCKET_IPX_SPX, 1,
            [Define if you want IPX/SPX transport])
fi

AC_MSG_CHECKING([transport-unix parameter])
AC_ARG_ENABLE(transport-unix,[
  --enable-transport-unix                    (Default is disabled)
    Enable UNIX socket connectivity to the server.
 ],[
   if test "$enableval" = "no"; then
     AC_MSG_RESULT(no)
   else
     AC_MSG_RESULT(yes)
     kineircd_test_compile_socket_unix=yes
     kineircd_test_compile_socket_domain_unix=yes
     kineircd_test_compile_socket_type_stream=yes
   fi
 ],[
   AC_MSG_RESULT([no (default)])
 ])
AC_SUBST(kineircd_test_compile_socket_unix)
AC_SUBST(kineircd_test_compile_socket_domain_unix)
if test "$kineircd_test_compile_socket_unix" = "yes"; then
  AC_DEFINE(HAVE_SOCKET_UNIX, 1,
            [Define if you want UNIX sockets transport])
fi

dnl Now fix up the socket types...
AC_SUBST(kineircd_test_compile_socket_type_stream)
if test "$kineircd_test_compile_socket_type_stream" = "yes"; then
  AC_DEFINE(HAVE_SOCKET_TYPE_STREAM, 1,
            [Define if you want streaming sockets like TCP])
fi
AC_SUBST(kineircd_test_compile_socket_type_seqpacket)
if test "$kineircd_test_compile_socket_type_seqpacket" = "yes"; then
  AC_DEFINE(HAVE_SOCKET_TYPE_SEQPACKET, 1,
            [Define if you want sequential datagram sockets like SPX])
fi

dnl
dnl Which modules shall we compile?
dnl
AC_MSG_CHECKING([which modules to compile])

if test "$kineircd_test_netsnmp_broken" = "no"; then
  AC_ARG_ENABLE(module-mod-snmp,
                AC_HELP_STRING([--disable-mod-snmp],
		               [Enable the SNMP module]),
   [
     if test "$enableval" \!= "yes"; then
       echo -n "mod_snmp "
       kineircd_test_compile_extensions_mod_snmp=yes
     fi
   ],[
     echo -n "mod_snmp "
     kineircd_test_compile_extensions_mod_snmp=yes
   ])
else
  kineircd_test_compile_extensions_mod_snmp=no
fi
AC_SUBST(kineircd_test_compile_extensions_mod_snmp)

dnl finish off the line
echo

dnl
dnl Developer stuff
dnl
AC_MSG_CHECKING([debugging parameter])
AC_ARG_ENABLE(debugging,[
  --enable-debugging[=level]                 (Default is disabled)
    Enable debugging/profile/sanity checking. This should NEVER be turned
    on for a production IRC server, EVER! It is designed for developers.
    If a level is specified, it increases the level of debugging, and
    can either be 'extended' or 'psycho', both giving obvious results.
 ],[
   if test "$enableval" = "no"; then
     AC_MSG_RESULT(no)
     kineircd_test_have_debugging=no
   else
     if test "$enableval" = "psycho"; then
       AC_MSG_RESULT([yes (psycho)])
       AC_DEFINE(DEBUG_PSYCHO,1,[psychotic-debugging?])
       AC_DEFINE(DEBUG_EXTENDED,1,[extended debugging?])
       AC_DEFINE(DEBUG,1,[debugging?])
       AC_DEFINE(DEBUG_ASSERT,1,[do assert() calls for excellent sanity?])
       kineircd_test_debugging=yes
     elif test "$enableval" = "extended"; then
       AC_MSG_RESULT([yes (extended)])
       AC_DEFINE(DEBUG_EXTENDED,1,[extended debugging?])
       AC_DEFINE(DEBUG,1,[debugging?])
       AC_DEFINE(DEBUG_ASSERT,1,[do assert() calls for excellent sanity?])
       kineircd_test_debugging=yes
     else
       AC_MSG_RESULT(yes)
       AC_DEFINE(DEBUG,1,[debugging?])
       AC_DEFINE(DEBUG_ASSERT,1,[do assert() calls for excellent sanity?])
       kineircd_test_debugging=yes
     fi
   fi
 ],[
   AC_MSG_RESULT([no (default)])
   kineircd_test_debugging=no
 ]) 
AC_SUBST(kineircd_test_debugging)


dnl -------------------------------------------------------------------------
dnl  Determining installation information
dnl -------------------------------------------------------------------------
dnl stuff goes here, would like to do it 'apache style' tho :)


dnl -------------------------------------------------------------------------
dnl  Creating files
dnl -------------------------------------------------------------------------
LDFLAGS="$LDFLAGS_SAVE"
AC_SUBST(INCDIRS)
AC_OUTPUT([
	Makefile
	include/Makefile
	include/kineircd/Makefile
	install/Makefile
	make/defs
	make/rules
	src/Makefile
	src/extensions/Makefile
	src/extensions/mod_snmp/Makefile
	src/lib/Makefile
	src/lib/config/Makefile
	src/lib/logger/Makefile
	src/lib/socket/Makefile
	src/protocols/Makefile
	src/protocols/common/Makefile
	src/protocols/common/irc2/Makefile
	src/protocols/server/Makefile
	src/protocols/server/an-p13/Makefile
	src/protocols/server/an1/Makefile
	src/protocols/user/Makefile
	src/protocols/user/irc2/Makefile
	src/protocols/user/irc3/Makefile
	])


echo
echo "Done!"
echo
echo "Type 'gmake' to build the package."
echo
