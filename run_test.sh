#!/bin/bash
count=0
for test in test_folder/*.out; do
	name=$(basename $test .out)
	arguments=test_folder/$name.c
	echo "Running test $name"
    gcc virtual_alloc.c $arguments -o virtual_alloc && ./virtual_alloc | diff - $test || echo "Test $name: failed!"
	count=$((count+1))
done
echo "Finished running $count tests!"