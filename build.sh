#!/usr/bin/env bash

rm -rf libsodium && git clone --depth 1 https://github.com/jedisct1/libsodium.git --branch stable libsodium

make clean
make check
