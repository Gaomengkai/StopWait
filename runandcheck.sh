#!/bin/env bash
# author: Merky
# date: 2022-09-22

# Description: This script will run ./bin/gbn for 100 times and check if the output is correct.

# Usage: ./runandcheck.sh [gbn|stopwait|sr]

[ ! $1 ] && echo "Usage: ./runandcheck.sh [gbn|stopwait|sr]" && exit 1

inputfile="input2.txt"
outputfile="output.txt"
binfile=$1
SHELL_FOLDER=$(cd "$(dirname "$0")";pwd)
inputfile=$SHELL_FOLDER/$inputfile
outputfile=$SHELL_FOLDER/$outputfile

for i in {1..10}
do
    # clear $inputfile
    > $inputfile
    for j in {1..104}
    do
        echo `cat /proc/sys/kernel/random/uuid | md5sum |head -c 20` >> $inputfile
    done
    echo "EOF" >>$inputfile
    $SHELL_FOLDER/bin/$binfile -v -i $inputfile -o $outputfile>log.log
    if diff -q $inputfile $outputfile
    then
        echo -e "\e[32mTest $i: Passed\e[0m"
    else
        echo -e "\e[33mTest $i: Failed\e[0m"
        exit
    fi
done