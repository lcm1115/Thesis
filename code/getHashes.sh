#!/bin/bash
# File: getHashes.sh
# Author: Liam Morris
# Description: Given a directory that contains a bunch of hash files (files
#              containing litecoin transaction hashes), determine which inputs
#              were associated with that transaction and output the results to a
#              file.

HASHDIR=$1
NUMFILES=`ls -l $HASHDIR | wc -l`
for FILE in $(ls $HASHDIR); do
    # Call blockparser to determine the hashes, feed it to the associated Python
    # script, and append output to inputs.out
    ./parser txinfo `cat $HASHDIR/$FILE | tr '\n' ' '` | python inputs.py >> inputs.out
done
