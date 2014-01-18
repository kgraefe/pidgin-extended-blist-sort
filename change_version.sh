#!/bin/bash

vim ChangeLog || exit 1
vim VERSION || exit 1
sed "s/@@VERSION@@/$(cat VERSION)/" configure.ac.in >configure.ac || exit 1
./autogen.sh || exit 1
./configure || exit 1
cp config.h config.h.mingw
