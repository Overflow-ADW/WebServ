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

# HTML de réponse avec style brutalist cohérent
print(f"""<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>SYSINFO.EXE - BRUTALIST SYSTEM MONITOR</title>
    <link rel="stylesheet" href="../styleIndex.css">
    <style>
        @import url('https://fonts.googleapis.com/css2?family=JetBrains+Mono:wght@300;400;700;800&family=Space+Grotesk:wght@300;400;500;700;900&display=swap');
        
        .info-grid {{
            display: grid;
            grid-template-columns: repeat(2, 1fr);
            gap: 1rem;
            margin: 2rem 0;
        }}
        
        .info-block {{
            border: 2px solid #333;
            padding: 1.5rem;
            transition: all 0.3s;
        }}
        
        .info-block:hover {{
            border-color: #00ff00;
        }}
        
        .info-title {{
            font-size: 1.1rem;
            font-weight: 700;
            margin-bottom: 0.5rem;
            color: #ffffff;
        }}
        
        .info-value {{
            color: #888;
            font-family: "JetBrains Mono", monospace;
            font-size: 0.9rem;
        }}
        
        .env-table {{
            width: 100%;
            border-collapse: collapse;
            margin: 2rem 0;
            border: 2px solid #333;
        }}
        
        .env-table th,
        .env-table td {{
            border: 1px solid #333;
            padding: 0.75rem;
            text-align: left;
            font-family: "JetBrains Mono", monospace;
            font-size: 0.9rem;
        }}
        
        .env-table th {{
            background-color: #333;
            color: #ffffff;
            font-weight: 700;
        }}
        
        .env-table td {{
            color: #888;
        }}
        
        .env-table tr:hover {{
            background-color: rgba(0, 255, 0, 0.1);
        }}
        
        .timestamp {{
            border: 2px solid #333;
            padding: 1rem;
            margin: 2rem 0;
            text-align: center;
            font-family: "JetBrains Mono", monospace;
            color: #888;
        }}
        
        @media (max-width: 768px) {{
            .info-grid {{
                grid-template-columns: 1fr;
            }}
        }}
    </style>
</head>
<body>
    <div class="container">
        <header class="header">
            <h1 class="title">SYSINFO.EXE</h1>
            <p class="subtitle">BRUTALIST SYSTEM MONITOR</p>
        </header>

        <section class="stats-grid">
            <div class="stat-block">
                <div class="stat-number">PY3</div>
                <div class="stat-label">Python Version</div>
            </div>
            <div class="stat-block">
                <div class="stat-number">CGI</div>
                <div class="stat-label">Interface Type</div>
            </div>
            <div class="stat-block">
                <div class="stat-number">{len(cgi_vars)}</div>
                <div class="stat-label">Variables Count</div>
            </div>
            <div class="stat-block">
                <div class="stat-number">LIVE</div>
                <div class="stat-label">System Status</div>
            </div>
        </section>

        <section class="info-grid">
            <div class="info-block">
                <div class="info-title">HOSTNAME</div>
                <div class="info-value">{hostname}</div>
            </div>
            <div class="info-block">
                <div class="info-title">PYTHON VERSION</div>
                <div class="info-value">{python_version.split()[0]}</div>
            </div>
            <div class="info-block">
                <div class="info-title">SERVER ENGINE</div>
                <div class="info-value">Webserv C++98 HTTP/1.1</div>
            </div>
            <div class="info-block">
                <div class="info-title">CGI STATUS</div>
                <div class="info-value">ACTIVE & RUNNING</div>
            </div>
        </section>

        <section>
            <h2 style="color: #ffffff; font-family: 'JetBrains Mono', monospace; margin: 2rem 0 1rem 0;">CGI ENVIRONMENT VARIABLES</h2>
            <table class="env-table">
                <thead>
                    <tr>
                        <th>VARIABLE</th>
                        <th>VALUE</th>
                    </tr>
                </thead>
                <tbody>""")

for var, value in cgi_vars.items():
    # Tronquer les valeurs très longues
    display_value = value[:60] + "..." if len(str(value)) > 60 else value
    print(f"""                    <tr>
                        <td><strong>{var}</strong></td>
                        <td>{display_value}</td>
                    </tr>""")

print(f"""                </tbody>
            </table>
        </section>

        <section class="timestamp">
            TIMESTAMP: {timestamp}
        </section>

        <section class="nav-section">
            <div class="nav-grid">
                <a href="/cgi-bin/sysinfo.py" class="nav-button">REFRESH</a>
                <a href="/cgi-bin/index.py" class="nav-button">CGI SCRIPTS</a>
                <a href="/" class="nav-button">HOME</a>
            </div>
        </section>

        <footer class="footer">
            <p>SYSINFO.EXE © 2024 - WEBSERV PROJECT</p>
        </footer>
    </div>
</body>
</html>""")
