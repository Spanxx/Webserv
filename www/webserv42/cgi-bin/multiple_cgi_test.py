#!/usr/bin/env python3
import os
import time

print("Content-Type: text/plain\n")
print(f"PID: {os.getpid()}")
print(f"Time: {time.time()}")
