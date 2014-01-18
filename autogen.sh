#! /bin/sh

ls VERSION >/dev/null || exit 1
ls configure.ac.in >/dev/null || exit 1
ls COPYING >/dev/null || exit 1
echo generating configure.ac || exit 1

languages=""
for f in po/*.po
do languages="$languages $(basename $f .po)"
done

sed -e "s/@@VERSION@@/$(cat VERSION)/" -e "s/@@LANGUAGES@@/$(echo $languages)/" configure.ac.in >configure.ac &&
echo aclocal || exit 1
aclocal || exit 1
echo autoheader || exit 1
autoheader || exit 1
echo libtoolize --copy || exit 1
libtoolize --copy || exit 1
echo automake --add-missing --copy || exit 1
automake --add-missing --copy || exit 1
echo autoconf || exit 1
autoconf || exit 1
echo libtoolize --copy --install || exit 1
libtoolize --copy --install || exit 1
echo intltoolize --copy --force || exit 1
intltoolize --copy --force 
