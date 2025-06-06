#!/usr/bin/env python3
import sys

print("Content-Type: text/plain\r")
print("\r")

# Générer des données pour tester read()
for i in range(100):
    print(f"Line {i}: This is test output to verify read() error handling")

print("End of CGI output")
