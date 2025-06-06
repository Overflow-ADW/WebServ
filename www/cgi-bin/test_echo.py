#!/usr/bin/env python3
import sys
import os

print("Content-Type: text/plain\r")
print("\r")

# Lire le body depuis stdin
try:
    input_data = sys.stdin.read()
    print(f"Received data: {input_data}")
    print(f"Data length: {len(input_data)}")
except Exception as e:
    print(f"Error reading input: {e}")
