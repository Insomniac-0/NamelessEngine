#!/bin/bash

set -eu
cd "$(dirname "$0")"
if (( $# == 0 )); then
	echo "Usage: ./run.sh <target> [program arguments...]:"
	exit 1
fi

target=$1
shift

exec "./build/$target" "$@"
