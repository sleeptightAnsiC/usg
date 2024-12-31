#!/usr/bin/env sh

# runs 'cc src/*.c' with compilers defined in 'rcp/compilers.txt'

COMPILERS=$(cat rcp/compilers.txt)
skipped=
failed=

mkdir -pv ./bin/superbuild

for i in $COMPILERS; do
	if [[ $(which $i) ]]; then
		echo "superbuild: $i"
		mkdir -pv ./bin/superbuild/$i
		pushd ./bin/superbuild/$i
		$i ../../../src/*.c
		if [ $? -ne 0 ]; then
			echo "superbuild: compilation failed with $i"
			failed+=" $i"
		fi
		popd
	else
		echo "superbuild: could not find $i"
		skipped+=" $i"
	fi
	echo
done

if [[ $skipped ]]; then
	echo "superbuild: some compilers could not be found and were skipped:$skipped"
fi
if [[ $failed ]]; then
	echo "superbuild: some compilations failed with following compilers:$failed"
fi

echo "superbuild: see created binaries at bin/superbuild/"
