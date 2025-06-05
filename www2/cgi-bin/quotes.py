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

# Couleurs aléatoires pour le thème
colors = [
    ("#FF6B6B", "#4ECDC4"),  # Rouge corail et turquoise
    ("#A8E6CF", "#FF8B94"),  # Vert menthe et rose
    ("#95E1D3", "#F3D250"),  # Bleu menthe et jaune
    ("#C7CEEA", "#FFB3BA"),  # Lavande et rose pâle
    ("#B4E7CE", "#FFC8DD"),  # Vert clair et rose bonbon
]

primary_color, secondary_color = random.choice(colors)

# Emojis aléatoires
emojis = ["🌟", "💡", "🚀", "⭐", "✨", "🎯", "🎪", "🎨", "🎭", "🎲"]
emoji = random.choice(emojis)

# Récupérer des informations de la requête
user_agent = os.environ.get('HTTP_USER_AGENT', 'Navigateur inconnu')
timestamp = datetime.now().strftime("%H:%M:%S")

# HTML de réponse
print(f"""<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>{emoji} Citation Inspirante - Webserv CGI</title>
    <style>
        body {{
            font-family: 'Georgia', serif;
            margin: 0;
            padding: 20px;
            background: linear-gradient(135deg, {primary_color} 0%, {secondary_color} 100%);
            min-height: 100vh;
            display: flex;
            align-items: center;
            justify-content: center;
        }}
        .container {{
            background: rgba(255, 255, 255, 0.95);
            padding: 40px;
            border-radius: 20px;
            box-shadow: 0 20px 40px rgba(0,0,0,0.1);
            text-align: center;
            max-width: 600px;
            backdrop-filter: blur(10px);
        }}
        .emoji {{
            font-size: 3em;
            margin-bottom: 20px;
        }}
        h1 {{
            color: #333;
            margin-bottom: 30px;
            font-size: 2em;
        }}
        .quote {{
            font-size: 1.4em;
            font-style: italic;
            color: #444;
            line-height: 1.6;
            margin: 30px 0;
            padding: 20px;
            background: rgba(255, 255, 255, 0.7);
            border-radius: 15px;
            border-left: 5px solid {primary_color};
        }}
        .author {{
            font-size: 1.1em;
            color: #666;
            font-weight: bold;
            margin-top: 20px;
        }}
        .refresh-btn {{
            background: {primary_color};
            color: white;
            padding: 15px 30px;
            border: none;
            border-radius: 25px;
            font-size: 1.1em;
            cursor: pointer;
            margin: 20px 10px;
            text-decoration: none;
            display: inline-block;
            transition: all 0.3s ease;
        }}
        .refresh-btn:hover {{
            background: {secondary_color};
            transform: translateY(-2px);
            box-shadow: 0 5px 15px rgba(0,0,0,0.2);
        }}
        .info {{
            background: rgba(0,0,0,0.05);
            padding: 15px;
            border-radius: 10px;
            margin: 20px 0;
            font-size: 0.9em;
            color: #666;
        }}
        .links {{
            margin-top: 30px;
        }}
        .links a {{
            color: {primary_color};
            text-decoration: none;
            margin: 0 15px;
            font-weight: bold;
        }}
        .links a:hover {{
            color: {secondary_color};
            text-decoration: underline;
        }}
    </style>
</head>
<body>
    <div class="container">
        <div class="emoji">{emoji}</div>
        <h1>Citation Inspirante du Moment</h1>
        
        <div class="quote">
            "{quote}"
        </div>
        
        <div class="author">
            — {author}
        </div>
        
        <a href="/cgi-bin/quotes.py" class="refresh-btn">
            🔄 Nouvelle Citation
        </a>
        
        <div class="info">
            <strong>📊 Informations de la session</strong><br>
            🕒 Généré à : {timestamp}<br>
            🌐 Navigateur : {user_agent[:50]}{"..." if len(user_agent) > 50 else ""}<br>
            🎲 Citation {quotes.index((quote, author)) + 1} sur {len(quotes)}
        </div>
        
        <div class="links">
            <a href="/">🏠 Accueil</a>
            <a href="/cgi-bin">📜 Scripts CGI</a>
            <a href="/cgi-bin/test.py">🐍 Test Python</a>
            <a href="/upload.html">📤 Upload</a>
        </div>
    </div>
</body>
</html>""")
