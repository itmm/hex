#!/bin/bash

if [ -f index.x ]; then
	FILES="${*:-index.x} "
elif [ -f index.md ]; then
	FILES="${*:-index.md} "
else
	FILES="$* "
fi
USED=""

while [[ "x$FILES" != "x" ]]; do
	i=`expr match "${FILES}" '[^ ]* '`
	f=${FILES:0:$((i - 1))}
	FILES=${FILES#$f }
	j=`expr match "${USED}" "\(\(.* \)*$f \)"`
	if [[ "$j" = "" ]]; then
		echo "$f"
		USED="$f $USED"
		NEW=`grep '@inc(' $f | cut '-d(' -f2 | cut '-d)' -f1`
		TOADD=''
		for n in $NEW; do
			TOADD="$TOADD$n "
		done
		FILES="$TOADD$FILES"
	fi
done
