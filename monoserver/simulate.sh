#!/bin/bash

# Check if correct number of arguments are provided
if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <integer1> <integer2>"
    exit 1
fi

# Assigning the arguments to variables
integer1=$1
integer2=$2

# Assuming folder names are folder1 and folder2, replace them with actual folder names
folder0="../chronos/bin"
folder1="./server/bin"
folder2="./client/bin"

perform_make() {
    local folder="$1"
    echo "Making in $folder"
    (
        cd "$folder"/../src || exit 1  # Change directory and exit if it fails
        make clean; make # Run make
    )
}

# Function to perform ./main n times
run() {
    local folder="$1"
    local times="$2"
    echo "Run in $folder $times times"
    (
        cd "$folder" || exit 1  # Change directory and exit if it fails
        i=1
        while [ "$i" -le "$times" ]; do
            ./main &  # Run ./main in the background
            i=$((i + 1))  # Increment the counter
        done
    )
}


perform_make "$folder1"

echo "\n\n"

perform_make "$folder2"

# Perform make in folder1 integer1 times
run "$folder1" "$integer1"

# Perform make in folder2 integer2 times
run "$folder2" "$integer2"

