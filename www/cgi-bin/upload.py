#!/usr/bin/env python3
import os
import sys
import re

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
            file_content = part[header_end + 4:]
            file_content = file_content.rstrip(b'\r\n-')
            return file_content
    return None

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

        file_data = parse_multipart(raw_data, boundary)

        if file_data:
            try:
                with open("uploaded_image.jpg", "wb") as f:
                    f.write(file_data)
                print("\nFile uploaded successfully!\n")
            except Exception as e:
                print(f"\nFailed to save file: {e}\n")
        else:
            print("\nFailed to extract file content.\n")
    except Exception as e:
        print(f"Exception occurred: {e}")

    print("</pre></body></html>")  # close debug block and HTML

if __name__ == "__main__":
    main()
