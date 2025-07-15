#!/usr/bin/env python3
import os
import json
import sys

# UPLOAD_DIR = "www/webserv42/files/uploads/"
upload_dir = os.environ.get('UPLOAD_DIR', '')
upload_block = os.environ.get('UPLOAD_BLOCK', '')
upload_block = upload_block.strip('/')
absolute_upload = os.environ.get('ABSOLUTE_UPLOAD', '')
upload_path = os.path.join(upload_dir, upload_block)
upload_path = os.path.normpath(upload_path) 
print(f"UPLOAD_DIR={upload_dir}", file=sys.stderr)
print(f"UPLOAD_BLOCK={upload_block}", file=sys.stderr)
print(f"FINAL upload_path: {upload_path}", file=sys.stderr)
print(f"FINAL absolute upload: {absolute_upload}", file=sys.stderr)


print("Content-Type: application/json\n")

try:
	files = os.listdir(absolute_upload)
	files = [f for f in files 
	  if os.path.isfile(os.path.join(absolute_upload, f)) and not f.startswith('.')]
	print(json.dumps(files))
except Exception as e:
	print(json.dumps({"error": str(e)}))


# upload_dir = os.environ.get('UPLOAD_DIR', '')  # e.g. "www/webserv42/files"
# upload_block = os.environ.get('UPLOAD_BLOCK', '').lstrip('/')  # e.g. "uploads"

# if not os.path.isabs(upload_dir):
#     base_dir = os.path.abspath(os.path.join(os.getcwd(), '..'))
    
#     # Check if upload_dir is already a subdir of base_dir to avoid doubling
#     full_path_candidate = os.path.join(base_dir, upload_dir)
    
#     # If the candidate path contains base_dir twice, strip the duplicate
#     # For simplicity, if upload_dir starts with the base_dir folder name, remove it
#     base_dir_name = os.path.basename(base_dir)
#     if upload_dir.startswith(base_dir_name):
#         upload_dir = upload_dir[len(base_dir_name):].lstrip('/')

#     upload_dir = os.path.join(base_dir, upload_dir)
# upload_path = os.path.join(upload_dir, upload_block)
# upload_path = os.path.abspath(upload_path)
