#!/bin/bash

cc -c ../whitelist/snippet.c
clang-3.9 -g -emit-llvm zopfli/zopfli/zopfli_bin.c -c -o zopfli.bc
opt-3.9 -load whitelist/libWhiteListPass.so -whitelist -sf CompressFile -sf LoadFile -sf SaveFile -sf AddStrings < zopfli.bc > zopfli_result.bc
llc-3.9 -filetype=obj ./zopfli_result.bc
cc -c zopfli/zopfli/blocksplitter.c zopfli/zopfli/cache.c\
                zopfli/zopfli/deflate.c zopfli/zopfli/gzip_container.c\
                zopfli/zopfli/hash.c zopfli/zopfli/katajainen.c\
                zopfli/zopfli/lz77.c zopfli/zopfli/squeeze.c\
                zopfli/zopfli/tree.c zopfli/zopfli/util.c\
                zopfli/zopfli/zlib_container.c zopfli/zopfli/zopfli_lib.c

cc -g -rdynamic blocksplitter.o cache.o\
             deflate.o gzip_container.o\
             hash.o katajainen.o\
             lz77.o squeeze.o\
             tree.o util.o\
             zlib_container.o zopfli_lib.o\
             zopfli_result.o snippet.o -lm \
             -L/usr/lib/gcc/x86_64-linux-gnu/5 -lbacktrace
