#!/usr/bin/env python3

import os
import random
from datetime import datetime

# Headers CGI
print("Content-Type: text/html")
print()  # Ligne vide obligatoire

# Citations inspirantes
quotes = [
    ("La vie est ce qui arrive pendant que vous faites d'autres projets.", "John Lennon"),
    ("Le succès c'est d'aller d'échec en échec sans perdre son enthousiasme.", "Winston Churchill"),
    ("L'imagination est plus importante que la connaissance.", "Albert Einstein"),
    ("Soyez vous-même, tous les autres sont déjà pris.", "Oscar Wilde"),
    ("Il n'y a qu'une façon d'échouer, c'est d'abandonner avant d'avoir réussi.", "Georges Clemenceau"),
    ("Hier n'est que histoire, demain est un mystère, aujourd'hui est un cadeau.", "Eleanor Roosevelt"),
    ("Le seul moyen de faire du bon travail est d'aimer ce que vous faites.", "Steve Jobs"),
    ("La programmation est un art, le débogage est un détective.", "Anonymous"),
    ("Code is poetry in motion, bugs are just misplaced semicolons.", "Dev Wisdom"),
    ("HTTP 200 : Tout va bien. HTTP 404 : Moi dans la vie.", "Programmer Humor"),
    ("Il n'y a que 10 types de personnes : ceux qui comprennent le binaire et les autres.", "Geek Classic"),
    ("Compiler sans erreur du premier coup, c'est comme gagner au loto.", "Dev Reality")
]

# Sélectionner une citation aléatoire
quote, author = random.choice(quotes)

# Récupérer des informations de la requête
user_agent = os.environ.get('HTTP_USER_AGENT', 'Navigateur inconnu')
timestamp = datetime.now().strftime("%H:%M:%S")

print(f"""<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>QUOTES.EXE - INSPIRATION ENGINE</title>
    <link rel="stylesheet" href="../style.css">
    <style>
        @import url('https://fonts.googleapis.com/css2?family=JetBrains+Mono:wght@300;400;700;800&family=Space+Grotesk:wght@300;400;500;700;900&display=swap');
        
        .quote-block {{
            border: 2px solid #333;
            padding: 2rem;
            margin: 2rem 0;
            transition: all 0.3s;
            background: rgba(0, 255, 0, 0.05);
        }}
        
        .quote-block:hover {{
            border-color: #00ff00;
        }}
        
        .quote-text {{
            font-size: 1.5rem;
            font-weight: 400;
            margin-bottom: 1rem;
            font-family: "JetBrains Mono", monospace;
        }}
        
        .quote-author {{
            font-size: 1rem;
            color: #888;
            text-align: right;
            font-weight: 700;
        }}
        
        .session-info {{
            border: 2px solid #333;
            padding: 1rem;
            margin: 2rem 0;
            font-family: "JetBrains Mono", monospace;
            font-size: 0.9rem;
            color: #888;
        }}
    </style>
</head>
<body>
    <div class="container">
        <header class="header">
            <h1 class="title">QUOTES.EXE</h1>
            <p class="subtitle">INSPIRATION ENGINE</p>
        </header>

        <section class="stats-grid">
            <div class="stat-block">
                <div class="stat-number">{len(quotes)}</div>
                <div class="stat-label">Total Quotes</div>
            </div>
            <div class="stat-block">
                <div class="stat-number">GET</div>
                <div class="stat-label">Method</div>
            </div>
            <div class="stat-block">
                <div class="stat-number">CGI</div>
                <div class="stat-label">Generation Type</div>
            </div>
            <div class="stat-block">
                <div class="stat-number">PY3</div>
                <div class="stat-label">Runtime Engine</div>
            </div>
        </section>

        <section class="quote-block">
            <div class="quote-text">"{quote}"</div>
            <div class="quote-author">— {author}</div>
        </section>

        <section class="session-info">
            <strong>SESSION INFO:</strong><br>
            TIMESTAMP: {timestamp}<br>
            USER_AGENT: {user_agent[:60]}{"..." if len(user_agent) > 60 else ""}<br>
            QUOTE_ID: {quotes.index((quote, author)) + 1:02d}/{len(quotes):02d}
        </section>

        <section class="nav-section">
            <div class="nav-grid">
                <a href="/cgi-bin/quotes.py" class="nav-button">NEW QUOTE</a>
                <a href="/cgi-bin/index.py" class="nav-button">CGI SCRIPTS</a>
                <a href="/" class="nav-button">HOME</a>
            </div>
        </section>

        <footer class="footer">
            <p>WEBSERV © 2025 - ECOLE 42 PROJECT</p>
        </footer>
    </div>
</body>
</html>""")
