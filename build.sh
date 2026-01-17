#!/bin/sh

mkdir -p public
for file in books media _navbar.md _sidebar.md index.html README.md
do
    cp -r $file public
done
