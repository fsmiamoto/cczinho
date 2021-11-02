#!/bin/sh

TEMP_ASM="$(mktemp -u --suffix=.s)"
TEMP_BIN="$(mktemp -u)"
TEMP_OBJ="$(mktemp -u)"
TEMP_OUT="$(mktemp -u)"
FAIL_COUNT=0

trap cleanup EXIT

cleanup() {
    [ -e "$TEMP_BIN" ] && rm "$TEMP_BIN"
    [ -e "$TEMP_ASM" ] && rm "$TEMP_ASM"
}

setup() {
    $CC -c ./test/functions.c -o $TEMP_OBJ
}

setup

assert() {
    expected="$1"
    input="$2"

    ./$BIN "$input" > "$TEMP_ASM"
    if [ $? -ne 0 ]; then
        echo "FAIL: $input => $expected failed to compile"
        FAIL_COUNT=$(($FAIL_COUNT + 1))
        return
    fi

    $CC -o "$TEMP_BIN" "$TEMP_ASM" "$TEMP_OBJ"
    "$TEMP_BIN" 2>&1 > "$TEMP_OUT"
    actual="$?"

    if [ "$actual" -ne "$expected" ]; then
        echo "FAIL: $input => $expected expected, but got $actual"
        echo "OUTPUT: $(cat $TEMP_OUT)"
        FAIL_COUNT=$(($FAIL_COUNT + 1))
        return
    fi

    if [ "$VERBOSE" -eq 0 ];  then
        return;
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

    assert 42 'foo=42;'
    assert 42 '_start_with_underscore=42;'
    assert 15 'with_digits_13=15;'
    assert 15 'a=5;b=15;'
    assert 5 'a=5;a=5;'

    assert 7 'return 7;';
    assert 5 'a=1; b=9; return 5*a;'
    assert 46 'a=1; b=9; return 5*b + a;'
    assert 1 'a=5; return a >= 2;' 
    assert 1 'a=5; return a < 6;'
    assert 0 'a=5; return 35 >= 53;'
    assert 1 'a=1; return a == 1;'
    assert 0 'a=1; return a == 0;'

    # if-else
    assert 5 'if(1) return 5; return 4;'
    assert 4 'if(0) return 5; return 4;'
    assert 5 'if(1) return 5; else return 4;'
    assert 4 'if(0) return 5; else return 4;'
    assert 42 'a=123; if (a < 125) return 42; else return 27;'
    assert 27 'a=256; if (a < 125) return 42; else return 27;'

    # while
    assert 3 'a = 5; while (a > 3) a=a-1; return a;'
    assert 5 'a = 5; while (0) a=a-1; return a;'

    # for
    assert 5 'a=0; for(a = 0; a < 5; a=a+1) 1; return a;'
    assert 5 'a=0; for(a = 3; a < 5; a=a+1) 1; return a;'
    assert 5 'a=0; for(a = 0; a < 5; 1) a=a+1; return a;'
    assert 6 'a=0; for(a = 6; a < 5; 1) a=a+1; return a;'
    assert 1 'a=0; for(;a == 0;) a=a+1; return a;'
    assert 5 'a=0; for(a=5;a == 2;a=a+20) 1; return a;'

    assert 2 '{ a=2; } return a;'
    assert 5 'a=5; if(1){} return a;'
    assert 8 'a=0; if(1) { a=2; a=a*4; } return a;'
    assert 4 'a=1; if(1) { b=1; b=2; b=a*4; } return b;'

    assert 5 'a=5; foo(); return a;'

    if [ "$FAIL_COUNT" -ne 0 ]; then
        exit 1;
    fi
}

main
