#!/bin/bash

SERIAL_BIN="./build/char_count_serial"
SIMD_BIN="./build/char_count_simd"

SERIAL_CSV="results_serial.csv"
SIMD_CSV="results_simd.csv"

echo "size_bytes,alignment,exec_time_us,memory_kb,char_count" > "$SERIAL_CSV"
echo "size_bytes,alignment,exec_time_us,memory_kb,char_count" > "$SIMD_CSV"

ALIGNMENTS=(16 64)
SIZES=$(python3 -c 'import numpy as np; print(" ".join(map(str, np.linspace(1024, 102400, num=50, dtype=int))))')

TARGET_CHAR=";"
SEED=12345

echo "Running benchmarks..."
for ALIGN in "${ALIGNMENTS[@]}"; do
  for SIZE in $SIZES; do
    echo "Testing size=$SIZE, alignment=$ALIGN"

    SERIAL_OUT=$($SERIAL_BIN "$SIZE" "$ALIGN" "$TARGET_CHAR")
    SERIAL_TIME=$(echo "$SERIAL_OUT" | grep "Execution time" | awk '{print $3}')
    SERIAL_MEM=$(echo "$SERIAL_OUT" | grep "Memory used" | awk '{print $4}')
    SERIAL_COUNT=$(echo "$SERIAL_OUT" | grep "Character" | awk '{print $4}')
    echo "$SIZE,$ALIGN,$SERIAL_TIME,$SERIAL_MEM,$SERIAL_COUNT" >> "$SERIAL_CSV"

    SIMD_OUT=$($SIMD_BIN "$SIZE" "$ALIGN" "$TARGET_CHAR")
    SIMD_TIME=$(echo "$SIMD_OUT" | grep "Execution time" | awk '{print $3}')
    SIMD_MEM=$(echo "$SIMD_OUT" | grep "Memory used" | awk '{print $4}')
    SIMD_COUNT=$(echo "$SIMD_OUT" | grep "Character" | awk '{print $4}')
    echo "$SIZE,$ALIGN,$SIMD_TIME,$SIMD_MEM,$SIMD_COUNT" >> "$SIMD_CSV"
  done
done

echo "Benchmark completed. Results saved to:"
echo "- $SERIAL_CSV"
echo "- $SIMD_CSV"
