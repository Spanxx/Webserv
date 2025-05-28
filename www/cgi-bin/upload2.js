#!/usr/bin/env node
const fs = require('fs');

const length = parseInt(process.env.CONTENT_LENGTH || '0', 10);
let data = Buffer.alloc(length);

process.stdin.on('data', chunk => {
  data = Buffer.concat([data, chunk]);
});

process.stdin.on('end', () => {
  const start = data.indexOf('\r\n\r\n') + 4;
  const end = data.lastIndexOf('------');
  const fileContent = data.slice(start, end);

  fs.writeFileSync('uploaded_image.jpg', fileContent);
  process.stdout.write("Content-Type: text/html\r\n\r\n");
  process.stdout.write("<h1>Upload complete (Node.js)</h1>");
});
