#!/bin/bash

# Script de test CGI basique
echo "Content-Type: text/html"
echo ""
cat << 'EOF'
<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>SHELL.EXE - BASH TESTER</title>
    <link rel="stylesheet" href="../styleIndex.css">
    <style>
        @import url('https://fonts.googleapis.com/css2?family=JetBrains+Mono:wght@300;400;700;800&family=Space+Grotesk:wght@300;400;500;700;900&display=swap');
        
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
        
        .env-item {
            margin: 0.5rem 0;
            padding: 0.5rem;
            border-left: 2px solid #333;
        }
        
        .post-data {
            background: rgba(0, 255, 0, 0.1);
            border: 2px solid #333;
            padding: 1rem;
            margin: 1rem 0;
            font-family: "JetBrains Mono", monospace;
            font-size: 0.8rem;
            white-space: pre-wrap;
        }
    </style>
</head>
<body>
    <div class="container">
        <header class="header">
            <h1 class="title">SHELL.EXE</h1>
            <p class="subtitle">BASH TESTER</p>
        </header>

        <section class="stats-grid">
            <div class="stat-block">
                <div class="stat-number">BASH</div>
                <div class="stat-label">Shell Type</div>
            </div>
            <div class="stat-block">
                <div class="stat-number">CGI</div>
                <div class="stat-label">Interface</div>
            </div>
            <div class="stat-block">
EOF

echo "            <div class=\"stat-number\">$REQUEST_METHOD</div>"
echo "            <div class=\"stat-label\">HTTP Method</div>"

cat << 'EOF'
            </div>
            <div class="stat-block">
                <div class="stat-number">LIVE</div>
                <div class="stat-label">Status</div>
            </div>
        </section>

        <section class="test-block">
            <div class="test-title">ENVIRONMENT VARIABLES</div>
            <div class="test-content">
EOF

echo "                <div class=\"env-item\">REQUEST_METHOD: $REQUEST_METHOD</div>"
echo "                <div class=\"env-item\">REQUEST_URI: $REQUEST_URI</div>"
echo "                <div class=\"env-item\">QUERY_STRING: $QUERY_STRING</div>"
echo "                <div class=\"env-item\">CONTENT_TYPE: $CONTENT_TYPE</div>"
echo "                <div class=\"env-item\">CONTENT_LENGTH: $CONTENT_LENGTH</div>"
echo "                <div class=\"env-item\">SERVER_SOFTWARE: $SERVER_SOFTWARE</div>"

cat << 'EOF'
            </div>
        </section>
EOF

if [ "$REQUEST_METHOD" = "POST" ]; then
    echo "        <section class=\"test-block\">"
    echo "            <div class=\"test-title\">POST DATA</div>"
    echo "            <div class=\"post-data\">"
    cat
    echo "            </div>"
    echo "        </section>"
fi

cat << 'EOF'
        <section class="test-block">
            <div class="test-title">SYSTEM INFORMATION</div>
            <div class="test-content">
EOF

echo "                <div class=\"env-item\">WORKING DIRECTORY: $(pwd)</div>"
echo "                <div class=\"env-item\">TIMESTAMP: $(date)</div>"
echo "                <div class=\"env-item\">HOSTNAME: $(hostname)</div>"

cat << 'EOF'
            </div>
        </section>

        <section class="nav-section">
            <div class="nav-grid">
                <a href="/cgi-bin/test_basic.sh" class="nav-button">REFRESH</a>
                <a href="/cgi-bin/index.py" class="nav-button">CGI SCRIPTS</a>
                <a href="/" class="nav-button">HOME</a>
            </div>
        </section>

        <footer class="footer">
            <p>SHELL.EXE © 2024 - WEBSERV PROJECT</p>
        </footer>
    </div>
</body>
</html>
EOF
