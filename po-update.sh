#!/bin/bash
cd po
echo "Updating POT template..."
intltool-update -po

for f in *.po
do echo "Checking ${f%.po} language file..."
intltool-update ${f%.po}
done

