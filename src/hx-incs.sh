#!/bin/bash

FILES="index.x "
NR=1
USED=""

while [[ "x$FILES" != "x" ]]; do
	i=`expr match "${FILES}" '[^ ]* '`
	f=${FILES:0:$i}
	FILES=${FILES#$f}
	j=`expr match "${USED}" '(.* )*$f '`
	if [[ "$j" = "0" ]]; then
		echo "$NR $f"
		USED="$f $USED"
		NR=$((NR + 1))
		NEW=`grep '@inc(' $f | cut '-d(' -f2 | cut '-d)' -f1`
		TOADD=''
		for n in $NEW; do
			TOADD="$TOADD$n "
		done
		FILES="$TOADD$FILES"
	fi
done

