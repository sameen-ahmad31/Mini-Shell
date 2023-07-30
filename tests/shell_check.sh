#!/bin/sh

# get the input to the shell
INPUT=`head -n 1 $1`
echo "$INPUT" | ./msh > shell_output.tmp
# get the expected output
tail -n +2 $1 > expected_output.tmp

# see if there is a difference between the output, and expeted output
DIFF=`diff -q shell_output.tmp expected_output.tmp`

if [ -z "$DIFF" ]
then
    echo "SUCCESS on $1"
else
    echo "FAILURE on $1"
    echo "---"
    echo "msh output for input \"$INPUT\":"
    cat shell_output.tmp
    echo "---"
    echo "Expected output:"
    cat expected_output.tmp
fi

rm shell_output.tmp expected_output.tmp
