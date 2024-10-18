#!/usr/bin/bash
function check_existing_dirs() {
    if [[ ! -d "lib/" || ! -d "include/" ]]; then
        echo "false"
        exit 0
    fi

    echo "true"
}

function setup_raylib() {
    mkdir include lib
    git clone --depth=1 "https://github.com/raysan5/raylib.git"
    cd raylib/src/
    # Modify config to allow all filetypes
    sed 's/\/\/#define SUPPORT_FILEFORMAT/#define SUPPORT_FILEFORMAT/' config.h > tmp.h; mv tmp.h config.h
    make
    cd ../../
    cp raylib/src/libraylib.a lib/
    cp raylib/src/raylib.h include/
    yes | rm -r raylib/
}

CC=gcc
SRC=game.c
LDLIBS='-L./lib/ -lraylib -lm -ldl -lpthread -lGL'
CFLAGS='-Wall'
INCLUDE='-I./include/'
OUT=game

existing=$(check_existing_dirs)
if [[ $existing = "false" ]]; then
    setup_raylib
fi

$CC $SRC $LDLIBS $CFLAGS $INCLUDE -o $OUT
