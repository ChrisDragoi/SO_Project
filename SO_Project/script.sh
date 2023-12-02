#!/bin/bash

if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <input_file> <character>"
    exit 1
fi

input_file="$1"
character="$2"

# Verificăm dacă fișierul de intrare există
if [ ! -f "$input_file" ]; then
    echo "Error: Input file '$input_file' not found."
    exit 1
fi

sentence_count=0

while IFS= read -r sentence; do
    if echo "$sentence" | grep -Eq "^[A-Z][A-Za-z0-9 ,.!]*[A-Za-z0-9][.?!]$" && \
       ! echo "$sentence" | grep -Eq ",[ ]*si" && \
       echo "$sentence" | grep -q "$character"; then
        sentence_count=$((sentence_count + 1))
    fi
done < "$input_file"

echo "$sentence_count" > sentence_count.txt
