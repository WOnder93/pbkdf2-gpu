#!/bin/bash
# ---------------------------------------------------------------------
# Copyright (C) 2015, Ondrej Mosnacek <omosnacek@gmail.com>
#
# This program is free software: you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation: either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
# ---------------------------------------------------------------------

BIN_DIR="$1"  # pbkdf2-gpu root build directory
DEST_DIR="$2" # destination directory for benchmark logs/data
ID="$3"       # benchmark id (will appear in log/data filenames)
ENV="$4"      # environment variables to be passed to benchmarking-tool (will be passed to the 'env' command)

if [ -z "$BIN_DIR" ]; then
    echo "ERROR: Build directory not specified!" 1>&2
    exit 1
fi

if [ -z "$DEST_DIR" ]; then
    DEST_DIR=.
    echo "NOTE: Using current directory as destination for log files..." 1>&2
fi

if [ -z "$ID" ]; then
    ID="local"
    echo "NOTE: Using '$ID' as benchmark ID..."
fi

# $ENV is empty by default

echo "Running CPU benchmarks..."
for hash in sha1 sha256; do
    for task in batch-size dk-length iterations salt-len; do
        echo "Running benchmark '$task'..."
        bash run-benchmark.sh "$DEST_DIR" "$hash" "$task" cpu "$ID" "$BIN_DIR" "$ENV"
    done
done
