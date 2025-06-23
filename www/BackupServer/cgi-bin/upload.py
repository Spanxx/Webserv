#!/usr/bin/env python3
import os
import sys
import re

def sanitize_filename(filename):
    return os.path.basename(filename)

def get_unique_filename(directory, filename):
    base, ext = os.path.splitext(filename)
    counter = 1
    candidate = filename
    while os.path.exists(os.path.join(directory, candidate)):
        candidate = f"{base}_{counter}{ext}"
        counter += 1
    return candidate

def parse_multipart(data, boundary):
    parts = data.split(boundary)
    for part in parts:
        if b'Content-Disposition' in part and b'filename="' in part:
            header_end = part.find(b'\r\n\r\n')
            if header_end == -1:
                continue
            headers = part[:header_end].decode(errors='ignore')
            filename_match = re.search(r'filename="([^"]+)"', headers)
            filename = filename_match.group(1) if filename_match else "unnamed_file"
            file_content = part[header_end + 4:].rstrip(b'\r\n-')
            return filename, file_content
    return None, None

def main():
    try:
        content_length = int(os.environ.get('CONTENT_LENGTH', 0))
        content_type = os.environ.get('CONTENT_TYPE', '')
        upload_dir = os.environ.get('UPLOAD_DIR', '')
        upload_block = os.environ.get('UPLOAD_BLOCK', '')
        match = re.search(r'boundary=(.*)', content_type)
        if not match:
            sys.exit(2)

        boundary = b'--' + match.group(1).encode()
        raw_data = sys.stdin.buffer.read(content_length)
        filename, file_data = parse_multipart(raw_data, boundary)

        if file_data:
            os.makedirs(upload_dir, exist_ok=True)
            filename = sanitize_filename(filename)
            unique_filename = get_unique_filename(upload_dir, filename)
            full_path = os.path.join(upload_dir, unique_filename)

            with open(full_path, "wb") as f:
                f.write(file_data)

            index_path = "www/BackupServer/html/index.html"
            with open(index_path, "r", encoding="utf-8") as f:
                html_content = f.read()

            insertion = f'\n<img src="{upload_block}{unique_filename}" alt="Uploaded Image" style="max-width:100%; margin-top:20px;">\n'
            html_content = html_content.replace("</form>", "</form>" + insertion, 1)

            print("Content-Type: text/html\r\n")
            print(html_content)
        else:
            sys.exit(2)

    except Exception as e:
        sys.exit(1)

if __name__ == "__main__":
    main()
