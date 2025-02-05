#!/bin/bash

DIR_PATH="../../bril/test/print"
BUILDPATH="../../build/cfg"

for TESTCASE in "$DIR_PATH"/*.json; do
    echo "Processing $TESTCASE..."
    "${BUILDPATH}" < "${TESTCASE}" 
done