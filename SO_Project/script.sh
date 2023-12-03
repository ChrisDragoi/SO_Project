#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <character>"
    exit 1
fi

input_content=$(cat)
character="$1"

sentence_count=0

while IFS= read -r sentence; do
    if echo "$sentence" | grep -Eq "^[A-Z][A-Za-z0-9 ,.!]*[A-Za-z0-9][.?!]$" && \
       ! echo "$sentence" | grep -Eq ",[ ]*si" && \
       echo "$sentence" | grep -q "$character"; then
        sentence_count=$((sentence_count + 1))
    fi
done <<< "$input_content"

echo "$sentence_count"
