#!/usr/bin/env python3

import os
import sys
from urllib.parse import quote

def generate_html_with_server_links(path, server_url):
    try:
        files = [f for f in os.listdir(path) if os.path.isfile(os.path.join(path, f))]
        html = "<html><body><h2>Files in '{}'</h2><ul>".format(path)
        for file in files:
            encoded = quote(file)
            html += f'<li><a href="{server_url}/{encoded}">{file}</a></li>'
        html += "</ul></body></html>"
        print(html)
    except FileNotFoundError:
        print(f"The folder '{path}' does not exist.")
    except Exception as e:
        print("An error occurred:", e)

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python script.py <folder_path> <server_url>")
        sys.exit(1)

    folder_path = sys.argv[1]
    server_url = sys.argv[2].rstrip("/")  # Remove trailing slash if any
    generate_html_with_server_links(folder_path, server_url)