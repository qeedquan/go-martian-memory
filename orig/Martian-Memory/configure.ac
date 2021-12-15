#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT(Martian Memory, 1.0, T-1000@Softhome.net)
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE(Martian Memory ,1.0)
AC_CONFIG_SRCDIR([src/])
#AC_CONFIG_HEADER([config.h])

AC_DEFINE([DATADIR],["/usr/local/share/martian"],[Data directory])
AC_DEFINE([SCOREDIR],["/var/lib/games/martian"],[Score directory])

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_MAKE_SET

# Checks for libraries.
SDL_VERSION=1.2.0
AM_PATH_SDL($SDL_VERSION,
	    :,
	    AC_MSG_ERROR([*** SDL version $SDL_VERSION not found. Visit http://www.libsdl.org and get it])
)
AC_CHECK_LIB(SDL_mixer, Mix_OpenAudio,, AC_MSG_ERROR([*** SDL_mixer not found. Visit http://www.libsdl.org and get it]))
AC_CHECK_LIB(pthread, pthread_create)
                                                                                                                                             
CFLAGS="$CFLAGS $SDL_CFLAGS"
CXXFLAGS="$CXXFLAGS $SDL_CFLAGS"
LIBS="$LIBS $SDL_LIBS"

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL

# Checks for library functions.
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([strdup])

if test "x$prefix" = xNONE; then
   prefix="${ac_default_prefix}"
fi

AC_ARG_ENABLE(enable-devel,
        [  --enable-devel    enables development paths],
        [enable_devel=$enableval])
                                                                                                                                             
if test "$enable_devel" = "yes"; then
        DATADIR="."
        SCOREDIR="."
else
        DATADIR="$datadir/Martian"
        SCOREDIR="/var/lib/games/Martian"
fi
                                                                                                                                             
eval DATAPATH=`eval echo "$DATADIR"`
eval SCOREPATH=`eval echo "$SCOREDIR"`
AC_DEFINE_UNQUOTED(DATADIR, "$DATAPATH")
AC_DEFINE_UNQUOTED(SCOREDIR, "$SCOREPATH")
AC_SUBST(DATAPATH)
AC_SUBST(SCOREPATH)
                                                                                                                                             
AC_CONFIG_FILES([Makefile
		 data/Makefile
                 docs/Makefile
                 fonts/Makefile
                 gfx/Makefile
                 music/Makefile
                 sounds/Makefile
                 src/Makefile])


AC_OUTPUT