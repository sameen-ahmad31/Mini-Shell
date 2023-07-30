#!/bin/sh

INPUT=`head -n 1 $1`
echo $INPUT | valgrind --leak-check=full --log-file="valgrind.out" ./msh > /dev/null
# Get how many errors valgrind reports
NERRORS=`cat valgrind.out | grep "ERROR SUMMARY" | cut -d ' ' -f 4`
if [ $NERRORS -ne 0 ]; then
    echo "Valgrind on $1: FAILURE"
else
    echo "Valgrind on $1: SUCCESS"
fi
rm valgrind.out
