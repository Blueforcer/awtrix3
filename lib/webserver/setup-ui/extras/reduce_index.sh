#/bin/sh

# Processing script to optionally reduce filesystem use by miniying, gzipping and preparing index.htm for embedding in code.
# Please see readme.md for more information.

# Requires xdd which is part of the VIM package
# Requires npm
#   sudo apt install npm
#   ln -s /usr/bin/nodejs /usr/bin/node
# Requires html-minifier
#   sudo npm install html-minifier -g

html-minifier \
 --case-sensitive \
 --collapse-boolean-attributes \
 --collapse-whitespace \
 --minify-css true \
 --minify-js true \
 --process-conditional-comments \
 --remove-attribute-quotes \
 --remove-comments \
 --remove-empty-attributes \
 --remove-optional-tags \
 --remove-redundant-attributes \
 --remove-script-type-attributes \
 --remove-style-link-type-attributes \
 -o edit.htm edit.htm

if [ $? -ne 0 ]
then
  echo "Error minifying index.htm"
  exit -1
fi

if [ -e edit.htm.gz ]
then
  rm edit.htm.gz
fi

gzip edit.htm
if [ $? -ne 0 ]
then
  echo "Error gzipping minified index.htm"
  exit -1
fi

echo '// WARNING: Auto-generated file. Please do not modify by hand.' > edit_htm.h
echo '// This file is an embeddable version of the gzipped index.htm file.' >> edit_htm.h
echo '// To update it, please rerun the `reduce_index.sh` script located in the `extras` subfolder' >> edit_htm.h
echo '// then recompile the sketch after each change to the `index.html` file.' >> edit_htm.h
xxd -i edit.htm.gz >> edit_htm.h
if [ $? -ne 0 ]
then
  echo "Error creating include file from index.htm.gz"
  exit -1
fi

echo Reduce complete.

