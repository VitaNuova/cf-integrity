#!/bin/bash

cc -c ../whitelist/snippet.c
clang-3.9 -g -emit-llvm  snake.c -c -o snake.bc
opt-3.9 -load whitelist/libWhiteListPass.so -whitelist -sf snake_draw_fruit -sf dequeue -sf enqueue -sf snake_draw_board -sf snake_index_to_coordinate -sf snake_write_text -sf snake_in_bounds -sf snake_game_over < snake.bc > snake_result.bc
llc-3.9 -filetype=obj ./snake_result.bc
cc -g -rdynamic snake_result.o snippet.o -L/usr/lib/gcc/x86_64-linux-gnu/5 -lbacktrace -lncurses
