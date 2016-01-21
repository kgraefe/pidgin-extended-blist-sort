#!/bin/bash
set +x

cd "$(dirname "$0")/../po"

echo "Updating POT template..."
intltool-update --pot --gettext-package=pidgin-extended-blist-sort

