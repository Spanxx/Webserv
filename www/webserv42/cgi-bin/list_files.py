#!/usr/bin/env python3
import os
import json

# UPLOAD_DIR = "www/webserv42/files/uploads/"
upload_dir = os.environ.get('UPLOAD_DIR', '')
upload_block = os.environ.get('UPLOAD_BLOCK', '')
upload_block = upload_block.lstrip('/')
upload_path = os.path.join(upload_dir, upload_block)

print("Content-Type: application/json\n")

try:
	files = os.listdir(upload_path)
	files = [f for f in files 
	  if os.path.isfile(os.path.join(upload_path, f)) and not f.startswith('.')]
	print(json.dumps(files))
except Exception as e:
	print(json.dumps({"error": str(e)}))

