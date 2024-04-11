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
folder1="./processes/server/src"
folder2="./processes/client/src"

# Function to perform make n times
perform_make() {
    local folder="$1"
    local times="$2"
    echo "Making in $folder $times times"
    (cd "$folder" && for _ in {1..$times}; do make; done)
}

# Perform make in folder1 integer1 times
perform_make "$folder1" "$integer1"

# Perform make in folder2 integer2 times
perform_make "$folder2" "$integer2"