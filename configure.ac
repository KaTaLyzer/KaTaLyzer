#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([KaTaLyzer], [1.0], [akozemcak@gmail.com])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_CONFIG_SRCDIR([src/backend/variables.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_MAKE_SET

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h fcntl.h netdb.h netinet/in.h stdint.h stdlib.h string.h sys/ioctl.h sys/socket.h sys/time.h unistd.h mysql/mysql.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T

# Checks for preprocessor symbols

AC_ARG_WITH([debug_k],
			[AS_HELP_STRING([--with-debug-k], [Debug katalyzer])],
			[AC_DEFINE(_DEBUG_K, [], [enable debug])],
			[])
AC_ARG_WITH([cdp],
			[AS_HELP_STRING([--without-cdp], [Do not use cdp])],
			[],
			[AC_DEFINE(CDP, [], [ support cdp])])
AC_ARG_WITH([sip],
			[AS_HELP_STRING([--without-sip], [Do not use sip])],
			[],
			[AC_DEFINE(_SIP, [], [support sip])])
AC_ARG_WITH([netflow],
			[AS_HELP_STRING([--without-netflow], [Do not use netflow])],
			[],
			[AC_DEFINE(NETFLOW, [], [support netflow])])

AC_ARG_WITH([pcap],
			[AS_HELP_STRING([--with-pcap], [Use of the Pcap libraries])],
			[AC_DEFINE(_PCAP,[], [use pcap]) PCAP_LIBS="-lpcap"],
			[])
AC_SUBST([PCAP_LIBS])

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([gettimeofday inet_ntoa memset pow socket strerror strstr strtol])

AC_CONFIG_FILES([Makefile
                 src/backend/Makefile])
AC_OUTPUT
