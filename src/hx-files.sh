#!/bin/bash

if [ -f index.x ]; then
	FILEs=${*:-index.x}
elif [ -f index.md ]; then
	FILEs=${*:-index.md}
else
	FILEs=$*
fi
grep '[dD]ef[(]file: ' $FILEs </dev/null | \
	cut '-d(' -f2 | \
	cut '-d)' -f1 | \
	cut '-d ' -f2 | \
	sort -u

