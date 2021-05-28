#!/bin/sh

TEMP_ASM="$(mktemp -u --suffix=.s)"
TEMP_BIN="$(mktemp -u)"

trap cleanup EXIT

cleanup() {
    rm "$TEMP_BIN" "$TEMP_ASM"
}

assert() {
    expected="$1"
    input="$2"

    ./$BIN "$input" > "$TEMP_ASM"
    $CC -o "$TEMP_BIN" "$TEMP_ASM"
    "$TEMP_BIN"
    actual="$?"

    if [ "$actual" -ne "$expected" ]; then
        echo "$input => $expected expected, but got $actual"
        exit 1;
    fi

    echo "$input => $actual"
}

main() {
    assert 21  '5+20-4'

    echo OK
}

main
