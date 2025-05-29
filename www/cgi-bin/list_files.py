#!/usr/bin/env python3
import os
import json

UPLOAD_DIR = "www/files/uploads/"

print("Content-Type: application/json\n")

try:
    files = os.listdir(UPLOAD_DIR)
    files = [f for f in files if os.path.isfile(os.path.join(UPLOAD_DIR, f))]
    print(json.dumps(files))
except Exception as e:
    print(json.dumps({"error": str(e)}))

