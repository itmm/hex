#!/bin/bash

FILEs=`hx-srcs.sh $*`
grep -lPzo '\n```\n\n' $FILEs
