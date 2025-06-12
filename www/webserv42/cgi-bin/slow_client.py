#!/usr/bin/env python3
import socket, time

s = socket.socket()
s.connect(("localhost", 8080))

# Send headers slowly
s.sendall(b"POST / HTTP/1.1\r\nHost: localhost\r\nContent-Length: 11\r\nContent-Type: text/html\r\n\r\n")
time.sleep(2)

# Send body in parts
s.sendall(b"hello")
time.sleep(10)
s.sendall(b"world")

# Read response
response = s.recv(4096)
print("Server responded with:\n", response.decode())

s.close()
