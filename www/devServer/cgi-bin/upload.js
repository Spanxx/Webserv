
document.getElementById('uploadForm').addEventListener('submit', async function(e) {
  e.preventDefault(); // prevent the default form submission

  const fileInput = document.getElementById('fileInput');
  const file = fileInput.files[0];
  if (!file) {
    alert("No file selected.");
    return;
  }

  const formData = new FormData();
  formData.append('myfile', file);

  const response = await fetch('/upload', {
    method: 'POST',
    body: formData
  });

  if (response.ok) {
    alert("File uploaded successfully.");
  } else {
    alert("Upload failed.");
  }
});
