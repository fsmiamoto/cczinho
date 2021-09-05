#!/bin/sh

TEMP_ASM="$(mktemp -u --suffix=.s)"
TEMP_BIN="$(mktemp -u)"

trap cleanup EXIT

cleanup() {
    [ -e "$TEMP_BIN" ] && rm "$TEMP_BIN"
    [ -e "$TEMP_ASM" ] && rm "$TEMP_ASM"
}

assert() {
    expected="$1"
    input="$2"

    ./$BIN "$input" > "$TEMP_ASM"
    if [ $? -ne 0 ]; then
        echo "FAIL: $input => $expected failed to compile"
        exit 1
    fi

    $CC -o "$TEMP_BIN" "$TEMP_ASM"
    "$TEMP_BIN"
    actual="$?"

    if [ "$actual" -ne "$expected" ]; then
        echo "FAIL: $input => $expected expected, but got $actual"
        exit 1
    fi

    echo "PASS: $input => $actual"
}

main() {
    assert 21 '5+20-4;'
    assert 42 '40+2;'
    assert 4 '3 + 1;'
    assert 15 '12 + 5 - 2;'

    assert 10 '5*16/8;'
    assert 47 '5+6*7;'

    assert 15 '5*(9-6);'
    assert 4 '(3+5)/2;'

    assert 10 '-10+20;'

    assert 0 '0==1;'
    assert 1 '42==42;'
    assert 1 '0!=1;'
    assert 0 '42!=42;'

    assert 1 '0<1;'
    assert 0 '1<1;'
    assert 0 '2<1;'
    assert 1 '0<=1;'
    assert 1 '1<=1;'
    assert 0 '2<=1;'

    assert 1 '1>0;'
    assert 0 '1>1;'
    assert 0 '1>2;'
    assert 1 '1>=0;'
    assert 1 '1>=1;'
    assert 0 '1>=2;'

    assert 1 'b=1;'
    assert 3 'a=b=3;'
}

main
