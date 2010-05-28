#!/bin/bash

PROJECT=pidgin-extended-blist-sort
VERSION=$(cat VERSION)
REPOSITORY=ppa:konradgraefe/pidgin-plugins
DISTRIBUTIONS="karmic lucid maverick"

if [ -f DEB_REVISION ]; then
	DEB_REVISION=$(cat DEB_REVISION)
else
	DEB_REVISION=1
fi

src_dir=$(pwd)

echo -n "make dist? (y/N) "
read -n 1 in
echo ""
if [ "$in" == "y" ]; then
	make dist
fi

echo -n "Debian package revision? ($DEB_REVISION) "
read in
if [ "$in" != "" ]; then
	DEB_REVISION=$in
fi


for dist in $DISTRIBUTIONS
do
	revision=$dist$DEB_REVISION

	rm -rf deb-pkg
	mkdir deb-pkg
	cd deb-pkg

	tar xzvf ../${PROJECT}-${VERSION}.tar.gz

	cd ${PROJECT}-${VERSION}

	cp -r ${src_dir}/debian .
	sed \
		-e "s/@@VERSION@@/${VERSION}/" \
		-e "s/@@DATE@@/$(date -R)/" \
		-e "s/@@DEB_REVISION@@/${revision}/" \
		-e "s/@@DISTRIBUTION@@/${dist}/" \
		debian/changelog.in >debian/changelog

	sed \
		-e "s/@@DATE@@/$(date -R)/" \
		debian/copyright.in >debian/copyright

	dpkg-buildpackage -S -rfakeroot

	cd ..
	lintian -i *.dsc

	echo ""
	echo -n "Upload ${PROJECT}_${VERSION}-${revision} to repository (y/N) "
	read -n 1 in
	echo ""

	if [ "$in" == "y" ]; then
		dput ${REPOSITORY} ${PROJECT}_${VERSION}-${revision}_source.changes
		echo $DEB_REVISION >DEB_REVISION
	fi

	cd ${src_dir}
done
