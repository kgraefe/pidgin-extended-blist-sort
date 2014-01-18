#!/bin/bash
PROJECT=pidgin-extended-blist-sort
WIN32DIR=${PROJECT}-$(cat VERSION)-win32

make -f Makefile.mingw clean || exit 1
make -f Makefile.mingw || exit 1
mkdir -p ${WIN32DIR}/pidgin/plugins || exit 1
sed 's/$/\r/' ChangeLog >${WIN32DIR}/ChangeLog.txt || exit 1
sed 's/$/\r/' README.win32 >${WIN32DIR}/ReadMe.txt || exit 1
cp src/${PROJECT}.dll ${WIN32DIR}/pidgin/plugins || exit 1
i586-mingw32msvc-strip --strip-unneeded ${WIN32DIR}/pidgin/plugins/${PROJECT}.dll || exit 1
for f in po/*.po; do
	lang=$(basename $f .po)
	mkdir -p ${WIN32DIR}/pidgin/locale/${lang}/LC_MESSAGES || exit 1
	cp po/${lang}.gmo ${WIN32DIR}/pidgin/locale/${lang}/LC_MESSAGES/${PROJECT}.mo || exit 1
done || exit 1
rm -f ${WIN32DIR}.zip || exit 1
cd ${WIN32DIR} || exit 1
zip -r ../${WIN32DIR}.zip * || exit 1
cd .. || exit 1
rm -rf ${WIN32DIR}
