#!/bin/bash

# Check if correct number of arguments are provided
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <integer1> <integer2>"
    exit 1
fi

# Assigning the arguments to variables
integer1=$1

# Assuming folder names are folder1 and folder2, replace them with actual folder names
folder1="./processes/bin"

# Function to perform make n times
perform_make() {
    local folder="$1"
    local times="$2"
    echo "Making in $folder $times times"
    (
        cd "$folder" || exit 1  # Change directory and exit if it fails
        i=1
        while [ "$i" -le "$times" ]; do
            ./main &  # Run ./main in the background
            i=$((i + 1))  # Increment the counter
        done
    )
}

# Perform make in folder1 integer1 times
perform_make "$folder1" "$integer1"