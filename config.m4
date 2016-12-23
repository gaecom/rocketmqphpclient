dnl $Id$
dnl config.m4 for extension rocketmqclient

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

PHP_ARG_WITH(rocketmqclient, for rocketmqclient support,
Make sure that the comment is aligned:
[  --with-rocketmqclient             Include rocketmqclient support])

dnl Otherwise use enable:

dnl PHP_ARG_ENABLE(rocketmqclient, whether to enable rocketmqclient support,
dnl Make sure that the comment is aligned:
dnl [  --enable-rocketmqclient           Enable rocketmqclient support])

if test "$PHP_ROCKETMQCLIENT" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-rocketmqclient -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/rocketmqclient.h"  # you most likely want to change this
  dnl if test -r $PHP_ROCKETMQCLIENT/$SEARCH_FOR; then # path given as parameter
  dnl   ROCKETMQCLIENT_DIR=$PHP_ROCKETMQCLIENT
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for rocketmqclient files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       ROCKETMQCLIENT_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$ROCKETMQCLIENT_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the rocketmqclient distribution])
  dnl fi

  dnl # --with-rocketmqclient -> add include path
  dnl PHP_ADD_INCLUDE($ROCKETMQCLIENT_DIR/include)

  dnl # --with-rocketmqclient -> check for lib and symbol presence
  dnl LIBNAME=rocketmqclient # you may want to change this
  dnl LIBSYMBOL=rocketmqclient # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $ROCKETMQCLIENT_DIR/$PHP_LIBDIR, ROCKETMQCLIENT_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_ROCKETMQCLIENTLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong rocketmqclient lib version or lib not found])
  dnl ],[
  dnl   -L$ROCKETMQCLIENT_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  dnl PHP_SUBST(ROCKETMQCLIENT_SHARED_LIBADD)

  PHP_ADD_LIBRARY(stdc++, 1, EXTRA_LDFLAGS)
  EXTRA_LDFLAGS="-lrocketmq -lz -lcurl -lpthread"
  CPPFILE="rocketmqclient.cpp"
  PHP_NEW_EXTENSION(rocketmqclient, $CPPFILE , $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
  PHP_REQUIRE_CXX()
fi
