dnl ./configure.in
dnl AutoConf configuration script for austhex
dnl Simon Butcher <pickle@austnet.org>


dnl  -------------------------------------------------------------------------
echo " -=> Getting ready to configure..."
dnl  -------------------------------------------------------------------------
AC_PREREQ(2.52)
AC_INIT(src/Makefile.in)
AC_CONFIG_AUX_DIR(autoconf)
AC_CONFIG_HEADER(include/config.h)

PROGNAME=austhex.servd
VERSION=8.0
AC_SUBST(PROGNAME)
AC_SUBST(VERSION)

dnl Save the LDFLAGS since we will be making a mess of it soon..
LDFLAGS_SAVE="$LDFLAGS"

INCDIRS=""


dnl  -------------------------------------------------------------------------
echo " -=> Checking 'default' definitions"
dnl  -------------------------------------------------------------------------
AC_MSG_CHECKING([what to set CXXFLAGS to])
if test "$CXXFLAGS" = ""; then
  CXXFLAGS="-Wall -O2 -fomit-frame-pointer -finline-functions"
  AC_MSG_RESULT(['$CXXFLAGS' (default)])
else
  AC_MSG_RESULT(['$CXXFLAGS' (given)])
fi


dnl  -------------------------------------------------------------------------
echo " -=> Checking for programs we need"
dnl  -------------------------------------------------------------------------
AC_PROG_CC
AC_PROG_GCC_TRADITIONAL
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_PROG_LN_S
AC_CHECK_PROG(RM, rm, rm -f, del)
AC_CACHE_SAVE


dnl  -------------------------------------------------------------------------
echo " -=> Checking compiler and system characteristics"
dnl  -------------------------------------------------------------------------
AC_AIX
AC_MINIX
AC_ISC_POSIX
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
     echo "  It is recommended that you edit config.h manually to correct this"
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
     echo "  It is recommended that you edit config.h manually to correct this"
   ])
 ])
AC_DEFINE_UNQUOTED(SELECT_FDSET_NUMFDS, $austhex_cv_sys_fdset_numfds)

AC_CACHE_SAVE


dnl  -------------------------------------------------------------------------
echo " -=> Checking structures and types"
dnl  -------------------------------------------------------------------------
AC_DECL_SYS_SIGLIST
AC_STRUCT_TM
AC_STRUCT_TIMEZONE
AC_TYPE_SIZE_T
AC_TYPE_PID_T
AC_TYPE_MODE_T
AC_TYPE_UID_T
AC_TYPE_SIGNAL
AC_CACHE_SAVE


dnl  -------------------------------------------------------------------------
echo " -=> Checking for header files"
dnl  -------------------------------------------------------------------------
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
		)
AC_CACHE_SAVE


dnl  -------------------------------------------------------------------------
echo " -=> Checking for libraries and specific library functions"
dnl  -------------------------------------------------------------------------
AC_CHECK_LIB(pthread, pthread_self,,[
  AC_MSG_RESULT(no)
  dnl should check for other threads libraries here. For now, just complain
  AC_MSG_WARN([A threads library will be needed in the future..])
 ])
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

AC_MSG_CHECKING([for OpenSSL library/include path])
AC_ARG_ENABLE(ssl,[  --enable-ssl=DIRECTORY        location of SSL library/include files],
 [
 	dnl Check the given path
	if test -f "$enableval/include/openssl/ssl.h"; then
		ssl_dir="$enableval"
		ssl_libs="$enableval/lib"
	fi
 ],[
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
 ])
dnl Check that we are not being told to skip this...
if test "$ssl_dir" = "no"; then
  AC_MSG_RESULT(ignoring)
else
  dnl Check that the 'ssl_dir' environment variable is not empty
  if test -n "$ssl_dir"; then
    dnl Check that the 'ssl_libs' environment is a directory
    if test -d "$ssl_libs"; then
      AC_MSG_RESULT($ssl_dir)
      AC_DEFINE(HAVE_OPENSSL)
      INCDIRS="$INCDIRS -I$ssl_dir/include"
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
       ])
    else
      AC_MSG_RESULT(unknown)
      AC_MSG_WARN([Only found half of an OpenSSL installation!])
      echo "  It appears your OpenSSL installation does not contain a library directory!"
      echo "  Use --with-ssl=DIRECTORY to corret this if you beleive this is wrong."
      echo "  Without this library you will not be able to use secured SSL connections."
    fi
  else
    AC_MSG_RESULT(unknown)
    AC_MSG_WARN([Could not find your OpenSSL installation])
    echo "  Use --with-ssl=DIRECTORY to correct this if you beleive this is wrong."
    echo "  Without this library you will not be able to use secured SSL connections."
  fi
fi
AC_SUBST(ssl_lib)
AC_SUBST(ssl_ldflags)

AC_CACHE_CHECK([for a working C++ Standard Template Library],
 austhex_cv_lib_stl_ok,[
  AC_LANG_CPLUSPLUS
  AC_TRY_RUN([
	#include <stl.h>
	
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
	#include <hash_map.h>

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
	#include <slist.h>

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


dnl  -------------------------------------------------------------------------
echo " -=> Determining compilation configuration parameters"
dnl  -------------------------------------------------------------------------
AC_MSG_CHECKING([allow-clients parameter])
AC_ARG_ENABLE(allow-clients,[  --enable-allow-clients        allow connections from users (clients)],
 [
   if test "$enableval" = "no"; then
     AC_MSG_RESULT(no)
   else
     AC_MSG_RESULT(yes)
     AC_DEFINE(ALLOW_CLIENT_CONNECTIONS)

     dnl This uses user protocols so we need this
     austhex_test_enable_user_protocols="yes"
   fi
 ],[
   AC_MSG_RESULT([yes (default)])
   AC_DEFINE(ALLOW_CLIENT_CONNECTIONS)

   dnl This uses user protocols so we need this
   austhex_test_enable_user_protocols="yes"
 ])

AC_MSG_CHECKING([allow-servers parameter])
AC_ARG_ENABLE(allow-servers,[  --enable-allow-servers        allow connections from servers],
 [
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
AC_ARG_ENABLE(allow-services,[  --enable-allow-services       allow connections from services (clients)],
 [
   if test "$enableval" = "no"; then
     AC_MSG_RESULT(no)
   else
     AC_MSG_RESULT(yes)
     AC_DEFINE(ALLOW_SERVICE_CONNECTIONS)
     
     dnl This uses user protocols so we need this
     austhex_test_enable_user_protocols="yes"
   fi
 ],[
   AC_MSG_RESULT([no (default)])
 ])

AC_MSG_CHECKING([convert-motd-tabs parameter])
AC_ARG_ENABLE(convert-motd-tabs,[  --enable-convert-motd-tabs    fix motd tabs into spaces for dumb GUI clients],
 [
   if test "$enableval" = "no"; then
     AC_MSG_RESULT(no)
   else
     AC_MSG_RESULT(yes)
     AC_DEFINE(CONVERT_MOTD_TABS)
   fi
 ],[
   AC_MSG_RESULT([yes (default)])
   AC_DEFINE(CONVERT_MOTD_TABS)
 ])

AC_MSG_CHECKING([drop-lame-servers parameter])
AC_ARG_ENABLE(drop-lame-servers,[  --enable-drop-lame-servers    drop out of sync (lame) servers],
 [
   if test "$enableval" = "no"; then
     AC_MSG_RESULT(no)
   else
     AC_MSG_RESULT(yes)
     AC_DEFINE(DROP_BRAINDEAD_SERVERS)
   fi
 ],[
   AC_MSG_RESULT([no (default)])
 ])

AC_MSG_CHECKING([floodlock-away-reply parameter])
AC_ARG_ENABLE(floodlock-away-reply,[  --enable-floodlock-away-reply set the away-reply floodlock timeout],
 [
   if test "$enableval" = "no"; then
     AC_MSG_RESULT(off)
   else
     if test "$enableval" -gt 0; then
       AC_MSG_RESULT([$enableval seconds])
       AC_DEFINE_UNQUOTED(FLOODLOCK_AWAY_REPLY, $enableval)
     else
       AC_MSG_RESULT([30 seconds (default)])
       AC_DEFINE_UNQUOTED(FLOODLOCK_AWAY_REPLY, 30)
     fi
   fi
 ],[
   AC_MSG_RESULT([30 seconds (default)])
   AC_DEFINE_UNQUOTED(FLOODLOCK_AWAY_REPLY, 30)
 ])

AC_MSG_CHECKING([floodlock-away-set parameter])
AC_ARG_ENABLE(floodlock-away-set,[  --enable-floodlock-away-set   set the away-setting floodlock timeout],
 [
   if test "$enableval" = "no"; then
     AC_MSG_RESULT(off)
   else
     if test "$enableval" -gt 0; then
       AC_MSG_RESULT([$enableval seconds])
       AC_DEFINE_UNQUOTED(FLOODLOCK_AWAY_SET, $enableval)
     else
       AC_MSG_RESULT([45 seconds (default)])
       AC_DEFINE_UNQUOTED(FLOODLOCK_AWAY_SET, 45)
     fi
   fi
 ],[
   AC_MSG_RESULT([45 seconds (default)])
   AC_DEFINE_UNQUOTED(FLOODLOCK_AWAY_SET, 45)
 ])

AC_MSG_CHECKING([helpers-see-umodes parameter])
AC_ARG_ENABLE(helpers-see-umodes,[  --enable-helpers-see-umodes   allow helpers/opers to see other users' modes],
 [
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

AC_MSG_CHECKING([ircii-kluge parameter])
AC_ARG_ENABLE(ircii-kluge,[  --enable-ircii-kluge          kluge to allow old ircII clients to work],
 [
   if test "$enableval" = "no"; then
     AC_MSG_RESULT(no)
   else
     AC_MSG_RESULT(yes)
     AC_DEFINE(BLOODY_IRCII_KLUGE)
   fi
 ],[
   AC_MSG_RESULT([yes (default)])
   AC_DEFINE(BLOODY_IRCII_KLUGE)
 ])

AC_MSG_CHECKING([match-counting parameter])
AC_ARG_ENABLE(match-counting,[  --enable-match-counting       add match counts to end of listing numerics],
 [
   if test "$enableval" = "no"; then
     AC_MSG_RESULT(no)
   else
     AC_MSG_RESULT(yes)
     AC_DEFINE(DO_MATCH_COUNTING)
   fi
 ],[
   AC_MSG_RESULT([yes (default)])
   AC_DEFINE(DO_MATCH_COUNTING)
 ])

AC_MSG_CHECKING([oper-wallops parameter])
AC_ARG_ENABLE(oper-wallops,[  --enable-oper-wallops         allow operators to use WALLOPS (can be abused)],
 [
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
AC_ARG_ENABLE(paranoid-opers,[  --enable-paranoid-opers       operators need to know when they are /whois'd],
 [
   if test "$enableval" = "no"; then
     AC_MSG_RESULT(no)
   else
     AC_MSG_RESULT(yes)
     AC_DEFINE(NOTIFY_PARANOID_OPERS_ON_WHOIS)
   fi
 ],[
   AC_MSG_RESULT([no (default)])
 ])

AC_MSG_CHECKING([passive-registration parameter])
AC_ARG_ENABLE(passive-registration,[  --enable-passive-registration report no errors on connection registrations],
 [
   if test "$enableval" = "no"; then
     AC_MSG_RESULT(no)
   else
     AC_MSG_RESULT(yes)
     AC_DEFINE(PASSIVE_REGISTRATION)
   fi
 ],[
   AC_MSG_RESULT([no (default)])
 ])

AC_MSG_CHECKING([strict-channel-names parameter])
AC_ARG_ENABLE(strict-channel-names,[  --enable-strict-channel-names enforce strict channel names to avoid spoofs],
 [
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
AC_ARG_ENABLE(strict-registration,[  --enable-strict-registration  enforce strict connection registration],
 [
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

AC_MSG_CHECKING([traceroute parameter])
AC_ARG_ENABLE(traceroute,[  --enable-traceroute           allow the TRACEROUTE command (can be abused)],
 [
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

AC_MSG_CHECKING([user-conn-ping parameter])
AC_ARG_ENABLE(user-conn-ping,[  --enable-user-conn-ping       enable anti-spoof connection ping requirement],
 [
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
AC_ARG_ENABLE(warn-bad-reg-cmds,[  --enable-warn-bad-reg-cmds    complain about unknown registration commands],
 [
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
AC_MSG_CHECKING([protocol-irc2user parameter])
if test "$austhex_test_enable_user_protocols" = "yes"; then
  AC_ARG_ENABLE(protocol-irc2user,[  --enable-protocol-irc2user    allow the irc2 user/client protocol],
   [
     if test "$enableval" = "no"; then
       AC_MSG_RESULT(no)
     else
       AC_MSG_RESULT(yes)
       AC_DEFINE(HAVE_PROTOCOL_IRC2USER)
     fi
   ],[
     AC_MSG_RESULT([yes (default)])
     AC_DEFINE(HAVE_PROTOCOL_IRC2USER)
   ])
else
  AC_MSG_RESULT([no (clients disallowed)])
fi

AC_MSG_CHECKING([protocol-p13server parameter])
AC_ARG_ENABLE(protocol-p13server,[  --enable-protocol-p13server   allow the P13 server protocol],
 [
   if test "$enableval" = "no"; then
     AC_MSG_RESULT(no)
   else
     AC_MSG_RESULT(yes)
     AC_DEFINE(HAVE_PROTOCOL_P13SERVER)
   fi
 ],[
   AC_MSG_RESULT([yes (default)])
   AC_DEFINE(HAVE_PROTOCOL_P13SERVER)
 ])
 
AC_MSG_CHECKING([protocol-p14server parameter])
AC_ARG_ENABLE(protocol-p14server,[  --enable-protocol-p14server   allow the P14 server protocol],
 [
   if test "$enableval" = "no"; then
     AC_MSG_RESULT(no)
   else
     AC_MSG_RESULT(yes)
     AC_DEFINE(HAVE_PROTOCOL_P14SERVER)
   fi
 ],[
   AC_MSG_RESULT([no (default)])
 ]) 


dnl  -------------------------------------------------------------------------
echo " -=> Determining installation information"
dnl  -------------------------------------------------------------------------
dnl stuff goes here, would like to do it 'apache style' tho :)


dnl  -------------------------------------------------------------------------
echo " -=> Creating files"
dnl  -------------------------------------------------------------------------
LDFLAGS="$LDFLAGS_SAVE"
AC_SUBST(INCDIRS)
AC_OUTPUT([
	Makefile
	install/Makefile
	src/Makefile
	])


echo
echo "Done!"
echo
echo "Type 'make' to build the package."
echo
