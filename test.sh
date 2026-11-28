#!/bin/bash
set -eu

cd -- "$(dirname -- "$0")"
valid=(fixtures/http/valid/*.http)
invalid=(fixtures/http/invalid/*.http)
incomplete=(fixtures/http/incomplete/*.http)
policy=(fixtures/http/policy/*.http)
unsupported=(fixtures/http/unsupported/*.http)
streams=(fixtures/http/streams/*.http)

for fixtures in "${valid[@]}" "${invalid[@]}" "${incomplete[@]}" "${policy[@]}" "${unsupported[@]}" "${streams[@]}"; do
    if [[ ! -f "$fixtures" ]]; then
        echo "Missing HTTP fixtures under fixtures/http" >&2
        exit 1
    fi
done

build_dir=$(mktemp -d "${TMPDIR:-/tmp}/webserv-tests.XXXXXX")
trap 'rm -rf -- "$build_dir"' EXIT
mkdir -p "$build_dir/src/HttpRequestsParser" "$build_dir/src/Test"
cp Makefile "$build_dir/"
cp -R inc "$build_dir/"
cp src/*.cpp "$build_dir/src/"
cp src/HttpRequestsParser/*.cpp "$build_dir/src/HttpRequestsParser/"
cp src/Test/*.cpp "$build_dir/src/Test/"
make -C "$build_dir" all
"$build_dir/webserv" \
    "${valid[@]}" \
    "${streams[@]}" \
    "${policy[@]}" \
    "${unsupported[@]}" \
    "${invalid[@]}" \
    "${incomplete[@]}"
