#!/usr/bin/env python3
# -*- coding: utf-8 -*-

print("Content-Type: text/html\n")

html_content = """<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>CGI.EXE - PYTHON EXECUTION ENVIRONMENT</title>
    <style>
        @import url('https://fonts.googleapis.com/css2?family=JetBrains+Mono:wght@300;400;700;800&family=Space+Grotesk:wght@300;400;500;700;900&display=swap');
        
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        
        :root {
            --neon-green: #39ff14;
            --neon-purple: #bf00ff;
            --neon-orange: #ff6600;
            --neon-blue: #00ccff;
            --dark-bg: #0d0d0d;
            --darker-bg: #050505;
            --text-primary: #ffffff;
            --text-secondary: #cccccc;
        }
        
        body {
            font-family: 'Space Grotesk', sans-serif;
            background: var(--dark-bg);
            color: var(--text-primary);
            min-height: 100vh;
            position: relative;
            overflow-x: hidden;
        }
        
        /* Matrix-style background */
        body::before {
            content: '';
            position: fixed;
            top: 0;
            left: 0;
            width: 100%;
            height: 100%;
            background: 
                linear-gradient(90deg, transparent 24%, rgba(57, 255, 20, 0.03) 25%, rgba(57, 255, 20, 0.03) 26%, transparent 27%, transparent 74%, rgba(57, 255, 20, 0.03) 75%, rgba(57, 255, 20, 0.03) 76%, transparent 77%, transparent),
                linear-gradient(0deg, transparent 24%, rgba(57, 255, 20, 0.03) 25%, rgba(57, 255, 20, 0.03) 26%, transparent 27%, transparent 74%, rgba(57, 255, 20, 0.03) 75%, rgba(57, 255, 20, 0.03) 76%, transparent 77%, transparent);
            background-size: 20px 20px;
            pointer-events: none;
            z-index: -1;
        }
        
        .container {
            max-width: 1200px;
            margin: 0 auto;
            padding: 2rem;
            position: relative;
            z-index: 1;
        }
        
        .header {
            text-align: center;
            margin-bottom: 3rem;
            position: relative;
        }
        
        .title {
            font-family: 'JetBrains Mono', monospace;
            font-size: clamp(2.5rem, 6vw, 5rem);
            font-weight: 900;
            text-transform: uppercase;
            background: linear-gradient(45deg, var(--neon-green), var(--neon-blue), var(--neon-purple));
            background-size: 300% 300%;
            -webkit-background-clip: text;
            -webkit-text-fill-color: transparent;
            background-clip: text;
            animation: gradientShift 4s ease-in-out infinite;
            letter-spacing: 3px;
            margin-bottom: 1rem;
        }
        
        @keyframes gradientShift {
            0%, 100% { background-position: 0% 50%; }
            50% { background-position: 100% 50%; }
        }
        
        .subtitle {
            font-family: 'JetBrains Mono', monospace;
            color: var(--neon-orange);
            font-size: 1.2rem;
            text-transform: uppercase;
            letter-spacing: 2px;
            text-shadow: 0 0 10px rgba(255, 102, 0, 0.5);
        }
        
        .script-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(350px, 1fr));
            gap: 2rem;
            margin: 3rem 0;
        }
        
        .script-card {
            background: rgba(0, 0, 0, 0.8);
            border: 3px solid var(--neon-blue);
            padding: 2rem;
            position: relative;
            transition: all 0.3s ease;
            overflow: hidden;
        }
        
        .script-card::after {
            content: '';
            position: absolute;
            top: 0;
            left: 0;
            right: 0;
            bottom: 0;
            background: 
                repeating-linear-gradient(
                    45deg,
                    transparent,
                    transparent 2px,
                    rgba(0, 204, 255, 0.1) 2px,
                    rgba(0, 204, 255, 0.1) 4px
                );
            pointer-events: none;
        }
        
        .script-card:hover {
            border-color: var(--neon-orange);
            box-shadow: 0 0 30px rgba(255, 102, 0, 0.2);
            transform: scale(1.02);
        }
        
        .script-title {
            font-family: 'JetBrains Mono', monospace;
            font-size: 1.3rem;
            color: var(--neon-blue);
            text-transform: uppercase;
            margin-bottom: 1rem;
            letter-spacing: 1px;
            position: relative;
            z-index: 2;
        }
        
        .script-description {
            color: var(--text-secondary);
            line-height: 1.6;
            margin-bottom: 1.5rem;
            position: relative;
            z-index: 2;
        }
        
        .execute-button {
            background: linear-gradient(45deg, var(--neon-green), var(--neon-blue));
            color: var(--dark-bg);
            border: none;
            padding: 1rem 2rem;
            font-family: 'JetBrains Mono', monospace;
            font-weight: 700;
            font-size: 1rem;
            text-transform: uppercase;
            text-decoration: none;
            display: inline-block;
            letter-spacing: 1px;
            transition: all 0.3s ease;
            position: relative;
            z-index: 2;
            overflow: hidden;
        }
        
        .execute-button:hover {
            background: linear-gradient(45deg, var(--neon-orange), var(--neon-purple));
            transform: translateY(-2px);
            box-shadow: 0 5px 15px rgba(57, 255, 20, 0.3);
            color: var(--text-primary);
        }
        
        .nav-section {
            text-align: center;
            margin: 4rem 0;
        }
        
        .nav-link {
            font-family: 'JetBrains Mono', monospace;
            color: var(--text-secondary);
            text-decoration: none;
            padding: 0.8rem 1.5rem;
            border: 2px solid var(--text-secondary);
            text-transform: uppercase;
            font-size: 0.9rem;
            letter-spacing: 1px;
            transition: all 0.3s ease;
            margin: 0 1rem;
        }
        
        .nav-link:hover {
            color: var(--neon-green);
            border-color: var(--neon-green);
            box-shadow: 0 0 15px rgba(57, 255, 20, 0.3);
        }
        
        @media (max-width: 768px) {
            .script-grid {
                grid-template-columns: 1fr;
            }
            
            .title {
                font-size: 3rem;
            }
        }
    </style>
</head>
<body>
    <div class="container">
        <header class="header">
            <h1 class="title">CGI.EXE</h1>
            <p class="subtitle">PYTHON EXECUTION ENVIRONMENT</p>
        </header>
        
        <section class="script-grid">
            <div class="script-card">
                <h3 class="script-title">🐍 test.py</h3>
                <p class="script-description">
                    Script de test CGI complet avec affichage des variables d'environnement, 
                    gestion des méthodes GET/POST et tests de fonctionnalité.
                </p>
                <a href="/cgi-bin/test.py" class="execute-button">EXECUTE</a>
            </div>
            
            <div class="script-card">
                <h3 class="script-title">🌟 quotes.py</h3>
                <p class="script-description">
                    Générateur de citations inspirantes avec interface élégante. 
                    Affiche des citations motivantes aléatoirement sélectionnées.
                </p>
                <a href="/cgi-bin/quotes.py" class="execute-button">EXECUTE</a>
            </div>
            
            <div class="script-card">
                <h3 class="script-title">🔧 sysinfo.py</h3>
                <p class="script-description">
                    Dashboard d'informations système complet. Affiche les données sur 
                    le serveur, l'OS, la mémoire et les processus en temps réel.
                </p>
                <a href="/cgi-bin/sysinfo.py" class="execute-button">EXECUTE</a>
            </div>
        </section>
        
        <section class="nav-section">
            <a href="/" class="nav-link">🏠 HOME</a>
            <a href="/upload.html" class="nav-link">📤 UPLOAD</a>
        </section>
    </div>
</body>
</html>"""

print(html_content)
