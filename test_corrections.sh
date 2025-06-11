#!/bin/bash
# filepath: /home/fvastena/Desktop/webserv2/setup_basic_tests.sh

echo "🔧 Configuration des tests de base..."

# Créer les répertoires
mkdir -p www/files www/test uploads

# Créer la page principale
cat > www/index.html << 'EOF'
<!DOCTYPE html>
<html>
<head>
    <title>Webserv - Tests de Base</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 40px; background: #f5f5f5; }
        .test { background: white; padding: 20px; margin: 15px 0; border-radius: 8px; box-shadow: 0 2px 5px rgba(0,0,0,0.1); }
        .method { font-weight: bold; padding: 5px 10px; border-radius: 3px; color: white; }
        .get { background: #28a745; }
        .post { background: #007bff; }
        .delete { background: #dc3545; }
        .unknown { background: #6c757d; }
        code { background: #f8f9fa; padding: 2px 5px; border-radius: 3px; font-family: monospace; }
    </style>
</head>
<body>
    <h1>🚀 Webserv - Tests de Base HTTP</h1>
    
    <div class="test">
        <h2><span class="method get">GET</span> Test de requête GET</h2>
        <p><code>curl -X GET http://localhost:8080/</code></p>
        <p>✅ Statut attendu: <strong>200 OK</strong></p>
    </div>
    
    <div class="test">
        <h2><span class="method post">POST</span> Test de requête POST</h2>
        <p><code>curl -X POST -H "Content-Type: text/plain" --data "Test data" http://localhost:8080/upload</code></p>
        <p>✅ Statut attendu: <strong>200 OK</strong> (si upload réussi)</p>
    </div>
    
    <div class="test">
        <h2><span class="method delete">DELETE</span> Test de requête DELETE</h2>
        <p><code>curl -X DELETE http://localhost:8080/files/deleteme.txt</code></p>
        <p>✅ Statut attendu: <strong>200 OK</strong> (si fichier existe) ou <strong>404 Not Found</strong></p>
    </div>
    
    <div class="test">
        <h2><span class="method unknown">UNKNOWN</span> Test de méthode inconnue</h2>
        <p><code>curl -X UNKNOWN http://localhost:8080/</code></p>
        <p>✅ Statut attendu: <strong>405 Method Not Allowed</strong></p>
    </div>
    
    <div class="test">
        <h2>📤 Test d'Upload/Download</h2>
        <p>1. Upload: <code>curl -X POST -F "file=@test.txt" http://localhost:8080/upload</code></p>
        <p>2. Download: <code>curl http://localhost:8080/uploads/test.txt</code></p>
    </div>
    
    <hr>
    <p><em>Webserv/1.0 - École 42</em></p>
</body>
</html>
EOF

# Créer une page 404 personnalisée
cat > www/404.html << 'EOF'
<!DOCTYPE html>
<html>
<head><title>404 - Page Non Trouvée</title></head>
<body style="font-family: Arial; text-align: center; padding: 100px;">
    <h1 style="color: #e74c3c;">404 - Page Non Trouvée</h1>
    <p>La ressource demandée n'existe pas sur ce serveur.</p>
    <a href="/">← Retour à l'accueil</a>
</body>
</html>
EOF

# Créer des fichiers de test pour DELETE
echo "Ce fichier peut être supprimé avec DELETE" > www/files/deleteme.txt
echo "Fichier de test 1" > www/files/test1.txt
echo "Fichier de test 2" > www/files/test2.txt

# Créer un fichier de test pour upload
echo "Contenu de test pour upload" > test_upload.txt

echo "✅ Configuration terminée !"
echo "Lancez: ./webserv configs/basic_test.conf"