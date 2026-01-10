#!/bin/bash

OUTPUT="merged_project.txt"

# Clean previous output
rm -f "$OUTPUT"

echo "/* ========================================= */" >> "$OUTPUT"
echo "/*      MERGED IRC SERVER SOURCE FILE        */" >> "$OUTPUT"
echo "/* ========================================= */" >> "$OUTPUT"
echo "" >> "$OUTPUT"

# List files explicitly (RECOMMENDED FOR 42)
FILES=(
    inc/Channel.hpp
    inc/Client.hpp
    inc/Responses.hpp
    inc/Server.hpp
    inc/Utils.hpp
    srcs/Channel.cpp
    srcs/Client.cpp
    srcs/Notification.cpp
    srcs/Server.cpp
    srcs/ServerAuth.cpp
    srcs/ServerAux.cpp
    srcs/ServerCommands.cpp
    srcs/Utils.cpp
    main.cpp
)

for file in "${FILES[@]}"; do
    if [ -f "$file" ]; then
        echo "" >> "$OUTPUT"
        echo "/* ===== FILE: $file ===== */" >> "$OUTPUT"
        echo "" >> "$OUTPUT"
        cat "$file" >> "$OUTPUT"
    else
        echo "⚠️  Skipping missing file: $file"
    fi
done

echo ""
echo "✅ Project merged into $OUTPUT"
