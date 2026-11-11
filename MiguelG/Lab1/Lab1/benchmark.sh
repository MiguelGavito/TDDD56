#!/bin/bash

SCRIPT="./mandelbrot-256-500-375--2-0.6--1-1-3-1"
OUTPUT="result.csv"
ITERATIONS=100

echo "Run and Result" > "$OUTPUT"

for i in $(seq 1 $ITERATIONS); do
    echo "Running $i/$ITERATIONS..."

    RESULT=$($SCRIPT)

    echo "$i,$RESULT" >> "$OUTPUT"
done

echo "Finish. result saved in $OUTPUT"