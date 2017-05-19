#! /bin/sh
set +x

test -f COPYING || exit 1
test -f Makefile.am.in || exit 1

./scripts/gen-configure_ac.sh || exit 1

sed \
	-e "s#@@HEADERFILES@@#$(find src -type f -name '*.h' | xargs echo)#" \
Makefile.am.in >Makefile.am || exit 1

mkdir -p m4
intltoolize --force --copy --automake || exit 1
autoreconf --force --install --verbose || exit 1

