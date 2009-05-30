#!/bin/bash

if [ -z $PREFIX ]; then
        PIDGINDIR=`which pidgin`
        BINDIR=`dirname $PIDGINDIR`
        PREFIX=`dirname $BINDIR`
fi

cp extended_blist_sort.so ${PREFIX}/lib/purple-2
cp extended_blist_sort.mo ${PREFIX}/share/locale/de/LC_MESSAGES/
