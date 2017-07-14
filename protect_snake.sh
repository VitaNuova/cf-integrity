#!/bin/bash

cc -c ../whitelist/snippet.c
clang-3.9 -g -emit-llvm snake.c -c -o snake.bc
opt-3.9 -load whitelist/libWhiteListPass.so -whitelist -sf eat_gold -sf collide_object -sf collision -sf setup_level -sf show_score < snake.bc > snake_result.bc
llc-3.9 -filetype=obj ./snake_result.bc
cc -g -rdynamic snake_result.o snippet.o  -L/usr/lib/gcc/x86_64-linux-gnu/5 -lbacktrace
