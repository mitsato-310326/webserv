#!/bin/bash
set -eu

cd -- "$(dirname -- "$0")"
valid=(fixtures/http/valid/*.http)
if [[ ! -f "${valid[0]}" ]]; then
    echo "No valid HTTP fixtures found" >&2
    exit 1
fi

build_dir=$(mktemp -d "${TMPDIR:-/tmp}/webserv-tests.XXXXXX")
trap 'rm -rf -- "$build_dir"' EXIT
mkdir "$build_dir/src"
cp Makefile "$build_dir/"
cp -R inc "$build_dir/"
cp src/*.cpp "$build_dir/src/"
make -C "$build_dir" all
"$build_dir/webserv" "${valid[@]}"
