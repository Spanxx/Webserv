#!/usr/bin/env python3

import cgi

print("Content-Type: text/html\n")

form = cgi.FieldStorage()

# Define correct answers
answers = {
    "q1": {"GET", "POST", "DELETE"},
    "q2": {"create", "connect", "bind"}
}

def check(question, correct_set):
    selected = form.getlist(question)
    return set(selected) == correct_set, selected

# Begin HTML output
print("""
<html>
<head>
  <title>Quiz Results</title>
  <style>
    body {
      font-family: 'Segoe UI', sans-serif;
      background-color: #f4f4f9;
      color: #333;
      padding: 2em;
      max-width: 800px;
      margin: auto;
    }
    h2 {
      color: #2c3e50;
      text-align: center;
      margin-bottom: 1em;
    }
    .result-card {
      background: white;
      border: 1px solid #ccc;
      border-left: 5px solid #6c63ff;
      padding: 1em 1.5em;
      margin-bottom: 1.5em;
      border-radius: 8px;
      box-shadow: 0 4px 6px rgba(0,0,0,0.05);
    }
    .correct {
      color: #27ae60;
      font-weight: bold;
    }
    .incorrect {
      color: #c0392b;
      font-weight: bold;
    }
    .score {
      font-size: 1.3em;
      text-align: center;
      background: #6c63ff;
      color: white;
      padding: 1em;
      border-radius: 8px;
      margin-top: 2em;
    }
    a.button {
      display: inline-block;
      text-decoration: none;
      background: #6c63ff;
      color: white;
      padding: 0.7em 1.2em;
      border-radius: 6px;
      margin-top: 2em;
      text-align: center;
    }
    a.button:hover {
      background: #594dff;
    }
  </style>
</head>
<body>
  <h2>Quiz Results</h2>
""")

score = 0
total = len(answers)

for i, (q, correct) in enumerate(answers.items(), start=1):
    is_correct, selected = check(q, correct)
    print('<div class="result-card">')
    print(f"<p><strong>Question {i}:</strong></p>")
    print("<p>Your answers: " + (", ".join(selected) if selected else "None") + "</p>")
    if is_correct:
        print('<p class="correct">✅ Correct!</p>')
        score += 1
    else:
        print('<p class="incorrect">❌ Incorrect.</p>')
        print(f"<p>Correct answers: {', '.join(correct)}</p>")
    print('</div>')

print(f"""
  <div class="score">
    Your Score: {score} / {total}
  </div>
  <div style="text-align:center;">
    <a href="/quiz.html" class="button">Try Again</a>
  </div>
</body>
</html>
""")
