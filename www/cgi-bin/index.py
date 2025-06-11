#!/usr/bin/env python3
# -*- coding: utf-8 -*-

print("Content-Type: text/html\n")

html_content = """<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>CGI.EXE - SCRIPT EXECUTION</title>
    <link rel="stylesheet" href="../styleIndex.css">
    <style>
        @import url('https://fonts.googleapis.com/css2?family=JetBrains+Mono:wght@300;400;700;800&family=Space+Grotesk:wght@300;400;500;700;900&display=swap');
    </style>
</head>
<body>
    <div class="container">
        <header class="header">
            <h1 class="title">CGI.EXE</h1>
            <p class="subtitle">SCRIPT EXECUTION</p>
        </header>

        <section class="stats-grid">
            <div class="stat-block">
                <div class="stat-number">CGI</div>
                <div class="stat-label">Protocol Support</div>
            </div>
            <div class="stat-block">
                <div class="stat-number">PY3</div>
                <div class="stat-label">Python Scripts</div>
            </div>
            <div class="stat-block">
                <div class="stat-number">SH</div>
                <div class="stat-label">Shell Scripts</div>
            </div>
            <div class="stat-block">
                <div class="stat-number">ENV</div>
                <div class="stat-label">Environment Vars</div>
            </div>
        </section>

        <section class="features-grid">
            <div class="feature-card">
                <h3 class="feature-title">TEST.PY</h3>
                <p class="feature-description">
                    Comprehensive CGI test script with environment variables,
                    request headers and POST data analysis. Perfect for debugging
                    CGI interactions and server configurations.
                </p>
                <a href="/cgi-bin/test.py" class="nav-button">EXECUTE</a>
            </div>
            <div class="feature-card">
                <h3 class="feature-title">QUOTES.PY</h3>
                <p class="feature-description">
                    Inspirational quote generator with elegant interface.
                    Displays randomly selected motivational quotes with
                     smooth transitions.
                </p>
                <a href="/cgi-bin/quotes.py" class="nav-button">EXECUTE</a>
            </div>
        </section>

        <section class="nav-section">
            <div class="nav-grid">
                <a href="/" class="nav-button">HOME</a>
                <a href="/upload.html" class="nav-button">FILE UPLOAD</a>
                <a href="/api-status.html" class="nav-button">API STATUS</a>
            </div>
        </section>

        <footer class="footer">
            <p>CGI.EXE © 2024 - WEBSERV PROJECT</p>
        </footer>
    </div>
</body>
</html>"""

print(html_content)
