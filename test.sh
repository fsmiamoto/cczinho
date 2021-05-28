#!/bin/sh

trap cleanup EXIT

cleanup() {
    rm tmp tmp.s
}

assert() {
    expected="$1"
    input="$2"

    ./czinho "$input" > tmp.s
    clang -o tmp tmp.s
    ./tmp
    actual="$?"

    if [ "$actual" -ne "$expected" ]; then
        echo "$input => $expected expected, but got $actual"
        exit 1;
    fi

    echo "$input => $actual"
}

main() {
    assert 0 0
    assert 42 42

    echo OK
}

main
