#!/bin/bash

grep '[dD]ef[(]file: ' $* </dev/null | \
	cut '-d(' -f2 | \
	cut '-d)' -f1 | \
	cut '-d ' -f2 | \
	sort -u

