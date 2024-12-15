#!/usr/bin/env bash

if [[ -e "submit.zip" ]]; then
    rm submit.zip
fi

zip -r submit.zip include lib src CMakeLists.txt README.pdf
