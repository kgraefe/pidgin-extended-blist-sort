#!/bin/bash
make clean && \
make  && \
PROJECT=extended_blist_sort && \
DIR=${PROJECT}-`cat VERSION`-linux-x86 && \
mkdir -p ${DIR} && \
cp src/.libs/${PROJECT}.so ${DIR} && \
cp po/de.gmo ${DIR}/${PROJECT}.mo && \
cp install_linux_package.sh ${DIR}/install.sh && \
tar czvf ${DIR}.tar.gz ${DIR} && \
rm -rf ${DIR}
