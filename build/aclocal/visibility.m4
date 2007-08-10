dnl visibility.m4 serial 1 (gettext-0.15)
dnl Copyright (C) 2005 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

dnl From Bruno Haible.

dnl Modified for use in wxWidgets by Vaclav Slavik:
dnl    - don't define HAVE_VISIBILITY (=0) if not supported
dnl    - use -fvisibility-inlines-hidden too
dnl    - test in C++ mode

dnl Tests whether the compiler supports the command-line option
dnl -fvisibility=hidden and the function and variable attributes
dnl __attribute__((__visibility__("hidden"))) and
dnl __attribute__((__visibility__("default"))).
dnl Does *not* test for __visibility__("protected") - which has tricky
dnl semantics (see the 'vismain' test in glibc) and does not exist e.g. on
dnl MacOS X.
dnl Does *not* test for __visibility__("internal") - which has processor
dnl dependent semantics.
dnl Does *not* test for #pragma GCC visibility push(hidden) - which is
dnl "really only recommended for legacy code".
dnl Set the variable CFLAG_VISIBILITY.
dnl Defines and sets the variable HAVE_VISIBILITY.

AC_DEFUN([WX_VISIBILITY],
[
  AC_REQUIRE([AC_PROG_CC])
  if test -n "$GCC"; then
    CFLAGS_VISIBILITY="-fvisibility=hidden"
    CXXFLAGS_VISIBILITY="-fvisibility=hidden -fvisibility-inlines-hidden"
    AC_MSG_CHECKING([for symbols visibility support])
    AC_CACHE_VAL(wx_cv_cc_visibility, [
      wx_save_CXXFLAGS="$CXXFLAGS"
      CXXFLAGS="$CXXFLAGS $CXXFLAGS_VISIBILITY"
      AC_LANG_PUSH(C++)
      AC_TRY_COMPILE(
        [
         /* we need gcc >= 4.0, older versions with visibility support
            didn't have class visibility: */
         #if defined(__GNUC__) && __GNUC__ < 4
         error this gcc is too old;
         #endif
          
         extern __attribute__((__visibility__("hidden"))) int hiddenvar;
         extern __attribute__((__visibility__("default"))) int exportedvar;
         extern __attribute__((__visibility__("hidden"))) int hiddenfunc (void);
         extern __attribute__((__visibility__("default"))) int exportedfunc (void);
         class __attribute__((__visibility__("default"))) Foo {
           Foo() {}
         };
        ],
        [],
        wx_cv_cc_visibility=yes,
        wx_cv_cc_visibility=no)
      AC_LANG_POP()
      CXXFLAGS="$wx_save_CXXFLAGS"])
    AC_MSG_RESULT([$wx_cv_cc_visibility])
    if test $wx_cv_cc_visibility = yes; then
      AC_DEFINE([HAVE_VISIBILITY])

      dnl Debian/Ubuntu's gcc 4.1 is affected:
      dnl https://bugs.launchpad.net/ubuntu/+source/gcc-4.1/+bug/109262
      AC_MSG_CHECKING([for broken libstdc++ visibility])
      AC_CACHE_VAL(wx_cv_cc_broken_libstdcxx_visibility, [
        wx_save_CXXFLAGS="$CXXFLAGS"
        wx_save_LDFLAGS="$LDFLAGS"
        CXXFLAGS="$CXXFLAGS $CXXFLAGS_VISIBILITY"
        LDFLAGS="$LDFLAGS -shared -fPIC"
        AC_LANG_PUSH(C++)
        AC_TRY_LINK(
          [
            #include <string>
          ],
          [
            std::string s("hello");
            return s.length();
          ],
          wx_cv_cc_broken_libstdcxx_visibility=no,
          wx_cv_cc_broken_libstdcxx_visibility=yes)
        AC_LANG_POP()
        CXXFLAGS="$wx_save_CXXFLAGS"
        LDFLAGS="$wx_save_LDFLAGS"])
      AC_MSG_RESULT([$wx_cv_cc_broken_libstdcxx_visibility])
      if test $wx_cv_cc_broken_libstdcxx_visibility = yes; then
        AC_DEFINE([HAVE_BROKEN_LIBSTDCXX_VISIBILITY])
      fi

    else
      CFLAGS_VISIBILITY=""
      CXXFLAGS_VISIBILITY=""
    fi
    AC_SUBST([CFLAGS_VISIBILITY])
    AC_SUBST([CXXFLAGS_VISIBILITY])
  fi
])
