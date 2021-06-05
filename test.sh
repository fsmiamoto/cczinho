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
        echo "FAIL: $input => $expected expected, but got $actual"
        exit 1;
    fi

    echo "PASS: $input => $actual"
}

main() {
    assert 21 '5+20-4'
    assert 42 '40+2'
    assert 4 '3 + 1'
    assert 15 '12 + 5 - 2'
}

main
