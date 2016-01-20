#!/bin/bash
set +x

cd "$(dirname "$0")/.."

vim ChangeLog || exit 1
vim VERSION || exit 1

./autogen.sh || exit 1

if [[ "$(uname -o)" == "Cygwin" ]]; then
	# We set some dummy variables here so that ./configure completes so that we
	# get our new config.h.mingw.
	export pidgin_CFLAGS=xxx
	export pidgin_LIBS=xxx
fi

./configure || exit 1
cp config.h config.h.mingw
