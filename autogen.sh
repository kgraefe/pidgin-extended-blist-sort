#! /bin/sh
set +x

test -f COPYING || exit 1

./scripts/gen-configure_ac.sh || exit 1

mkdir -p m4
intltoolize --force --copy --automake || exit 1
autoreconf --force --install --verbose || exit 1

