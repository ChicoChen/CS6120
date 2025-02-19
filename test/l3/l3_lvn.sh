#!/bin/bash

BRENCH_PATH="../../bril/brench"
TOML_PATH="../../bril/examples/test/lvn"
BUILDPATH="../../build/lvn"

echo "testing Brench benchmarks"
pushd ${BRENCH_PATH} > /dev/null
brench brench.toml > results.csv
if awk -F ',' '$3 == "incorrect" { exit 1 }' results.csv; then
    echo "All pass"
else
    echo "The following benchmarks failed:"
    awk -F ',' '$3 == "incorrect" { print $0 }' results.csv
fi

echo -e " \ntesting example benchmarks"
popd > /dev/null
pushd ${TOML_PATH} > /dev/null
turnt *.bril

#TODO:
#   1. change implementation to pass toml-test
#   2. implement constant propagation
#   3. implement constant folding
#   4. current implementation not support bool datatype when tracking assignment (src/l3_lvn.cpp line#63)
