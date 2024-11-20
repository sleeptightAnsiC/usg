#!/usr/bin/env sh

# CHECKS source tree with include-what-you-use tool
# WARN: Make sure to call this script from the project root directory!

set -euo pipefail

CFLAGS="@./rcp/flags_clang.txt"
SRC="./src"
CC=include-what-you-use

which $CC > /dev/null || ( echo "$CC is required in order to run this script!" && exit 1 )

for i in $(ls $SRC | grep '.c'); do
	$CC $SRC/$i $CFLAGS
done

