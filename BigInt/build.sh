#!/usr/bin/env bash

build_type=$1
if [ -z "$build_type" ]; then
    build_path="build"
else
    build_path="build-${build_type}"
fi

cmake_defs="-DCMAKE_BUILD_TYPE=${build_type}"
build_path="build-${build_type}"

mkdir -p "${build_path}"
cd "${build_path}" || return
cmake "${cmake_defs}" ..

make -j 16
cd ..
