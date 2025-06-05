#!/usr/bin/env python3

import os
import sys
import socket
from datetime import datetime

# Headers CGI
print("Content-Type: text/html")
print()  # Ligne vide obligatoire

# Obtenir des informations système
hostname = socket.gethostname()
python_version = sys.version
timestamp = datetime.now().strftime("%d/%m/%Y %H:%M:%S")

# Variables d'environnement CGI importantes
cgi_vars = {
    'REQUEST_METHOD': os.environ.get('REQUEST_METHOD', 'N/A'),
    'QUERY_STRING': os.environ.get('QUERY_STRING', ''),
    'CONTENT_TYPE': os.environ.get('CONTENT_TYPE', 'N/A'),
    'CONTENT_LENGTH': os.environ.get('CONTENT_LENGTH', '0'),
    'HTTP_USER_AGENT': os.environ.get('HTTP_USER_AGENT', 'N/A'),
    'HTTP_HOST': os.environ.get('HTTP_HOST', 'N/A'),
    'REMOTE_ADDR': os.environ.get('REMOTE_ADDR', 'N/A'),
    'SERVER_NAME': os.environ.get('SERVER_NAME', 'N/A'),
    'SERVER_PORT': os.environ.get('SERVER_PORT', 'N/A'),
    'SCRIPT_NAME': os.environ.get('SCRIPT_NAME', 'N/A'),
    'PATH_INFO': os.environ.get('PATH_INFO', 'N/A')
}

# HTML de réponse
print(f"""<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>🔧 System Info - Webserv CGI</title>
    <style>
        body {{
            font-family: 'Courier New', monospace;
            margin: 0;
            padding: 20px;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
            color: #333;
        }}
        .container {{
            background: rgba(255, 255, 255, 0.95);
            padding: 30px;
            border-radius: 15px;
            box-shadow: 0 15px 35px rgba(0,0,0,0.1);
            max-width: 900px;
            margin: 0 auto;
            backdrop-filter: blur(10px);
        }}
        h1 {{
            color: #667eea;
            text-align: center;
            margin-bottom: 30px;
            font-size: 2.2em;
        }}
        h2 {{
            color: #764ba2;
            border-bottom: 2px solid #667eea;
            padding-bottom: 10px;
            margin-top: 30px;
        }}
        .info-grid {{
            display: grid;
            grid-template-columns: 1fr 1fr;
            gap: 20px;
            margin: 20px 0;
        }}
        .info-box {{
            background: rgba(102, 126, 234, 0.1);
            padding: 15px;
            border-radius: 10px;
            border-left: 4px solid #667eea;
        }}
        .env-table {{
            width: 100%;
            border-collapse: collapse;
            margin: 20px 0;
            background: white;
            border-radius: 10px;
            overflow: hidden;
            box-shadow: 0 5px 15px rgba(0,0,0,0.1);
        }}
        .env-table th {{
            background: #667eea;
            color: white;
            padding: 15px;
            text-align: left;
            font-weight: bold;
        }}
        .env-table td {{
            padding: 12px 15px;
            border-bottom: 1px solid #eee;
        }}
        .env-table tr:nth-child(even) {{
            background: rgba(102, 126, 234, 0.05);
        }}
        .env-table tr:hover {{
            background: rgba(102, 126, 234, 0.1);
        }}
        .timestamp {{
            text-align: center;
            color: #666;
            font-style: italic;
            margin: 20px 0;
            padding: 15px;
            background: rgba(0,0,0,0.05);
            border-radius: 10px;
        }}
        .links {{
            text-align: center;
            margin-top: 30px;
        }}
        .links a {{
            color: #667eea;
            text-decoration: none;
            margin: 0 15px;
            font-weight: bold;
            padding: 10px 20px;
            border: 2px solid #667eea;
            border-radius: 25px;
            transition: all 0.3s ease;
            display: inline-block;
        }}
        .links a:hover {{
            background: #667eea;
            color: white;
            transform: translateY(-2px);
        }}
        .status {{
            display: inline-block;
            padding: 5px 15px;
            background: #4CAF50;
            color: white;
            border-radius: 20px;
            font-size: 0.9em;
            margin-left: 10px;
        }}
        @media (max-width: 768px) {{
            .info-grid {{
                grid-template-columns: 1fr;
            }}
            .links a {{
                display: block;
                margin: 10px 0;
            }}
        }}
    </style>
</head>
<body>
    <div class="container">
        <h1>🔧 System Information Dashboard</h1>
        
        <div class="info-grid">
            <div class="info-box">
                <strong>🖥️ Hostname:</strong><br>
                {hostname}
                <span class="status">ONLINE</span>
            </div>
            <div class="info-box">
                <strong>🐍 Python Version:</strong><br>
                {python_version.split()[0]}
            </div>
            <div class="info-box">
                <strong>📡 Server:</strong><br>
                Webserv C++98 HTTP/1.1
            </div>
            <div class="info-box">
                <strong>⚡ CGI Status:</strong><br>
                Active & Running
                <span class="status">OK</span>
            </div>
        </div>

        <h2>🌐 CGI Environment Variables</h2>
        <table class="env-table">
            <thead>
                <tr>
                    <th>Variable</th>
                    <th>Value</th>
                </tr>
            </thead>
            <tbody>""")

for var, value in cgi_vars.items():
    # Tronquer les valeurs très longues
    display_value = value[:80] + "..." if len(str(value)) > 80 else value
    print(f"""                <tr>
                    <td><strong>{var}</strong></td>
                    <td>{display_value}</td>
                </tr>""")

print(f"""            </tbody>
        </table>

        <div class="timestamp">
            🕒 Informations générées le {timestamp}
        </div>

        <div class="links">
            <a href="/">🏠 Accueil</a>
            <a href="/cgi-bin">📜 Scripts CGI</a>
            <a href="/cgi-bin/test.py">🐍 Test</a>
            <a href="/cgi-bin/quotes.py">🌟 Citations</a>
            <a href="/upload.html">📤 Upload</a>
        </div>
    </div>
</body>
</html>""")
