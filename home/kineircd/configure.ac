dnl ./configure.in
dnl AutoConf configuration script for austhex
dnl Simon Butcher <pickle@austnet.org>


dnl -----
dnl Setup
dnl -----
AC_PREREQ(2.52)
AC_INIT(src/Makefile.in)
AC_CONFIG_AUX_DIR(autoconf)
AC_CONFIG_HEADER(include/config.h)

VERSION=8.0
AC_SUBST(VERSION)

dnl Save the LDFLAGS since we will be making a mess of it soon..
LDFLAGS_SAVE="$LDFLAGS"

INCDIRS=""


dnl ------------------------
dnl Fix up 'default' defines
dnl ------------------------
AC_MSG_CHECKING([what to set CXXFLAGS to])
if test "$CXXFLAGS" = ""; then
  CXXFLAGS="-Wall -O2 -fomit-frame-pointer -finline-functions"
  AC_MSG_RESULT(['$CXXFLAGS' (default)])
else
  AC_MSG_RESULT(['$CXXFLAGS' (given)])
fi


dnl -------------------
dnl Checks for programs
dnl -------------------
AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_CHECK_PROG(RM, rm, rm -f, del)


dnl --------------------
dnl Checks for libraries
dnl --------------------
dnl (none)


dnl -----------------------
dnl Checks for header files
dnl -----------------------
AC_HEADER_STDC
AC_TIME_WITH_SYS_TIME
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


dnl -------------------------------------------------------------
dnl Checks for typedefs, structures, and compiler characteristics
dnl -------------------------------------------------------------
AC_C_BIGENDIAN
AC_C_CONST
AC_C_INLINE
AC_C_CHAR_UNSIGNED
AC_C_LONG_DOUBLE
AC_HEADER_STAT
AC_HEADER_TIME
AC_STRUCT_TM
AC_STRUCT_TIMEZONE
AC_DECL_SYS_SIGLIST
AC_TYPE_SIZE_T
AC_TYPE_PID_T
AC_TYPE_MODE_T


dnl ----------------------------
dnl Checks for library functions
dnl ----------------------------
AC_TYPE_SIGNAL
AC_FUNC_ERROR_AT_LINE
AC_FUNC_FORK
AC_TYPE_SIGNAL
AC_FUNC_VPRINTF
AC_CHECK_FUNCS( \
	inet_ntoa \
	memset \
	select \
	socket \
	strchr \
		)


dnl ----------------
dnl Look for OpenSSL
dnl ----------------
AC_MSG_CHECKING([for OpenSSL library/include path])
AC_ARG_WITH(ssl,[
  --with-ssl=DIRECTORY    location of SSL library/include files
 ],[
 	dnl Check the given path
	if test -f "$dir/include/openssl/ssl.h"; then
		ssl_dir="$dir"
		ssl_libs="$dir/lib"
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
      AC_MSG_WARN([Your OpenSSL installation seems incomplete!])
     ])
  else
    AC_MSG_WARN(half found)
    echo "It appears your OpenSSL does not contain a library directory!"
    echo "Use --with-ssl=DIRECTORY to corret this if you beleive this is wrong."
  fi
else
  AC_MSG_WARN(not found)
  echo "Use --with-ssl=DIRECTORY to correct this if you beleive this is wrong."
fi
AC_SUBST(ssl_lib)
AC_SUBST(ssl_ldflags)


dnl ---------------------
dnl Check system services
dnl ---------------------
AC_EXEEXT
AC_OBJEXT


dnl ---------------------------
dnl Run local test programs (C)
dnl ---------------------------
AC_LANG_C

AC_MSG_CHECKING(for struct sigcontext in signal handlers)
AC_TRY_COMPILE([#include <sys/signal.h>],[struct sigcontext c;], 
[AC_DEFINE(HAVE_STRUCT_SIGCONTEXT) AC_MSG_RESULT(yes)],[AC_MSG_RESULT(no)])

AC_MSG_CHECKING(whether signal SIGCHLD is brain-damaged)
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
	   
	   if (level++ > 2)
	     exit(1);
	   
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
 ],
 [AC_MSG_RESULT([no (good)])],
 [AC_DEFINE(SIGCHLD_BRAIN_DAMAGE) AC_MSG_RESULT(yes)])

AC_MSG_CHECKING([maximum number of files I can open at once])
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
 ],
 [MAX_OPEN_FD=`cat conftestval` 
  AC_MSG_RESULT($MAX_OPEN_FD)],
 [MAX_OPEN_FD=256
  echo
  AC_MSG_WARN(assuming 256)])
AC_DEFINE_UNQUOTED(MAX_FD_PER_PROCESS, $MAX_OPEN_FD)

AC_MSG_CHECKING([default select() file descriptor bit-set size])
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
 ],
 [SELECT_FDSET_SIZE=`cat conftestval` 
  AC_MSG_RESULT($SELECT_FDSET_SIZE)],
 [SELECT_FDSET_SIZE=256
  echo
  AC_MSG_WARN(assuming 256)])
AC_DEFINE_UNQUOTED(SELECT_FDSET_NUMFDS, $SELECT_FDSET_SIZE)


dnl -----------------------------
dnl Run local test programs (C++)
dnl -----------------------------
AC_LANG_CPLUSPLUS

AC_MSG_CHECKING([for a working C++ Standard Template Library])
AC_TRY_RUN([
	#include <stl.h>
	
	int main(void)
	{
	   list<int> l;
	   l.clear();
	   
	   map<int, int> m;
	   m.clear();
	}
 ],
 [AC_DEFINE(HAVE_CXX_STL) AC_MSG_RESULT(yes)], 
 [echo
  AC_MSG_ERROR([Sorry, you need a working Standard Template Library])])

AC_MSG_CHECKING(for an SGI compliant hash capability in your STL)
AC_TRY_RUN([
	#include <hash_map.h>

	int main(void)
 	{
	   hash_map<int, int> hm;
	   hm.clear();
	}
 ],
 [AC_DEFINE(STL_HAS_HASH) AC_MSG_RESULT(yes)],
 [AC_MSG_RESULT(no)])

AC_MSG_CHECKING(for an SGI compliant single list template in your STL)
AC_TRY_RUN([
	#include <slist.h>

	int main(void)
 	{
	   slist<int> sl;
	   sl.clear();
	   
	   exit(0);
	}
 ],
 [AC_DEFINE(STL_HAS_SLIST) AC_MSG_RESULT(yes)],
 [AC_MSG_RESULT(no)])


dnl -------------------------------
dnl Last chance to fix things up :)
dnl -------------------------------
LDFLAGS="$LDFLAGS_SAVE"
AC_SUBST(INCDIRS)


dnl --------------------------------
dnl Make the files we needed to edit
dnl --------------------------------
AC_OUTPUT(Makefile \
	install/Makefile \
	src/Makefile)


dnl ------------------------------------------
dnl Print a pretty message for the nice people
dnl ------------------------------------------
echo
echo "Done!"
echo
echo "Type 'make' or 'gmake' to build the package."
echo
