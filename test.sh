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
    assert 10 '5*16/8'
    assert 47 '5+6*7'
    assert 15 '5*(9-6)'
    assert 4 '(3+5)/2'
    assert 10 '-10+20'
}

main
