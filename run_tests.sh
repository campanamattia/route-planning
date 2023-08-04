#!/bin/bash

# Store the start time
start_time=$(date +%s)

# Iterate from 1 to 100
for ((X=1; X<=111; X++)); do
    # Run the commands with the current value of X
    ./a.out < "test/open_${X}.txt" > out/last.txt

    # Suppress the output of the diff command
    diff out/last.txt "test/open_${X}.output.txt" > /dev/null

    # Check the exit code of the diff command to see if there are any differences
    if [ $? -eq 0 ]; then
        echo "Test $X: No differences found"
    else
        echo "Test $X: Differences found"
    fi
done

# Calculate the overall execution time
end_time=$(date +%s)
execution_time=$((end_time - start_time))

# Display the overall execution time
echo "Overall execution time: ${execution_time} seconds"
