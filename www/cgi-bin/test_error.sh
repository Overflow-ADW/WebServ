#!/bin/bash

# Script CGI avec test d'erreur
echo "Content-Type: text/html"
echo ""

cat << 'EOF'
<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ERROR.EXE - ERROR TESTER</title>
    <link rel="stylesheet" href="../styleIndex.css">
    <style>
        @import url('https://fonts.googleapis.com/css2?family=JetBrains+Mono:wght@300;400;700;800&family=Space+Grotesk:wght@300;400;500;700;900&display=swap');
        
        .error-block {
            border: 2px solid #ff0000;
            padding: 1.5rem;
            margin: 1rem 0;
            background: rgba(255, 0, 0, 0.1);
        }
        
        .error-title {
            font-size: 1.1rem;
            font-weight: 700;
            margin-bottom: 1rem;
            color: #ff0000;
        }
        
        .error-content {
            color: #ff0000;
            font-family: "JetBrains Mono", monospace;
            font-size: 0.9rem;
            line-height: 1.4;
        }
        
        .test-block {
            border: 2px solid #333;
            padding: 1.5rem;
            margin: 1rem 0;
            transition: all 0.3s;
        }
        
        .test-block:hover {
            border-color: #00ff00;
        }
        
        .test-title {
            font-size: 1.1rem;
            font-weight: 700;
            margin-bottom: 1rem;
            color: #ffffff;
        }
        
        .test-content {
            color: #888;
            font-family: "JetBrains Mono", monospace;
            font-size: 0.9rem;
            line-height: 1.4;
        }
    </style>
</head>
<body>
    <div class="container">
        <header class="header">
            <h1 class="title">ERROR.EXE</h1>
            <p class="subtitle">ERROR TESTER</p>
        </header>

        <section class="stats-grid">
            <div class="stat-block">
                <div class="stat-number">ERR</div>
                <div class="stat-label">Test Type</div>
            </div>
            <div class="stat-block">
                <div class="stat-number">BASH</div>
                <div class="stat-label">Shell Type</div>
            </div>
            <div class="stat-block">
                <div class="stat-number">500</div>
                <div class="stat-label">Expected Code</div>
            </div>
            <div class="stat-block">
                <div class="stat-number">CGI</div>
                <div class="stat-label">Interface</div>
            </div>
        </section>

        <section class="test-block">
            <div class="test-title">ERROR TEST DESCRIPTION</div>
            <div class="test-content">
                This script demonstrates CGI error handling by executing an invalid command.
                The server should handle this gracefully and return an appropriate error response.
            </div>
        </section>

        <section class="error-block">
            <div class="error-title">EXECUTING INVALID COMMAND...</div>
            <div class="error-content">
                Command: non_existent_command_that_will_fail<br>
                Expected result: Command not found error<br>
                Status: EXECUTING...
            </div>
        </section>
EOF

# Erreur intentionnelle pour tester la gestion d'erreur du serveur
echo "        <section class=\"error-block\">"
echo "            <div class=\"error-title\">ERROR OUTPUT</div>"
echo "            <div class=\"error-content\">"
echo "                <pre>"

# Cette commande va échouer intentionnellement
non_existent_command_that_will_fail 2>&1 || echo "ERROR: Command failed as expected"

echo "                </pre>"
echo "            </div>"
echo "        </section>"

cat << 'EOF'
        <section class="nav-section">
            <div class="nav-grid">
                <a href="/cgi-bin/test_error.sh" class="nav-button">RETRY ERROR</a>
                <a href="/cgi-bin/index.py" class="nav-button">CGI SCRIPTS</a>
                <a href="/" class="nav-button">HOME</a>
            </div>
        </section>

        <footer class="footer">
            <p>WEBSERV © 2025 - ECOLE 42 PROJECT</p>
        </footer>
    </div>
</body>
</html>
EOF
