#!/bin/bash
set -eu

python3 -c 'import sys; from pathlib import Path; print(repr(Path(sys.argv[1]).read_bytes()))'  "$1"
