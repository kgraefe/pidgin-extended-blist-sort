#! /bin/sh

aclocal \
&& automake --add-missing \
&& autoconf \
&& libtoolize --copy --install \
&& (intltoolize --version) < /dev/null > /dev/null 2>&1 || {
    echo;
    echo "You must have intltool installed to compile extended-blist-sort";
    echo;
    exit;
}
