AC_PREREQ([2.69])
AC_INIT([centerim5], [m4_esyscmd([misc/git-version-gen version])],
	[centerim-devel@centerim.org], [centerim5], [http://www.centerim.org/])
# Prevent AC_PROG_CC, AC_PROG_CXX from initializing CFLAGS and CXXFLAGS.
: ${CFLAGS=""}
: ${CXXFLAGS=""}
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_AUX_DIR([config])
AC_CONFIG_HEADER([config.h])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([-Wall subdir-objects])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_GNU_SOURCE

# Checks for programs.
AC_PROG_CC
AC_PROG_CC_C99
AC_PROG_CXX
AX_CXX_COMPILE_STDCXX([11], [noext])
AM_PROG_AR

LT_INIT([disable-static])

# Set compiler flags.

AC_ARG_ENABLE([strict], [AC_HELP_STRING([--enable-strict],
	[enable extra compiler warnings during build])])
AC_ARG_ENABLE([debug], [AC_HELP_STRING([--enable-debug],
	[compile with debugging support])])

# If not --enable-debug then try to set -O2 as default.
AS_IF([test "$enable_debug" != yes],
	[save_cflags="$CFLAGS"]
	[CFLAGS="-O2"]
	[AC_MSG_CHECKING([whether CC supports -O2])]
	[AC_LANG_PUSH([C])]
	[AC_COMPILE_IFELSE([AC_LANG_PROGRAM([])],
		[AC_MSG_RESULT([yes])]
		[AM_CFLAGS="$AM_CFLAGS -O2"],
		[AC_MSG_RESULT([no])])]
	[AC_LANG_POP([C])]
	[CFLAGS="$save_cflags"]

	[save_cxxflags="$CXXFLAGS"]
	[CXXFLAGS="-O2"]
	[AC_MSG_CHECKING([whether CXX supports -O2])]
	[AC_LANG_PUSH([C++])]
	[AC_COMPILE_IFELSE([AC_LANG_PROGRAM([])],
		[AC_MSG_RESULT([yes])]
		[AM_CXXFLAGS="$AM_CXXFLAGS -O2"],
		[AC_MSG_RESULT([no])])]
	[AC_LANG_POP([C++])]
	[CXXFLAGS="$save_cxxflags"])

# If --enable-strict is specified then enable compilation warnings.
AS_IF([test "$enable_strict" = yes],
	[save_cflags="$CFLAGS"]
	[CFLAGS="-Wall -Wextra -pedantic"]
	[AC_MSG_CHECKING([whether CC supports -Wall -Wextra -pedantic])]
	[AC_LANG_PUSH([C])]
	[AC_COMPILE_IFELSE([AC_LANG_PROGRAM([])],
		[AC_MSG_RESULT([yes])],
		[AC_MSG_RESULT([no])]
		[AC_MSG_ERROR([the C compiler does not recognize options -Wall -Wextra -pedantic that are needed by --enable-strict])])]
	[AC_LANG_POP([C])]
	[CFLAGS="$save_cflags"]
	[AM_CFLAGS="$AM_CFLAGS -Wall -Wextra -pedantic"]

	[save_cxxflags="$CXXFLAGS"]
	[CXXFLAGS="-Wall -Wextra -pedantic -Wno-long-long -Wsuggest-override"]
	[AC_MSG_CHECKING([whether CXX supports -Wall -Wextra -pedantic -Wno-long-long -Wsuggest-override])]
	[AC_LANG_PUSH([C++])]
	[AC_COMPILE_IFELSE([AC_LANG_PROGRAM([])],
		[AC_MSG_RESULT([yes])],
		[AC_MSG_RESULT([no])]
		[AC_MSG_ERROR([the C++ compiler does not recognize options -Wall -Wextra -pedantic -Wno-long-long -Wsuggest-override that are needed by --enable-strict])])]
	[AC_LANG_POP([C++])]
	[CXXFLAGS="$save_cxxflags"]
	[AM_CXXFLAGS="$AM_CXXFLAGS -Wall -Wextra -pedantic -Wno-long-long -Wsuggest-override"])

# If --enable-debug is specified then compile with -g -O0.
AS_IF([test "$enable_debug" = yes],
	[save_cflags="$CFLAGS"]
	[CFLAGS="-g -O0"]
	[AC_MSG_CHECKING([whether CC supports -g -O0])]
	[AC_LANG_PUSH([C])]
	[AC_COMPILE_IFELSE([AC_LANG_PROGRAM([])],
		[AC_MSG_RESULT([yes])],
		[AC_MSG_RESULT([no])]
		[AC_MSG_ERROR([the C compiler does not recognize options -g -O0 that are needed by --enable-debug])])]
	[AC_LANG_POP([C])]
	[CFLAGS="$save_cflags"]
	[AM_CFLAGS="$AM_CFLAGS -g -O0"]

	[save_cxxflags="$CXXFLAGS"]
	[CXXFLAGS="-g -O0"]
	[AC_MSG_CHECKING([whether CXX supports -g -O0])]
	[AC_LANG_PUSH([C++])]
	[AC_COMPILE_IFELSE([AC_LANG_PROGRAM([])],
		[AC_MSG_RESULT([yes])],
		[AC_MSG_RESULT([no])]
		[AC_MSG_ERROR([the C++ compiler does not recognize options -g -O0 that are needed by --enable-debug])])]
	[AC_LANG_POP([C++])]
	[CXXFLAGS="$save_cxxflags"]
	[AM_CXXFLAGS="$AM_CXXFLAGS -g -O0"])
AC_SUBST([AM_CFLAGS])
AC_SUBST([AM_CXXFLAGS])
AS_IF([test "$enable_debug" = yes],
	[AC_DEFINE([DEBUG], [1], [Define if debugging is enabled.])])

# Checks for libraries.

AM_ICONV()
AS_IF([test "$am_cv_func_iconv" != yes],
	[AC_MSG_ERROR([iconv not found])])

# gettext -- v0.19.3 was released on 2014-10-16.
AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.19.3])

# libpurple -- v2.9.0 is needed because of PurpleConvChatBuddy::ui_data, this
# version was released on 2011-06-23.
# find . \( -name \*.c -or -name \*.cpp -o -name \*.h \) -print0 | xargs -0 sed -n 's/.*\(purple_[^( ]*\)(.*/\1/p' | sort -u | less
PKG_CHECK_MODULES([PURPLE], [purple >= 2.7.0])

# glib -- v2.32.0 is needed because of g_environ_setenv(), this version was
# released on 2012-03-24.
# find . \( -name \*.c -o -name \*.cpp -o -name \*.h \) -print0 | xargs -0 sed -n 's/.*\(g_[^ (]*\)(.*/\1/p' | sort -u | less
PKG_CHECK_MODULES([GLIB], [glib-2.0 >= 2.32.0])

# libsigc++ -- v2.2.0 was released on 2008-02-22.
PKG_CHECK_MODULES([SIGC], [sigc++-2.0 >= 2.2.0])

# ncursesw -- v5.8 was released on 2011-02-26.
PKG_CHECK_MODULES([NCURSESW], [ncursesw >= 5.8])

# Checks for tools needed by the termex test framework. Only warn about these
# missing dependencies.

# tic
AC_PATH_PROG([TIC], [tic])
AC_ARG_VAR([TIC], [path to tic])
AS_IF([test -z "$TIC"],
	[AC_MSG_WARN([tic was not found, running tests will not be possible])])

# python -- v3.5 is needed for termex.py (see the script for details), this
# version was released on 2015-09-13.
AC_PATH_PROG([PYTHON], [python3])
AC_ARG_VAR([PYTHON], [path to python3])
AS_IF([test -n "$PYTHON"],
	[AC_MSG_CHECKING([python3 is at least version 3.5])]
	[python_version_ok=`$PYTHON -c 'import sys; print(sys.hexversion >= 0x03050000)'`]
	[AS_IF([test "$python_version_ok" == True],
		[AC_MSG_RESULT([yes])],
		[AC_MSG_RESULT([no])])])
AS_IF([test -z "$PYTHON" -o "$python_version_ok" != True],
	[AC_MSG_WARN([python >= 3.5 was not found, running tests will not be possible])])

AM_CONDITIONAL([TERMEX_TESTS],
	[test -n "$TIC" -a -n "$PYTHON" -a "$python_version_ok" == True])

# CIM_RECURSIVE_EVAL(VALUE, RESULT)
# =================================
# Interpolate the VALUE in loop until it does not change, and set the result
# to $RESULT.
# WARNING: It is easy to get an infinite loop with some unsane input.
# Taken from http://ac-archive.sourceforge.net/adl/relpaths.html.
AC_DEFUN([CIM_RECURSIVE_EVAL],
[_lcl_receval="$1"
$2=`(test "$prefix" = NONE && prefix="$ac_default_prefix"
	test "$exec_prefix" = NONE && exec_prefix="$prefix"
	_lcl_receval_old=''
	while test "[$]_lcl_receval_old" != "[$]_lcl_receval"; do
		_lcl_receval_old="[$]_lcl_receval"
		eval _lcl_receval="\"[$]_lcl_receval\""
	done
	echo "[$]_lcl_receval")`])

# Define locale and pkgdir directories. It would be nicer to do this in
# Makefile.ams by adding appropriate '-D' compiler options to respective
# CPPFLAGS, which is a common solution that can be seen in other projects. In
# that case, it is not needed to resolve the paths at the configure time using
# the CIM_RECURSIVE_EVAL function. However this solution unfortunately does not
# work correctly when values of the defines are changed, the source files that
# use these defines do not get correctly recompiled. To fix this, the defines
# have to be put into the config.h file.
CIM_RECURSIVE_EVAL([$localedir], [localedir_eval])
AC_DEFINE_UNQUOTED([LOCALEDIR], ["$localedir_eval"],
	[Define to the locale directory.])

CIM_RECURSIVE_EVAL([$libdir/AC_PACKAGE_NAME], [pkglibdir_eval])
AC_DEFINE_UNQUOTED([PKGLIBDIR], ["$pkglibdir_eval"],
	[Define to the pkglib directory.])

# Create output files.
AC_CONFIG_FILES([Makefile
		 contrib/Makefile
		 cppconsui/Makefile
		 doc/Makefile
		 doc/doxygen-cppconsui.conf
		 misc/Makefile
		 plugins/Makefile
		 po/Makefile.in
		 src/Makefile
		 tests/Makefile])
AC_OUTPUT
