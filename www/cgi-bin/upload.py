#!/usr/bin/env python3
import os
import sys
import re

def sanitize_filename(filename):
    # Basic sanitization: remove path separators for security
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
    print(f"Number of parts: {len(parts)}")
    for i, part in enumerate(parts):
        print(f"Part {i} starts with: {part[:100]!r}")
        if b'Content-Disposition' in part and b'filename="' in part:
            header_end = part.find(b'\r\n\r\n')
            if header_end == -1:
                print("No header/body separator found")
                continue
            headers = part[:header_end].decode(errors='ignore')
            # Extract filename from Content-Disposition
            filename_match = re.search(r'filename="([^"]+)"', headers)
            if filename_match:
                filename = filename_match.group(1)
            else:
                filename = "unnamed_file"

            file_content = part[header_end + 4:]
            file_content = file_content.rstrip(b'\r\n-')

            return filename, file_content
    return None, None

def main():
    # Print HTTP headers first!
    print("Content-Type: text/html\r\n")
    print("<html><body><pre>")  # start debug output block

    try:
        content_length = int(os.environ.get('CONTENT_LENGTH', 0))
        content_type = os.environ.get('CONTENT_TYPE', '')

        print(f"CONTENT_LENGTH: {content_length}")
        print(f"CONTENT_TYPE: {content_type}")

        match = re.search(r'boundary=(.*)', content_type)
        if not match:
            print("Missing boundary in content type")
            print("</pre></body></html>")
            return

        boundary = b'--' + match.group(1).encode()
        print(f"Boundary: {boundary}")

        raw_data = sys.stdin.buffer.read(content_length)
        print(f"Raw data (first 500 bytes): {raw_data[:500]!r}")

        filename, file_data = parse_multipart(raw_data, boundary)

        if file_data:
            try:
                upload_dir = "www/files/uploads"
                os.makedirs(upload_dir, exist_ok=True)
                filename = sanitize_filename(filename)
                unique_filename = get_unique_filename(upload_dir, filename)
                full_path = os.path.join(upload_dir, unique_filename)

                with open(full_path, "wb") as f:
                    f.write(file_data)
                index_path = "www/html/index.html"
                with open(index_path, "r", encoding="utf-8") as f:
                    html_content = f.read()
                insertion = f'\n<img src="/uploads/{unique_filename}" alt="Uploaded Image" style="max-width:100%; margin-top:20px;">\n'
                html_content = html_content.replace("</form>", "</form>" + insertion)
                print("Content-Type: text/hmtl\r\n")
                print(html_content)
		
		
            except Exception as e:
                print(f"\nFailed to save file: {e}\n")
        else:
            print("\nFailed to extract file content.\n")
    except Exception as e:
        print(f"Exception occurred: {e}")

    print("</pre></body></html>")  # close debug block and HTML

if __name__ == "__main__":
    main()
