 AC_INIT(foobar, 1.0, me@mail.com)
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_SRCDIR(src/main.cpp)

AM_INIT_AUTOMAKE([foreign])

AC_PROG_CXX([clang++ gcc])
AX_CHECK_COMPILE_FLAG([-std=c++11], [CXXFLAGS="$CXXFLAGS -std=c++11"])
AC_SUBST([AM_CXXFLAGS], ['-std=-D__STRICT_ANSI__ -Wall -Wextra'])

AC_OUTPUT(Makefile src/Makefile)
