#!/bin/bash

port=$1
char_to_wait="U"

stty -F $port 115200

while true; do
    line=$(head -n 1 "$port")
    echo "Command received: $line"
    sleep 0.1

    if [[ "$line" == *"$char_to_wait"* ]]; then
        response=$(shuf -i 0-100 -n 1)
        echo "$response" > "$port"
        echo "Data sent: $response"
    else
        response=$(shuf -i 0-9999 -n 1)
        response_with_decimal=$(echo "scale=3; $response / 1000" | bc | awk '{printf "%.3f", $1}')
        echo "$response_with_decimal" > "$port"
        echo "Data sent: $response_with_decimal"
    fi
done
