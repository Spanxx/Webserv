#!/usr/bin/env python3
import time

# Simulate a long-running process
time.sleep(30)

print("Content-Type: text/html")
print()
print("<html><body><h1>Hello from CGI land!</h1></body></html>")
