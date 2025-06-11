#!/usr/bin/env python3

import os
import sys
from datetime import datetime

# Headers CGI
print("Content-Type: text/html")
print()  # Ligne vide obligatoire

# Variables d'environnement importantes
env_vars = ['REQUEST_METHOD', 'QUERY_STRING', 'CONTENT_TYPE', 
           'CONTENT_LENGTH', 'HTTP_HOST', 'HTTP_USER_AGENT',
           'SERVER_NAME', 'SERVER_PORT', 'SCRIPT_NAME', 'PATH_INFO']

# Handle POST data if present
request_method = os.environ.get('REQUEST_METHOD', '')
content_length = os.environ.get('CONTENT_LENGTH', '0')
post_data = ""
post_error = ""

if request_method == 'POST' and content_length and content_length != '0':
    try:
        post_data = sys.stdin.read(int(content_length))
    except Exception as e:
        post_error = str(e)

# HTML de réponse
print(f"""<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>TEST.EXE - CGI TESTER</title>
    <link rel="stylesheet" href="../style.css">
    <style>
        @import url('https://fonts.googleapis.com/css2?family=JetBrains+Mono:wght@300;400;700;800&family=Space+Grotesk:wght@300;400;500;700;900&display=swap');
        
        .test-block {{
            border: 2px solid #333;
            padding: 1.5rem;
            margin: 1rem 0;
            transition: all 0.3s;
        }}
        
        .test-block:hover {{
            border-color: #00ff00;
        }}
        
        .test-title {{
            font-size: 1.1rem;
            font-weight: 700;
            margin-bottom: 1rem;
            color: #ffffff;
        }}
        
        .test-content {{
            color: #888;
            font-family: "JetBrains Mono", monospace;
            font-size: 0.9rem;
            line-height: 1.4;
        }}
        
        .env-grid {{
            display: grid;
            grid-template-columns: 1fr 2fr;
            gap: 0.5rem;
            margin: 0.5rem 0;
        }}
        
        .env-key {{
            color: #ffffff;
            font-weight: 700;
        }}
        
        .env-value {{
            color: #888;
            word-break: break-all;
        }}
        
        .post-data {{
            background: rgba(0, 255, 0, 0.1);
            border: 2px solid #333;
            padding: 1rem;
            margin: 1rem 0;
            font-family: "JetBrains Mono", monospace;
            font-size: 0.8rem;
            white-space: pre-wrap;
            word-break: break-all;
        }}
        
        .error {{
            background: rgba(255, 0, 0, 0.1);
            border: 2px solid #ff0000;
            padding: 1rem;
            margin: 1rem 0;
            color: #ff0000;
        }}
        
        .all-env {{
            border: 2px solid #333;
            padding: 1rem;
            margin: 1rem 0;
            max-height: 300px;
            overflow-y: auto;
            font-family: "JetBrains Mono", monospace;
            font-size: 0.8rem;
            color: #888;
        }}
        
        details {{
            margin: 1rem 0;
        }}
        
        summary {{
            color: #ffffff;
            font-weight: 700;
            cursor: pointer;
            padding: 0.5rem;
            border: 2px solid #333;
            transition: all 0.3s;
        }}
        
        summary:hover {{
            border-color: #00ff00;
        }}
    </style>
</head>
<body>
    <div class="container">
        <header class="header">
            <h1 class="title">TEST.EXE</h1>
            <p class="subtitle">CGI TESTER</p>
        </header>

        <section class="stats-grid">
            <div class="stat-block">
                <div class="stat-number">{request_method}</div>
                <div class="stat-label">Request Method</div>
            </div>
            <div class="stat-block">
                <div class="stat-number">{content_length}</div>
                <div class="stat-label">Content Length</div>
            </div>
            <div class="stat-block">
                <div class="stat-number">PY3</div>
                <div class="stat-label">Script Engine</div>
            </div>
            <div class="stat-block">
                <div class="stat-number">CGI</div>
                <div class="stat-label">Interface Type</div>
            </div>
        </section>

        <section class="test-block">
            <div class="test-title">SERVER INFORMATION</div>
            <div class="test-content">
                <div class="env-grid">
                    <div class="env-key">TIMESTAMP:</div>
                    <div class="env-value">{datetime.now().strftime("%Y-%m-%d %H:%M:%S")}</div>
                    <div class="env-key">PYTHON VERSION:</div>
                    <div class="env-value">{sys.version.split()[0]}</div>
                    <div class="env-key">WORKING DIRECTORY:</div>
                    <div class="env-value">{os.getcwd()}</div>
                </div>
            </div>
        </section>

        <section class="test-block">
            <div class="test-title">CGI ENVIRONMENT VARIABLES</div>
            <div class="test-content">""")

for var in env_vars:
    value = os.environ.get(var, 'NOT_DEFINED')
    # Tronquer les valeurs très longues
    display_value = value[:80] + "..." if len(str(value)) > 80 else value
    print(f"""                <div class="env-grid">
                    <div class="env-key">{var}:</div>
                    <div class="env-value">{display_value}</div>
                </div>""")

print("""            </div>
        </section>""")

# Afficher les données POST si présentes
if request_method == 'POST':
    if post_error:
        print(f"""        <section class="error">
            <div class="test-title">POST DATA ERROR</div>
            <div class="test-content">ERROR: {post_error}</div>
        </section>""")
    elif post_data:
        print(f"""        <section class="test-block">
            <div class="test-title">POST DATA RECEIVED</div>
            <div class="post-data">{post_data}</div>
        </section>""")
    else:
        print("""        <section class="test-block">
            <div class="test-title">POST REQUEST</div>
            <div class="test-content">POST method detected but no data received</div>
        </section>""")

print("""        <section class="test-block">
            <details>
                <summary>ALL ENVIRONMENT VARIABLES</summary>
                <div class="all-env">""")

for key, value in sorted(os.environ.items()):
    print(f"{key}: {value}<br>")

print(f"""                </div>
            </details>
        </section>

        <section class="nav-section">
            <div class="nav-grid">
                <a href="/cgi-bin/test.py" class="nav-button">REFRESH</a>
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
