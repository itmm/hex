#!/bin/bash

FILEs=${*:-index.x}
grep '[dD]ef[(]file: ' $FILEs </dev/null | \
	cut '-d(' -f2 | \
	cut '-d)' -f1 | \
	cut '-d ' -f2 | \
	sort -u

