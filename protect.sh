#!/bin/bash

cc -c whitelist/snippet.c
clang-3.9 -emit-llvm whitelist/sample.c -c -o sample.bc
opt-3.9 -load build/whitelist/libWhiteListPass.so -whitelist -sf readKey < sample.bc > result.bc
llc-3.9 -filetype=obj ./result.bc
cc -rdynamic result.o snippet.o
