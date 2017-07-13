#!/bin/bash

cc -c snake/snake.c
clang-3.9 -emit-llvm snake/snake.c -c -o snake.bc
opt-3.9 -load whitelist/libWhiteListPass.so -whitelist -sf eat_gold -sf collide_object -sf collision -sf setup_level -sf show_score < snake.bc > snake_result.bc
llc-3.9 -filetype=obj ./snake_result.bc
cc -rdynamic snake_result.o snippet.o
