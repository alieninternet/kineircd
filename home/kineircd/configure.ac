dnl ./configure.in
dnl AutoConf configuration script for austhex
dnl Simon Butcher <pickle@austnet.org>
dnl 
dnl Copyright (c) 2001,2002 AustHex Development Team
dnl (See DEV-TEAM file for details)
dnl
dnl This file is a part of AustHex.
dnl 
dnl AustHex is free software; you can redistribute it and/or modify
dnl it under the terms of the GNU General Public License as published by
dnl the Free Software Foundation; either version 2 of the License, or
dnl (at your option) any later version.
dnl 
dnl AustHex is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl GNU General Public License for more details.
dnl 
dnl You should have received a copy of the GNU General Public License
dnl along with AustHex; if not, write to the Free Software
dnl Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


dnl -------------------------------------------------------------------------
dnl  Getting ready to configure...
dnl -------------------------------------------------------------------------
AC_PREREQ(2.52)
AC_INIT(src/Makefile.in)
AC_CONFIG_AUX_DIR(autoconf)
AC_CONFIG_HEADER(include/autoconf.h)

dnl Determine our version information.
dnl These should not be touched as they define the version of this software
dnl from an 'official' standpoint - only the developers who should touch 
dnl them .. uhh.. should touch them :)
VERSION_MAJOR=8
VERSION_MINOR=0
VERSION_EXTRA=.prealpha
PACKAGE_NAME=austhex.servd
PACKAGE_TARNAME=
PACKAGE_VERSION=$VERSION_MAJOR.$VERSION_MINOR$VERSION_EXTRA
PACKAGE_STRING=
PACKAGE_BUGREPORT=dev@austnet.org

dnl Export our version information
AC_SUBST(VERSION_MAJOR)
AC_SUBST(VERSION_MINOR)
AC_SUBST(VERSION_EXTRA)
AC_SUBST(PACKAGE_NAME)
AC_SUBST(PACKAGE_TARNAME)
AC_SUBST(PACKAGE_VERSION)
AC_SUBST(PACKAGE_STRING)
AC_SUBST(PACKAGE_BUGREPORT)

dnl Save LDFLAGS since we will be making a mess of it soon..
LDFLAGS_SAVE="$LDFLAGS"
INCDIRS=""


dnl -------------------------------------------------------------------------
dnl  Checking 'default' definitions
dnl -------------------------------------------------------------------------
AC_MSG_CHECKING([what to set CXXFLAGS to])
if test "$CXXFLAGS" = ""; then
  CXXFLAGS="-Wall -O2 -fomit-frame-pointer -finline-functions"
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
 austhex_cv_sys_sigchld_braindamaged,[
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
     austhex_cv_sys_sigchld_braindamaged=no
   ],[
     austhex_cv_sys_sigchld_braindamaged=yes
   ])
 ])
if test "$austhex_cv_sys_sigchld_braindamaged" = "yes"; then
  AC_DEFINE(SIGCHLD_BRAIN_DAMAGE) 
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

AC_CACHE_CHECK([maximum number of files I can open at once],
 austhex_cv_sys_maxfds,[
  dnl Try a brute-force method
  AC_LANG_C
  AC_TRY_RUN([
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <fcntl.h>
	#include <unistd.h>
	#include <stdio.h>
	#include <string.h>
	
	int main(void) {
	   FILE *outfd;
	   int fd;
	   int count = 6;
	   /* ^^^^ = stdin + stdout + stderr + outfd + configure + config.log */
	   
	   /* open a file for our own purposes, while we can */
	   outfd = fopen("conftestval", "w");
	   
	   if (!outfd) {
	      exit(1);
	   }
	   
	   /* Open as many files as we can */
	   while ((fd = open("conftestval", O_RDONLY)) != -1) {
	      count++;
	   }
	
	   /* Write to the file, finally. */
	   fprintf(outfd, "%d\n", count);
	   
	   exit(0);
	}
   ],[
     austhex_cv_sys_maxfds=`cat conftestval`
   ],[
     austhex_cv_sys_maxfds=256
     AC_MSG_WARN([Making the assumption of 256 bits could be wrong.])
     echo "  It is recommended that you edit autoconf.h manually to correct this"
   ])
 ])
AC_DEFINE_UNQUOTED(MAX_FD_PER_PROCESS, $austhex_cv_sys_maxfds)

AC_CACHE_CHECK([default select() file fd_set bit-size],
 austhex_cv_sys_fdset_numfds,[
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
	   fprintf(outfd, "%d\n", 
	   	   (sizeof(testset) * 8));
	   
	   exit(0);
	}
   ],[
     austhex_cv_sys_fdset_numfds=`cat conftestval`
   ],[
     austhex_cv_sys_fdset_numfds=256
     AC_MSG_WARN([Making the assumption of 256 bits could be wrong.])
     echo "  It is recommended that you edit autoconf.h manually to correct this"
   ])
 ])
AC_DEFINE_UNQUOTED(SELECT_FDSET_NUMFDS, $austhex_cv_sys_fdset_numfds)

AC_CACHE_CHECK([for tzset() and timezone/daytime variables],
 austhex_cv_sys_tzset_and_friends,[
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
     austhex_cv_sys_tzset_and_friends=yes
   ],[
     austhex_cv_sys_tzset_and_friends=no
   ])
 ])
if test "$austhex_cv_sys_tzset_and_friends" = "yes"; then
  AC_DEFINE(HAVE_TZSET_AND_FRIENDS) 
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

AC_MSG_CHECKING([snmp-agent parameter])
AC_ARG_ENABLE(snmp-agent,[
  --enable-snmp-agent=[agentx|master]        (Default is disabled)
    SNMP additions to the server allow you to monitor the server remotely
    via the standard SNMP protocols. This will requre you to have the 
    NET-SNMP libraries installed, however. Specifying agentx here will allow
    the server's SNMP component to connect to a master agent (eg. snmpd) which
    will proxy the requests to the server (this is recommended). Specifying
    master here will allow this server to become a master SNMP agent itself.
    For better control, it is preferred that you use the agentx method.
    Anything else specified here will automatically presume 'agentx'.
 ],[
   if test "$enableval" = "no"; then
     AC_MSG_RESULT(no)
     austhex_test_have_snmp_agent=no
   else
     dnl "Master or agentx?"
     if test "$enableval" = "master"; then
       AC_MSG_RESULT([yes (master agent)])
       AC_DEFINE(HAVE_SNMP_AGENT_MASTER)
       AC_DEFINE(HAVE_SNMP_AGENT_MASTER_PORT, 161)
     else
       AC_MSG_RESULT([yes (agentx)])
       AC_DEFINE(HAVE_SNMP_AGENT)
     fi
     
     dnl We want to know if we have a valid snmp library here..
dnl     AC_CHECK_LIB(ucdagent, init_agent,[
       AC_DEFINE(HAVE_SNMP_AGENT)
       austhex_test_have_snmp_agent=yes
       snmp_lib="-lucdagent -lucdmibs -lsnmp -ldl"
       snmp_ldflags="-L/usr/local/lib -L/usr/lib"
       austhex_test_need_openssl=yes
dnl      ],[
dnl       AC_MSG_WARN([Could not find the appropriate SNMP libraries --
dnl                    SNMP disabled])
dnl       austhex_test_have_snmp_agent=no
dnl      ])
   fi
 ],[
   AC_MSG_RESULT([no (default)])
   austhex_test_have_snmp_agent=no
 ]) 
AC_SUBST(austhex_test_have_snmp_agent)
AC_SUBST(snmp_lib)
AC_SUBST(snmp_ldflags)

AC_MSG_CHECKING([for OpenSSL library/include path])
AC_ARG_ENABLE(ssl,[  --enable-ssl=DIRECTORY * * * * location of SSL library/include files],
 [
   dnl Just run with the given path.. we will check it soon
   ssl_dir="$enableval"
   ssl_libs="$enableval/lib"
 ],[
   dnl Say it is required
   austhex_test_need_openssl_required=yes
 ])
dnl Check that we are not being told to skip this...
if test "$austhex_test_need_openssl" = "yes"; then
  dnl We are forced to try and get it..
  austhex_test_need_openssl_required=yes
  dnl Do they not want the support?
  if test "$ssl_dir" = "no"; then
    austhex_test_openssl_not_wanted=yes   
  fi
else
  if test "$ssl_dir" = "no"; then
    dnl They do not want it and we do not need it - ignore it
    AC_MSG_RESULT(ignoring)
    austhex_test_need_openssl=no
  else
    dnl They want it, try and get it
    austhex_test_need_openssl=yes
  fi
fi
if test "$austhex_test_need_openssl" = "yes"; then
  dnl Check that the 'ssl_dir' environment variable is not empty
  if test -n "$ssl_dir"; then
    dnl Check for files in numerous different paths :-/
    for dirA in /usr/local /usr/lib /var/lib /usr /var /opt . ~; do
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
      dnl Check if they do not want SSL support
      if test "$austhex_test_openssl_not_wanted" = "yes"; then 
        AC_MSG_RESULT([$ssl_dir (support turned off though)])
      else
        AC_MSG_RESULT($ssl_dir)
        AC_DEFINE(HAVE_OPENSSL)
        INCDIRS="$INCDIRS -I$ssl_dir/include"
      fi
      ssl_lib="-lssl -lcrypto"
      ssl_ldflags="-L$ssl_libs"
      LDFLAGS="$LDFLAGS $ssl_ldflags"
      AC_CHECK_LIB(ssl,SSL_accept,[
        AC_MSG_CHECKING([for /dev/urandom])
        dnl /dev/urandom must be a 'char' file, eg. definately a device
        if test -c /dev/urandom; then
          AC_MSG_RESULT(yes)
        else
          AC_MSG_RESULT(no)
  	  AC_DEFINE(MUST_INIT_PRNG)
        fi
      ],[
        AC_MSG_RESULT(unknown)
        AC_MSG_WARN([Your OpenSSL installation seems incomplete!])
        austhex_test_openssl_broken=yes
       ])
    else
      AC_MSG_RESULT(unknown)
      AC_MSG_WARN([Only found half of an OpenSSL installation!])
      echo "  It appears your OpenSSL installation does not contain a library directory!"
      echo "  Use --with-ssl=DIRECTORY to corret this if you beleive this is wrong."
      echo "  Without this library you will not be able to use secured SSL connections."
      austhex_test_openssl_broken=yes
    fi
  else
    AC_MSG_RESULT(unknown)
    AC_MSG_WARN([Could not find your OpenSSL installation])
    echo "  Use --with-ssl=DIRECTORY to correct this if you beleive this is wrong."
    echo "  Without this library you will not be able to use secured SSL connections."
    austhex_test_openssl_broken=yes
  fi
fi
dnl Make sure we got it OK if we desparately need it
if test "$austhex_test_need_openssl_required" = "yes"; then
  if test "$austhex_test_openssl_broken" = "yes"; then
    AC_MSG_ERROR([Something you have included requires OpenSSL and I cannot find it.])
  fi
fi
AC_SUBST(ssl_lib)
AC_SUBST(ssl_ldflags)

AC_CACHE_CHECK([for a working C++ Standard Template Library],
 austhex_cv_lib_stl_ok,[
  AC_LANG_CPLUSPLUS
  AC_TRY_RUN([
	#include <list>
	#include <map>
	
	int main(void)
	{
	   list<int> l;
	   l.clear();
	   
	   map<int, int> m;
	   m.clear();
	}
   ],[
     austhex_cv_lib_stl_ok=yes
   ],[
     austhex_cv_lib_stl_ok=no
   ])
 ])
if test "$austhex_cv_lib_stl_ok" = "yes"; then
  AC_DEFINE(HAVE_CXX_STL) 
else
  AC_MSG_ERROR([Sorry, you need a working Standard Template Library])
fi

AC_CACHE_CHECK([for an SGI compliant hash capability in your STL],
 austhex_cv_lib_stl_sgi_hash,[
  AC_LANG_CPLUSPLUS
  AC_TRY_RUN([
	#include <hash_map>

	int main(void)
 	{
	   hash_map<int, int> hm;
	   hm.clear();
	}
   ],[
     austhex_cv_lib_stl_sgi_hash=yes
   ],[
     austhex_cv_lib_stl_sgi_hash=no
   ])
 ])
if test "$austhex_cv_lib_stl_sgi_hash" = "yes"; then
  AC_DEFINE(STL_HAS_HASH)
fi

AC_CACHE_CHECK([for an SGI compliant single list template in your STL],
 austhex_cv_lib_stl_sgi_slist,[
  AC_LANG_CPLUSPLUS
  AC_TRY_RUN([
	#include <slist>

	int main(void)
 	{
	   slist<int> sl;
	   sl.clear();
	   
	   exit(0);
	}
   ],[
     austhex_cv_lib_stl_sgi_slist=yes
   ],[
     austhex_cv_lib_stl_sgi_slist=no
   ])
 ])
if test "$austhex_cv_lib_stl_sgi_slist" = "yes"; then
  AC_DEFINE(STL_HAS_SLIST)
fi

AC_CACHE_SAVE


dnl -------------------------------------------------------------------------
dnl  Determining compilation configuration parameters
dnl -------------------------------------------------------------------------
dnl
dnl Stuff that effects the following arguments
dnl
AC_MSG_CHECKING([lower-cpu parameter])
AC_ARG_ENABLE(lower-cpu,[
  --enable-lower-cpu                         (Default is disabled)
    With this enabled, it will effect the defaults of some of the feature
    settings below in order to reduce cpu usage. The change will not be
    very dramatic, however.
 ],[
   if test "$enableval" = "no"; then
     AC_MSG_RESULT(no)
   else
     AC_MSG_RESULT(yes)
     austhex_test_lower_cpu="yes"
   fi
 ],[
   AC_MSG_RESULT([no (default)])
 ])


AC_MSG_CHECKING([lower-memory parameter])
AC_ARG_ENABLE(lower-memory,[
  --enable-lower-memory                      (Default is disabled)
    With this enabled, like the option above, it will effect the default
    setting of some of the feature settings below, however this option
    will reduce any additional memory usage instead. Similar to the option
    above, this change will not be a dramatic one.
 ],[
   if test "$enableval" = "no"; then
     AC_MSG_RESULT(no)
   else
     AC_MSG_RESULT(yes)
     austhex_test_lower_memory="yes"
   fi
 ],[
   AC_MSG_RESULT([no (default)])
 ])


dnl
dnl Misc feature settings
dnl
AC_MSG_CHECKING([accept-unknown-langs parameter])
AC_ARG_ENABLE(accept-unknown-langs,[
  --enable-accept-unknown-langs              (Default is disabled)
    When enabled, this allows languages that are unknown to this server to
    be set by a user and become visible to other servers, users and services
    on the network. This can be good in the case of other servers/services
    on the network being aware of more languages than are known locally
    (eg. the langages being requested). This can be bad in the respect that
    it leaves the language command open to abuse with users being able to
    select rediculous languages that don't exist (the server does not check
    against existing language codes). This probably isn't worthwhile if
    the linguistic dialogs are remotely updated over the network (which means
    every server on the network would be updated with the same
    specifications).
 ],[
   if test "$enableval" = "no"; then
     AC_MSG_RESULT(no)
   else
     AC_MSG_RESULT(yes)
     AC_DEFINE(ACCEPT_UNKNOWN_LANGS)
   fi
 ],[
   AC_MSG_RESULT([no (default)])
 ])

AC_MSG_CHECKING([command-die parameter])
AC_ARG_ENABLE(command-die,[
  --enable-command-die                       (Default is enabled)
    This enables the 'DIE' IRC Operator command. This command is one which
    many server administrators do not like since disgruntled operators can
    easily wipe out the server. On the other hand, the command is here out
    of pure convenience.
 ],[
   if test "$enableval" = "no"; then
     AC_MSG_RESULT(no)
   else
     AC_MSG_RESULT(yes)
     AC_DEFINE(ALLOW_COMMAND_DIE)
   fi
 ],[
   AC_MSG_RESULT([yes (default)])
   AC_DEFINE(ALLOW_COMMAND_DIE)
 ])

AC_MSG_CHECKING([allow-clients parameter])
AC_ARG_ENABLE(allow-clients,[
  --enable-allow-clients                     (Default is enabled)
    This allows clients (users/bots/scouts - anything that uses a normal
    client protocol) to connect to the server. Disabling this also has the
    same effect as disabling client protocols on the server if services
    are also not allowed to connect. Normally this should be left unless
    the server is designed explicitly as a hubbing server alone and clients
    are not allowed.
 ],[
   if test "$enableval" = "no"; then
     AC_MSG_RESULT(no)
   else
     AC_MSG_RESULT(yes)
     AC_DEFINE(ALLOW_CLIENT_CONNECTIONS)

     dnl This uses user protocols so we need this
     austhex_test_enable_client_protocols="yes"
   fi
 ],[
   AC_MSG_RESULT([yes (default)])
   AC_DEFINE(ALLOW_CLIENT_CONNECTIONS)

   dnl This uses user protocols so we need this
   austhex_test_enable_client_protocols="yes"
 ])

AC_MSG_CHECKING([allow-servers parameter])
AC_ARG_ENABLE(allow-servers,[
  --enable-allow-servers                     (Default is enabled)
    This allows servers to connect to the server. Normally this should be
    enabled, unless in the case of a stand-alone server which will never
    accept connections to a network of IRC servers.
 ],[
   if test "$enableval" = "no"; then
     AC_MSG_RESULT(no)
   else
     AC_MSG_RESULT(yes)
     AC_DEFINE(ALLOW_SERVER_CONNECTIONS)
   fi
 ],[
   AC_MSG_RESULT([yes (default)])
   AC_DEFINE(ALLOW_SERVER_CONNECTIONS)
 ])

AC_MSG_CHECKING([allow-services parameter])
AC_ARG_ENABLE(allow-services,[
  --enable-allow-services                    (Default is disabled)
    This allows services to connect to this server. Services are very
    similar to bots using the client interface, however they MUST have
    authorisation to connect as they have a very different level of access
    (in some cases higher and in others lower levels of access). Normally
    services use the same protocols as the client protocols, so disabling
    this and clients will result in client protocols being disabled. Do
    not confuse this with servers with built in services such as
    ChanOP/ChanServ et cetra, as they are unrelated.
 ],[
   if test "$enableval" = "no"; then
     AC_MSG_RESULT(no)
   else
     AC_MSG_RESULT(yes)
     AC_DEFINE(ALLOW_SERVICE_CONNECTIONS)
     
     dnl This uses user protocols so we need this
     austhex_test_enable_client_protocols="yes"
   fi
 ],[
   AC_MSG_RESULT([no (default)])
 ])

AC_MSG_CHECKING([convert-motd-tabs parameter])
AC_ARG_ENABLE(convert-motd-tabs,[
  --enable-convert-motd-tabs                 (Default is enabled)
    When this is enabled, TAB characters in your MOTD file will be converted
    to a series of spaces upon loading into memory to 'simulate' a TAB. Some
    text editors use tabs to shrink the size of large spaces (jed for example)
    and this can effect some IRC clients (especially GUI clients) which do
    not translate the TAB character properly resulting in a messy looking
    MOTD. Disabling this will only effect the MOTD loading process.
 ],[
   if test "$enableval" = "no"; then
     AC_MSG_RESULT(no)
   else
     AC_MSG_RESULT(yes)
     AC_DEFINE(CONVERT_MOTD_TABS)
   fi
 ],[
   if test "$austhex_test_lower_cpu" = "yes"; then
     AC_MSG_RESULT([no (lower-cpu default)])
   else
     AC_MSG_RESULT([yes (default)])
     AC_DEFINE(CONVERT_MOTD_TABS)
   fi
 ])

AC_MSG_CHECKING([default-config-file parameter])
AC_ARG_ENABLE(default-config-file,[
  --enable-default-config-file=file          (Default is "ircd.conf")
    This is the default configuration file to load at runtime. It can be
    overridden on the command line. This option cannot be disabled.

 ],[
   if test "$enableval" = "no"; then
     AC_MSG_RESULT([ircd.conf (default, fixed)])
     AC_DEFINE(DEFAULT_CONFIG_FILE, "ircd.conf")
   else
     AC_MSG_RESULT(yes)
     AC_DEFINE(DEFAULT_CONFIG_FILE, $enableval)
   fi
 ],[
   AC_MSG_RESULT([ircd.conf (default)])
   AC_DEFINE(DEFAULT_CONFIG_FILE, "ircd.conf")
 ])

AC_MSG_CHECKING([drop-lame-servers parameter])
AC_ARG_ENABLE(drop-lame-servers,[
  --enable-drop-lame-servers                 (Default is disabled)
    This allows your server to drop other servers that are potentially
    'spoofing' data, however this may also be produced by major 'out-of-sync'
    conditions which can temporarily exist on lagging connections. Arguably
    having this on is a good idea, but may become a disasterous nightmare
    on large networks with soggy links, especially networks based on a
    star-topology. Do not set this unless you know what you're doing.
 ],[
   if test "$enableval" = "no"; then
     AC_MSG_RESULT(no)
   else
     AC_MSG_RESULT(yes)
     AC_DEFINE(DROP_BRAINDEAD_SERVERS)
   fi
 ],[
   AC_MSG_RESULT([no (default)])
 ])

AC_MSG_CHECKING([floodlock-accept-msg parameter])
AC_ARG_ENABLE(floodlock-accept-msg,[
  --enable-floodlock-accept-msg=seconds      (Default is '120')
    This enabled the accept-request message flood-lock. When in the
    'caller-id' user mode (+g), as user that is not on your ACCEPT list who
    is trying to send a message to you will cause a message to be sent to you
    notifying their request to talk to you. The period of time between those
    request messages can be limited here (a number in seconds) to avoid
    people being able to flood while you're trying to 'ignore' them.
 ],[
   if test "$enableval" = "no"; then
     AC_MSG_RESULT(off)
   else
     if test "$enableval" -gt 0; then
       AC_MSG_RESULT([$enableval seconds])
       AC_DEFINE_UNQUOTED(FLOODLOCK_ACCEPT_MSG, $enableval)
     else
       AC_MSG_RESULT([120 seconds (default)])
       AC_DEFINE(FLOODLOCK_ACCEPT_MSG, 120)
     fi
   fi
 ],[
   if test "$austhex_test_lower_cpu" = "yes"; then
     AC_MSG_RESULT([off (lower-cpu default)])
   else
     AC_MSG_RESULT([120 seconds (default)])
     AC_DEFINE(FLOODLOCK_ACCEPT_MSG, 120)
   fi
 ])

AC_MSG_CHECKING([floodlock-away-reply parameter])
AC_ARG_ENABLE(floodlock-away-reply,[
  --enable-floodlock-away-reply=seconds      (Default is '60')
    This enabled the away-reply flood-lock. When talking to someone who is
    marked as away, the server sends you a message notifying you of the
    user being away. This makes the server only send a 'user is away' reply
    to a message upon the first message, and then only after this flood-lock
    timeout (in seconds) has expired. This can reduce the amount of traffic 
    leaving the server dramatically, but does use a tiny bit more memory. 
    Disabling this feature follows the traditional functionality -- one away 
    notification reply per message sent.
 ],[
   if test "$enableval" = "no"; then
     AC_MSG_RESULT(off)
   else
     if test "$enableval" -gt 0; then
       AC_MSG_RESULT([$enableval seconds])
       AC_DEFINE_UNQUOTED(FLOODLOCK_AWAY_REPLY, $enableval)
     else
       AC_MSG_RESULT([60 seconds (default)])
       AC_DEFINE(FLOODLOCK_AWAY_REPLY, 60)
     fi
   fi
 ],[
   if test "$austhex_test_lower_cpu" = "yes"; then
     AC_MSG_RESULT([off (lower-cpu default)])
   elif test "$austhex_test_lower_memory" = "yes"; then
     AC_MSG_RESULT([off (lower-memory default)])
   else
     AC_MSG_RESULT([60 seconds (default)])
     AC_DEFINE(FLOODLOCK_AWAY_REPLY, 60)
   fi
 ])

AC_MSG_CHECKING([floodlock-away-set parameter])
AC_ARG_ENABLE(floodlock-away-set,[
  --enable-floodlock-away-set=seconds        (Default is '45')
    This enabled the away-set flood-lock, which reduces the amount of time
    between a user being able to set /AWAY. The user will be able to change
    their away message twice before having to wait the amount of time given
    in this flood-lock (in seconds) to change their away message. The
    flood-lock remains in place if they unset their away message, which they
    can do anytime regardless of the flood-lock state. This mainly effects
    server to server output, as the away message is broadcast over the network
    to maintain network synchronicity. Many scripts these days will change
    their away message every couple of seconds to acheive an 'idle time'
    counter in their away message (which is highly wasteful since WHOIS
    already does this). This uses a slight bit more memory, but the server
    protocol output can be dramatically reduced. Disabling this will allow
    anyone to change their away message whenever they like.
 ],[
   if test "$enableval" = "no"; then
     AC_MSG_RESULT(off)
   else
     if test "$enableval" -gt 0; then
       AC_MSG_RESULT([$enableval seconds])
       AC_DEFINE_UNQUOTED(FLOODLOCK_AWAY_SET, $enableval)
     else
       AC_MSG_RESULT([45 seconds (default)])
       AC_DEFINE(FLOODLOCK_AWAY_SET, 45)
     fi
   fi
 ],[
   if test "$austhex_test_lower_memory" = "yes"; then
     AC_MSG_RESULT([off (lower-memory default)])
   else
     AC_MSG_RESULT([45 seconds (default)])
     AC_DEFINE(FLOODLOCK_AWAY_SET, 45)
   fi
 ])

AC_MSG_CHECKING([floodlock-knock-msg parameter])
AC_ARG_ENABLE(floodlock-knock-msg,[
  --enable-floodlock-knock-msg=seconds       (Default is '300')
    This enabled the channel KNOCK message flood-lock, to avoid flooders
    taking advantage of the KNOCK command and using it to flood a channel
    while they are not allowed in (invite only). When disabled, users are
    able to KNOCK as many times as they like. When enabled, they can only
    send one KNOCK per time given above in seconds, per channel.
 ],[
   if test "$enableval" = "no"; then
     AC_MSG_RESULT(off)
   else
     if test "$enableval" -gt 0; then
       AC_MSG_RESULT([$enableval seconds])
       AC_DEFINE_UNQUOTED(FLOODLOCK_KNOCK_MSG, $enableval)
     else
       AC_MSG_RESULT([300 seconds (default)])
       AC_DEFINE(FLOODLOCK_KNOCK_MSG, 300)
     fi
   fi
 ],[
   if test "$austhex_test_lower_cpu" = "yes"; then
     AC_MSG_RESULT([off (lower-cpu default)])
   else
     AC_MSG_RESULT([300 seconds (default)])
     AC_DEFINE(FLOODLOCK_KNOCK_MSG, 300)
   fi
 ])

AC_MSG_CHECKING([force-motd parameter])
AC_ARG_ENABLE(force-motd,[
  --enable-force-motd                        (Default is disabled)
    This enables the Message of the Day (MOTD) to be 'forcefully' sent to the
    user upon connection. This is very different from toggling the server mode
    which turns the MOTD on/off upon connection. What this this does is
    changes the numeric of the MOTD so that clients which ignore the MOTD by
    hiding it upon connection cannot hide it (due to it seeming like a
    different numeric). This is a dirty kluge, and is likely to be
    circumvented by a lot of scripts. In anycase, it may be useful still.
 ],[
   if test "$enableval" = "no"; then
     AC_MSG_RESULT(no)
   else
     AC_MSG_RESULT(yes)
     AC_DEFINE(FORCE_MOTD)
   fi
 ],[
   AC_MSG_RESULT([no (default)])
 ])

AC_MSG_CHECKING([helpers-see-umodes parameter])
AC_ARG_ENABLE(helpers-see-umodes,[
  --enable-helpers-see-umodes                (Default is enabled)
    This enabled helpers and operators to be able to see user modes via
    the /mode commands (eg. /mode nickname). This command has the potential
    to be abused, but can be very handy in instances where a user needs help
    and does not know how to see what modes they have enabled.
 ],[
   if test "$enableval" = "no"; then
     AC_MSG_RESULT(no)
   else
     AC_MSG_RESULT(yes)
     AC_DEFINE(HELPERS_CAN_SEE_USER_MODES)
   fi
 ],[
   AC_MSG_RESULT([yes (default)])
   AC_DEFINE(HELPERS_CAN_SEE_USER_MODES)
 ])

AC_MSG_CHECKING([max-listen-queue parameter])
AC_ARG_ENABLE(max-listen-queue,[
  --enable-max-listen-queue=sockets          (Default is 25)
    This option defines how many sockets are allowed to be pending connection
    in the TCP/IP stack. This option cannot be disabled. If you do not
    know what this does, it is best to leave it alone.
 ],[
   if test "$enableval" -gt 0 -a "$enableval" -lt 1000; then
     AC_MSG_RESULT([$enableval sockets])
     AC_DEFINE_UNQUOTED(LISTEN_MAXQUEUE, $enableval)
   else
     AC_MSG_RESULT([25 sockets (default, fixed)])
     AC_DEFINE(LISTEN_MAXQUEUE, 25)
   fi
 ],[
   AC_MSG_RESULT([25 sockets (default)])
   AC_DEFINE(LISTEN_MAXQUEUE, 25)
 ])

AC_MSG_CHECKING([oper-wallops parameter])
AC_ARG_ENABLE(oper-wallops,[
  --enable-oper-wallops                      (Default is enabled)
    Following IRC protocol specifications nobody but servers are allowed to
    generate WALLOPS messages (sent to anyone with usermode +w set).
    Fortunately we can bend the rules a little from the older days of IRC
    and allow IRC operators to use WALLOPS to send messages about the network
    to people. The reason for this being disabled in the IRC specifications
    is to avoid the abuse which is so common with this command, so the
    option exists to disable it.
 ],[
   if test "$enableval" = "no"; then
     AC_MSG_RESULT(no)
   else
     AC_MSG_RESULT(yes)
     AC_DEFINE(ALLOW_OPER_WALLOPS)
   fi
 ],[
   AC_MSG_RESULT([yes (default)])
   AC_DEFINE(ALLOW_OPER_WALLOPS)
 ])

AC_MSG_CHECKING([paranoid-opers parameter])
AC_ARG_ENABLE(paranoid-opers,[
  --enable-paranoid-opers                    (Default is enabled)
    This feature sends a notification to operators when they are being
    'checked out' via WHOIS. When a user executes a WHOIS on an operator while
    they are on-line, they will receive notification that someone has looked
    at their WHOIS information. Currently this does not effect any other
    commands but may in the future.
 ],[
   if test "$enableval" = "no"; then
     AC_MSG_RESULT(no)
   else
     AC_MSG_RESULT(yes)
     AC_DEFINE(NOTIFY_PARANOID_OPERS_ON_WHOIS)
   fi
 ],[
   AC_MSG_RESULT([yes (default)])
   AC_DEFINE(NOTIFY_PARANOID_OPERS_ON_WHOIS)
 ])

AC_MSG_CHECKING([passive-registration parameter])
AC_ARG_ENABLE(passive-registration,[
  --enable-passive-registration              (Default is disabled)
    This feature increases registration security by never saying anything
    to the client/server during the connection phase. This means no errors
    will be reported, instead the user will be disconnected. Unfortunately
    this means that nicknames that are currently in use will result in
    people being disconnected. This was implemented for servers who receive
    a lot of registration attackers.
 ],[
   if test "$enableval" = "no"; then
     AC_MSG_RESULT(no)
   else
     AC_MSG_RESULT(yes)
     AC_DEFINE(PASSIVE_REGISTRATION)
   fi
 ],[
   AC_MSG_RESULT([no (default)])
 ])

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
     AC_DEFINE(PID_FILE)
   fi
 ],[
   AC_MSG_RESULT([no (default)])
 ])

AC_MSG_CHECKING([strict-channel-names parameter])
AC_ARG_ENABLE(strict-channel-names,[
  --enable-strict-channel-names              (Default is enabled)
    This enabled stricter channel name checking than usual. This stops
    channels with potentially nasty characters being created, such as any
    control character, and the mysterious 'extra-space' (nbsp) character
    which exists in the standard ISO-8859-1 and derivative character sets.
    This will stop many fake channels being created and has been requested
    by many users.
 ],[
   if test "$enableval" = "no"; then
     AC_MSG_RESULT(no)
   else
     AC_MSG_RESULT(yes)
     AC_DEFINE(STRICT_CHANNEL_NAMES)
   fi
 ],[
   AC_MSG_RESULT([yes (default)])
   AC_DEFINE(STRICT_CHANNEL_NAMES)
 ])

AC_MSG_CHECKING([strict-registration parameter])
AC_ARG_ENABLE(strict-registration,[
  --enable-strict-registration               (Default is enabled)
    This enabled a stricter registration process. Once a connection type
    has been established (user/server/service etc) it cannot be changed.
    The can stop many registration-attacks which exploit the fact that the
    server is a little brain-dead during the registration phase, or attempt 
    to exploit the single-parser issues of other IRC daemons.
 ],[
   if test "$enableval" = "no"; then
     AC_MSG_RESULT(no)
   else
     AC_MSG_RESULT(yes)
     AC_DEFINE(STRICT_REGISTRATIONS)
   fi
 ],[
   AC_MSG_RESULT([yes (default)])
   AC_DEFINE(STRICT_REGISTRATIONS)
 ])

dnl this really should check if syslog is AVAILABLE!! .. right? :(
AC_MSG_CHECKING([syslog-logging parameter])
AC_ARG_ENABLE(syslog-logging,[
  --enable-syslog-logging[=identity]         (Default is on, 'ircd')
    When enabled, this enables logging to the syslog device (if it is
    available on your system). If an optional identity is specified, this
    will be used to identify the process in the log file (eg. you may wish
    to use 'austhex' instead of 'ircd'). When disabled, no logging will
    occur. This software does not output much to the log, so it won't hurt
    if you leave this on.
 ],[
   if test "$enableval" = "no"; then
     AC_MSG_RESULT(no)
   elif test "$enableval" = "yes"; then
     AC_MSG_RESULT([yes, identity: ircd])
     AC_DEFINE(SYSLOG_IDENT, "ircd")
   else
     AC_MSG_RESULT([yes, identity: $enableval])
     AC_DEFINE(SYSLOG_IDENT, $enableval)
   fi
 ],[
   AC_MSG_RESULT([yes, identity: ircd (default)])
   AC_DEFINE(SYSLOG_IDENT, "ircd")
 ])

AC_MSG_CHECKING([time-to-be-idle parameter])
AC_ARG_ENABLE(time-to-be-idle,[
  --enable-time-to-be-idle=seconds           (Default is '30')
    When enabled, idle information will only be considered/shown when the
    idle time of a user is greater than the given time (in seconds). This
    is a small addition to the code and will lower some bandwidth output.
    When disabled, idle times will always be shown (even if a person is only
    idle 0 seconds).
 ],[
   if test "$enableval" = "no"; then
     AC_MSG_RESULT(off)
   else
     if test "$enableval" -gt 0 -a "$enableval" -lt 1000000; then
       AC_MSG_RESULT([$enableval seconds])
       AC_DEFINE_UNQUOTED(TIME_TO_BE_IDLE, $enableval)
     else
       AC_MSG_RESULT([30 seconds (default)])
       AC_DEFINE(TIME_TO_BE_IDLE, 30)
     fi
   fi
 ],[
   if test "$austhex_test_lower_cpu" = "yes"; then
     AC_MSG_RESULT([off (lower-cpu default)])
   else
     AC_MSG_RESULT([30 seconds (default)])
     AC_DEFINE(TIME_TO_BE_IDLE, 30)
   fi
 ])

AC_MSG_CHECKING([traceroute parameter])
AC_ARG_ENABLE(traceroute,[
  --enable-traceroute                        (Default is enabled)
    This enabled the traceroute command for network routing staff to use.
    This command can be used to probe other networks so it is a command
    that has the potential to be abused. Also, on some servers, it may not
    be worth enabled it since it is common to firewall ICMP to the server
    to avoid some attacks. This command can be useful, however, for
    detecting quality of links to other servers in instances where a simple
    ping isn't enough.
 ],[
   if test "$enableval" = "no"; then
     AC_MSG_RESULT(no)
   else
     AC_MSG_RESULT(yes)
     AC_DEFINE(HAVE_CMD_TRACEROUTE)
   fi
 ],[
   AC_MSG_RESULT([yes (default)])
   AC_DEFINE(HAVE_CMD_TRACEROUTE)
 ])

AC_MSG_CHECKING([transport-tcp-ipv4 parameter])
AC_ARG_ENABLE(transport-tcp-ipv4,[
  --enable-transport-tcp-ipv4                (Default is enabled)
    Enable TCP/IPv4 connectivity to the server.
 ],[
   if test "$enableval" = "no"; then
     AC_MSG_RESULT(no)
     austhex_test_have_tcp_ipv4_transport=no
   else
     AC_MSG_RESULT(yes)
     AC_DEFINE(HAVE_TRANSPORT_TCP_IPV4)
     austhex_test_have_tcp_ipv4_transport=yes
   fi
 ],[
   AC_MSG_RESULT([yes (default)])
   AC_DEFINE(HAVE_TRANSPORT_TCP_IPV4)
   austhex_test_have_tcp_ipv4_transport=yes
 ])
AC_SUBST(austhex_test_have_tcp_ipv4_transport)

AC_MSG_CHECKING([transport-tcp-ipv6 parameter])
AC_ARG_ENABLE(transport-tcp-ipv6,[
  --enable-transport-tcp-ipv6                (Default is disabled)
    Enable TCP/IPv6 connectivity to the server. The implementation is
    currently incomplete.
 ],[
   if test "$enableval" = "no"; then
     AC_MSG_RESULT(no)
     austhex_test_have_tcp_ipv6_transport=no
   else
     AC_MSG_RESULT(yes)
     AC_DEFINE(HAVE_TRANSPORT_TCP_IPV6)
     austhex_test_have_tcp_ipv6_transport=yes
   fi
 ],[
   AC_MSG_RESULT([no (default)])
   austhex_test_have_tcp_ipv6_transport=no
 ])
AC_SUBST(austhex_test_have_tcp_ipv6_transport)

AC_MSG_CHECKING([transport-spx-ipx parameter])
AC_ARG_ENABLE(transport-spx-ipx,[
  --enable-transport-spx-ipx                 (Default is disabled)
    Enable SPX/IPX connectivity to the server.
 ],[
   if test "$enableval" = "no"; then
     AC_MSG_RESULT(no)
     austhex_test_have_spx_ipx_transport=no
   else
     AC_MSG_RESULT(yes)
     AC_DEFINE(HAVE_TRANSPORT_SPX_IPX)
     austhex_test_have_spx_ipx_transport=yes
   fi
 ],[
   AC_MSG_RESULT([no (default)])
   austhex_test_have_spx_ipx_transport=no
 ])
AC_SUBST(austhex_test_have_spx_ipx_transport)

AC_MSG_CHECKING([user-conn-ping parameter])
AC_ARG_ENABLE(user-conn-ping,[
  --enable-user-conn-ping                    (Default is enabled)
    This feature will force clients to reply to a PING request with random
    data upon connection to the server before allowing the client to
    proceed and complete registration. This can trip up poorly programmed
    bots which cannot reply to server requests like a PING by not allowing
    them onto the network.
 ],[
   if test "$enableval" = "no"; then
     AC_MSG_RESULT(no)
   else
     AC_MSG_RESULT(yes)
     AC_DEFINE(USER_CONNECTION_PINGPONG)
   fi
 ],[
   AC_MSG_RESULT([yes (default)])
   AC_DEFINE(USER_CONNECTION_PINGPONG)
 ])

AC_MSG_CHECKING([warn-bad-reg-cmds parameter])
AC_ARG_ENABLE(warn-bad-reg-cmds,[
  --enable-warn-bad-reg-cmds                 (Default is disabled)
    This can open up a security issue with the registration phase. With this
    disabled, unknown commands being sent during the registration phase are
    simply ighnored. When this is enabled, however, unknown commands will
    return an error reply (providing passive registrations are not enabled,
    otherwise unknown commands will result in the connection being dropped).
    When enabled, this follows proper registration protocols, but leaves you
    open to attacks that exploit this, as it could be used for registration
    attacks that flood your out-bound pipe.
 ],[
   if test "$enableval" = "no"; then
     AC_MSG_RESULT(no)
   else
     AC_MSG_RESULT(yes)
     AC_DEFINE(WARN_UNKNOWN_COMMANDS_IN_REGISTRATION)
   fi
 ],[
   AC_MSG_RESULT([no (default)])
 ])

dnl
dnl Protocol toggles
dnl
AC_MSG_CHECKING([irc2user-protocol parameter])
if test "$austhex_test_enable_client_protocols" = "yes"; then
  AC_ARG_ENABLE(irc2user-protocol,[
  --enable-irc2user-protocol                 (Default is enabled)
    The IRC-2 protocol is used with every irc client these days. Unless the
    server is being set up for something specific (such as hubbing) it would
    be wise to leave this enabled.
   ],[
     if test "$enableval" = "no"; then
       AC_MSG_RESULT(no)
       austhex_test_have_irc2user_protocol=no
     else
       AC_MSG_RESULT(yes)
       AC_DEFINE(HAVE_IRC2USER_PROTOCOL)
       austhex_test_have_irc2user_protocol=yes
     fi
   ],[
     AC_MSG_RESULT([yes (default)])
     AC_DEFINE(HAVE_IRC2USER_PROTOCOL)
     austhex_test_have_irc2user_protocol=yes
   ])

  dnl If we are using the protocol, we may need some more stuff...
  if test "$austhex_test_have_irc2user_protocol" = "yes"; then
    AC_MSG_CHECKING([irc2user-ircii-kluge parameter])
    AC_ARG_ENABLE(irc2user-ircii-kluge,[
  --enable-irc2user-ircii-kluge              (Default is enabled)
    This enables a kluge (a workaround) to allow the old ircII client and
    other technically broken clients to connect by adding additional data
    to the server's output. This unfortunate anomaly was created during the
    change-over from the IRC-1 protocol to the IRC-2 protocol, and will
    generate a very slight increase in outgoing client data. This kluge
    is arguably not necessary these days as the effected clients are
    practically extinct.
     ],[
       if test "$enableval" = "no"; then
         AC_MSG_RESULT(no)
       else
         AC_MSG_RESULT(yes)
         AC_DEFINE(HAVE_IRC2USER_IRCII_KLUGE)
       fi
     ],[
       AC_MSG_RESULT([yes (default)])
       AC_DEFINE(HAVE_IRC2USER_IRCII_KLUGE)
     ])
     
    AC_MSG_CHECKING([irc2user-match-counting parameter])
    AC_ARG_ENABLE(irc2user-match-counting,[
  --enable-irc2user-match-counting           (Default is enabled)
    This enabled an aesthetics option to many commands that involve a
    variable list of items, such as HELP, NAMES, WHO etc. Instead of a
    simple 'End of X list' (where X is the command name), it will add the
    number of matches if there is more than one match, or a 'no matches'
    message if there were no matches. This uses only a very slight amount of
    CPU usage, but if you're being tight on the CPU usage you may wish to
    disable this as it does not effect any commands' functionality.
     ],[
       if test "$enableval" = "no"; then
         AC_MSG_RESULT(no)
       else
         AC_MSG_RESULT(yes)
         AC_DEFINE(HAVE_IRC2USER_MATCH_COUNTING)
       fi
     ],[
       if test "$austhex_test_lower_cpu" = "yes"; then
         AC_MSG_RESULT([no (lower-cpu default)])
       else
         AC_MSG_RESULT([yes (default)])
         AC_DEFINE(HAVE_IRC2USER_MATCH_COUNTING)
       fi
     ])
  fi
else
  AC_MSG_RESULT([no (clients disallowed)])
  austhex_test_have_irc2user_protocol=no
fi
AC_SUBST(austhex_test_have_irc2user_protocol)

AC_MSG_CHECKING([irc3user-protocol parameter])
if test "$austhex_test_enable_client_protocols" = "yes"; then
  AC_ARG_ENABLE(irc3user-protocol,[
  --enable-irc3user-protocol                 (Default is disabled)
    The IRC-3 protocol is currently highly experimental and is not used
    in any available clients. The protocol is still on the drawing boards
    so this function should be ignored.
   ],[
     if test "$enableval" = "no"; then
       AC_MSG_RESULT(no)
       austhex_test_have_irc3user_protocol=no
     else
       AC_MSG_RESULT(yes)
       AC_DEFINE(HAVE_IRC3USER_PROTOCOL)
       austhex_test_have_irc3user_protocol=yes
     fi
   ],[
     AC_MSG_RESULT([no (default)])
     austhex_test_have_irc3user_protocol=no
   ])
else
  AC_MSG_RESULT([no (clients disallowed)])
  austhex_test_have_irc3user_protocol=no
fi
AC_SUBST(austhex_test_have_irc3user_protocol)
 
AC_MSG_CHECKING([p13server-protocol parameter])
AC_ARG_ENABLE(p13server-protocol,[
  --enable-p13server-protocol                (Default is enabled)
    The P13 protocol is the last textual AustNet server the server protocol.
    The protocol is very bulky, and only handles star-topology based
    networks. This protocol mains for compatibility back to austhex.servd7.x.
 ],[
   if test "$enableval" = "no"; then
     AC_MSG_RESULT(no)
     austhex_test_have_p13server_protocol=no
   else
     AC_MSG_RESULT(yes)
     AC_DEFINE(HAVE_P13SERVER_PROTOCOL)
     austhex_test_have_p13server_protocol=yes
   fi
 ],[
   AC_MSG_RESULT([yes (default)])
   AC_DEFINE(HAVE_P13SERVER_PROTOCOL)
   austhex_test_have_p13server_protocol=yes
 ])
AC_SUBST(austhex_test_have_p13server_protocol)

if test "$austhex_test_have_p13server_protocol" = "yes"; then
  AC_MSG_CHECKING([p13server-compress parameter])
  AC_ARG_ENABLE(p13server-compress,[
  --enable-p13server-compress                (Default is enabled)
    This allows server to server links using the P13 protocol (above) to be
    negotiated for compression which will significantly reduce the amount of
    data being transferred. Unfortunately due to the nature of this protocol
    and the tightness of the compression it will add a large amount of 
    memory usage and CPU load to the server, but it should not be noticable 
    on properly set up servers.
   ],[
     if test "$enableval" = "no"; then
       AC_MSG_RESULT(no)
       austhex_test_have_p13server_compress=no
     else
       AC_MSG_RESULT(yes)
       AC_DEFINE(HAVE_P13SERVER_COMPRESS)
       austhex_test_have_p13server_compress=yes
     fi
   ],[
     AC_MSG_RESULT([yes (default)])
     AC_DEFINE(HAVE_P13SERVER_COMPRESS)
     austhex_test_have_p13server_compress=yes
   ])

  AC_MSG_CHECKING([p13server-extended parameter])
  AC_ARG_ENABLE(p13server-extended,[
  --enable-p13server-extended                (Default is enabled)
    Previous versions of the P13 server to server protocol have certain
    flaws and absenses of commands. To correct this, the new additions to
    P13 allow backwards compatibility while fixing many of those problems.
    Unfortunately some servers cannot handle the new corrections and may
    work eratically, or become out of sync. The additions include
    compatibility with IPv6 and work around some very nasty byte-ordering
    issues that had become an issue. It is not recommended to disable this,
    however it may become necessary as a temporary solution. Some
    functionality may be lost with reverting to the non-extended P13.
   ],[
     if test "$enableval" = "no"; then
       AC_MSG_RESULT(no)
     else
       AC_MSG_RESULT(yes)
       AC_DEFINE(HAVE_P13SERVER_EXTENDED)
     fi
   ],[
     AC_MSG_RESULT([yes (default)])
     AC_DEFINE(HAVE_P13SERVER_EXTENDED)
   ])
else
  austhex_test_have_p13server_compress=no
fi
AC_SUBST(austhex_test_have_p13server_compress)
 
AC_MSG_CHECKING([p14server-protocol parameter])
AC_ARG_ENABLE(p14server-protocol,[
  --enable-p14server-protocol                (Default is disabled)
    The P14 protocol is a major advancement in IRC server to server
    protocols. While being tightly implemented, it allows for endless
    expandability using variable width identifiers, and helps assist security
    and lower bandwidth network-wide. The major component of this protocol
    is that it no longer requires a star-topology network design by
    implementing 'bone' and 'tendril' links thus allowing for well designed
    virtually split-free network design with minimal lag.
    This protocol is currently experimental and incomplete.
 ],[
   if test "$enableval" = "no"; then
     AC_MSG_RESULT(no)
     austhex_test_have_p14server_protocol=no
   else
     AC_MSG_RESULT(yes)
     AC_DEFINE(HAVE_P14SERVER_PROTOCOL)
     austhex_test_have_p14server_protocol=yes
   fi
 ],[
   AC_MSG_RESULT([no (default)])
   austhex_test_have_p14server_protocol=no
 ]) 
AC_SUBST(austhex_test_have_p14server_protocol)

if test "$austhex_test_have_p14server_protocol" = "yes"; then
  AC_MSG_CHECKING([p14server-compress parameter])
  AC_ARG_ENABLE(p14server-compress,[
  --enable-p14server-compress                (Default is enabled)
    This allows server to server links using the P14 protocol (above) to be
    negotiated for compression which will reduce the amount of data being 
    transferred. This compression is tight and will use noticable amounts of
    CPU usage.
   ],[
     if test "$enableval" = "no"; then
       AC_MSG_RESULT(no)
       austhex_test_have_p14server_compress=no
     else
       AC_MSG_RESULT(yes)
       AC_DEFINE(HAVE_P14SERVER_COMPRESS)
       austhex_test_have_p14server_compress=yes
     fi
   ],[
     AC_MSG_RESULT([yes (default)])
     AC_DEFINE(HAVE_P14SERVER_COMPRESS)
     austhex_test_have_p14server_compress=yes
   ])
else
  austhex_test_have_p14server_compress=no
fi
AC_SUBST(austhex_test_have_p14server_compress)

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
     austhex_test_have_debugging=no
   else
     if test "$enableval" = "psycho"; then
       AC_MSG_RESULT([yes (psycho)])
       AC_DEFINE(DEBUG_PSYCHO)
       AC_DEFINE(DEBUG_EXTENDED)
       AC_DEFINE(DEBUG)
       austhex_test_debugging=yes
     elif test "$enableval" = "extended"; then
       AC_MSG_RESULT([yes (extended)])
       AC_DEFINE(DEBUG_EXTENDED)
       AC_DEFINE(DEBUG)
       austhex_test_debugging=yes
     else
       AC_MSG_RESULT(yes)
       AC_DEFINE(DEBUG)
       austhex_test_debugging=yes
     fi
   fi
 ],[
   AC_MSG_RESULT([no (default)])
   austhex_test_debugging=no
 ]) 
AC_SUBST(austhex_test_debugging)


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
	install/Makefile
	make/defs
	make/rules
	src/Makefile
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
