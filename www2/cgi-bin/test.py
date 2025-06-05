#!/usr/bin/env python3

import os
import sys
from datetime import datetime

# Headers CGI
print("Content-Type: text/html")
print()  # Ligne vide obligatoire

# HTML de réponse
print("""<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <title>Test CGI Python</title>
    <style>
        body {{ font-family: Arial, sans-serif; margin: 40px; }}
        .info {{ background: #f0f8ff; padding: 20px; border-radius: 8px; }}
        .env-var {{ background: #f5f5f5; padding: 10px; margin: 5px 0; border-left: 4px solid #007acc; }}
    </style>
</head>
<body>
    <h1>🐍 Script CGI Python</h1>
    <div class="info">
        <h2>Informations du serveur</h2>
        <p><strong>Heure actuelle:</strong> {}</p>
        <p><strong>Python version:</strong> {}</p>
        <p><strong>Working directory:</strong> {}</p>
    </div>
    
    <h2>Variables d'environnement CGI</h2>
""".format(datetime.now().strftime("%Y-%m-%d %H:%M:%S"), 
           sys.version, 
           os.getcwd()))

# Afficher les variables d'environnement importantes
env_vars = ['REQUEST_METHOD', 'QUERY_STRING', 'CONTENT_TYPE', 
           'CONTENT_LENGTH', 'HTTP_HOST', 'HTTP_USER_AGENT',
           'SERVER_NAME', 'SERVER_PORT', 'SCRIPT_NAME']

for var in env_vars:
    value = os.environ.get(var, 'Non définie')
    print('<div class="env-var"><strong>{}:</strong> {}</div>'.format(var, value))

# Handle POST data if present
request_method = os.environ.get('REQUEST_METHOD', '')
content_length = os.environ.get('CONTENT_LENGTH', '0')

if request_method == 'POST' and content_length and content_length != '0':
    try:
        post_data = sys.stdin.read(int(content_length))
        print("""
    <h2>📬 Données POST reçues</h2>
    <div class="env-var">
        <strong>Taille:</strong> {} bytes<br>
        <strong>Contenu:</strong><br>
        <pre>{}</pre>
    </div>""".format(content_length, post_data))
    except Exception as e:
        print("""
    <h2>❌ Erreur lecture POST</h2>
    <div class="env-var">
        <strong>Erreur:</strong> {}
    </div>""".format(str(e)))

print("""
    <h2>Toutes les variables d'environnement</h2>
    <details>
        <summary>Cliquer pour voir toutes les variables</summary>
        <pre>""")

for key, value in sorted(os.environ.items()):
    print("{}: {}".format(key, value))

print("""        </pre>
    </details>
</body>
</html>""")
