#!/bin/bash

cc -c ../whitelist/snippet.c
#export CXX="-W -Wall -Werror -DVERSION='1.0.1'"
clang-3.9 -g -emit-llvm snake.c -c -o snake.bc -Xclang -DVERSION=\"1.0.1\"
opt-3.9 -load whitelist/libWhiteListPass.so -whitelist -sf eat_gold -sf collide_object -sf collision -sf setup_level -sf show_score < snake.bc > snake_result.bc
llc-3.9 -filetype=obj ./snake_result.bc
cc -g -rdynamic snake_result.o snippet.o  -L/usr/lib/gcc/x86_64-linux-gnu/5 -lbacktrace
