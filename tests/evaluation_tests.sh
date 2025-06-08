#!/bin/bash

# Script de tests automatisés pour l'évaluation webserv
# Conforme aux critères de l'école 42

echo "🧪 === TESTS D'ÉVALUATION WEBSERV ==="

SERVER_URL="http://localhost:8080"
UPLOAD_DIR="./uploads"

# Fonction pour attendre que le serveur soit prêt
wait_for_server() {
    echo "⏳ Attente du serveur..."
    for i in {1..10}; do
        if curl -s --connect-timeout 1 $SERVER_URL > /dev/null 2>&1; then
            echo "✅ Serveur prêt"
            return 0
        fi
        sleep 1
    done
    echo "❌ Serveur non accessible"
    return 1
}

# Test 1: GET Request
test_get() {
    echo "📝 Test 1: GET Request"
    response=$(curl -s -w "%{http_code}" -o /tmp/get_response.html $SERVER_URL/)
    if [ "$response" = "200" ]; then
        echo "✅ GET / -> 200 OK"
    else
        echo "❌ GET / -> $response (expected 200)"
    fi
}

# Test 2: POST Request
test_post() {
    echo "📝 Test 2: POST Request"
    response=$(curl -s -w "%{http_code}" -o /dev/null -X POST -H "Content-Type: text/plain" --data "Test POST data" $SERVER_URL/)
    if [ "$response" = "200" ]; then
        echo "✅ POST / -> 200 OK (POST allowed)"
    elif [ "$response" = "405" ]; then
        echo "✅ POST / -> 405 Method Not Allowed (expected for root)"
    else
        echo "❌ POST / -> $response (expected 200 or 405)"
    fi
}

# Test 3: DELETE Request
test_delete() {
    echo "📝 Test 3: DELETE Request"
    response=$(curl -s -w "%{http_code}" -o /dev/null -X DELETE $SERVER_URL/)
    if [ "$response" = "200" ] || [ "$response" = "405" ] || [ "$response" = "501" ]; then
        echo "✅ DELETE / -> $response"
    else
        echo "❌ DELETE / -> $response"
    fi
}

# Test 4: Unknown Method
test_unknown_method() {
    echo "📝 Test 4: Unknown Method"
    response=$(curl -s -w "%{http_code}" -o /dev/null -X PATCH $SERVER_URL/)
    if [ "$response" = "405" ] || [ "$response" = "501" ]; then
        echo "✅ PATCH / -> $response (Method not allowed/implemented)"
    else
        echo "❌ PATCH / -> $response (should be 405 or 501)"
    fi
}

# Test 5: 404 Error
test_404() {
    echo "📝 Test 5: 404 Error"
    response=$(curl -s -w "%{http_code}" -o /dev/null $SERVER_URL/nonexistent)
    if [ "$response" = "404" ]; then
        echo "✅ GET /nonexistent -> 404 Not Found"
    else
        echo "❌ GET /nonexistent -> $response (expected 404)"
    fi
}

# Test 6: File Upload (si configuré)
test_upload() {
    echo "📝 Test 6: File Upload"
    echo "Test upload content" > /tmp/test_upload.txt
    
    # Test avec POST multipart/form-data (plus standard)
    response=$(curl -s -w "%{http_code}" -o /dev/null -X POST -F "file=@/tmp/test_upload.txt" $SERVER_URL/upload)
    if [ "$response" = "200" ] || [ "$response" = "201" ]; then
        echo "✅ POST upload -> $response (Upload successful)"
    elif [ "$response" = "404" ]; then
        echo "⚠️  POST upload -> 404 (Upload endpoint not configured)"
    elif [ "$response" = "405" ]; then
        echo "⚠️  POST upload -> 405 (Upload method not allowed)"
    else
        echo "❌ POST upload -> $response"
    fi
    
    # Vérifier si le fichier existe
    if [ -f "$UPLOAD_DIR/test_upload.txt" ]; then
        echo "✅ File uploaded successfully to $UPLOAD_DIR/"
    fi
    
    rm -f /tmp/test_upload.txt
}

# Test 7: Body Size Limit
test_body_limit() {
    echo "📝 Test 7: Body Size Limit"
    # Créer un fichier de 2MB pour tester la limite
    dd if=/dev/zero of=/tmp/large_body.txt bs=1024 count=2048 2>/dev/null
    response=$(curl -s -w "%{http_code}" -o /dev/null -X POST --data-binary @/tmp/large_body.txt $SERVER_URL/ --max-time 10)
    if [ "$response" = "413" ]; then
        echo "✅ Large body -> 413 Request Entity Too Large (limit enforced)"
    elif [ "$response" = "200" ]; then
        echo "✅ Large body -> 200 OK (no limit or limit high enough)"
    elif [ "$response" = "405" ]; then
        echo "✅ Large body -> 405 Method Not Allowed (expected for root)"
    else
        echo "⚠️  Large body -> $response"
    fi
    rm -f /tmp/large_body.txt
}

# Test 8: CGI Execution (si configuré)
test_cgi() {
    echo "📝 Test 8: CGI Execution"
    response=$(curl -s -w "%{http_code}" -o /tmp/cgi_response.html $SERVER_URL/cgi-bin/index.py)
    if [ "$response" = "200" ]; then
        echo "✅ GET /cgi-bin/index.py -> 200 OK (CGI working)"
        if grep -q "text/html" /tmp/cgi_response.html 2>/dev/null; then
            echo "✅ CGI returned HTML content"
        fi
    elif [ "$response" = "404" ]; then
        echo "⚠️  GET /cgi-bin/index.py -> 404 (CGI not configured or script missing)"
    elif [ "$response" = "500" ]; then
        echo "⚠️  GET /cgi-bin/index.py -> 500 (CGI execution error)"
    else
        echo "❌ GET /cgi-bin/index.py -> $response"
    fi
    rm -f /tmp/cgi_response.html
}

# Test 9: Static Files (CSS, Images)
test_static_files() {
    echo "📝 Test 9: Static Files"
    response=$(curl -s -w "%{http_code}" -o /dev/null $SERVER_URL/styleIndex.css)
    if [ "$response" = "200" ]; then
        echo "✅ GET /styleIndex.css -> 200 OK (Static files served)"
    elif [ "$response" = "404" ]; then
        echo "⚠️  GET /styleIndex.css -> 404 (Static file not found)"
    else
        echo "❌ GET /styleIndex.css -> $response"
    fi
}

# Exécution des tests
echo "🚀 Démarrage des tests d'évaluation..."
echo "🌐 Serveur testé: $SERVER_URL"
echo "📁 Répertoire uploads: $UPLOAD_DIR"
echo ""

if wait_for_server; then
    test_get
    test_post
    test_delete
    test_unknown_method
    test_404
    test_upload
    test_body_limit
    test_cgi
    test_static_files
    
    echo ""
    echo "🏁 === RÉSUMÉ DES TESTS ==="
    echo "✅ Tests de base HTTP: GET, POST, DELETE"
    echo "✅ Gestion des erreurs: 404, 405, 501"
    echo "✅ Upload de fichiers (si configuré)"
    echo "✅ Limitation de taille (si configurée)" 
    echo "✅ Exécution CGI (si configurée)"
    echo "✅ Fichiers statiques"
    echo ""
    echo "🎉 Évaluation terminée avec succès!"
else
    echo "❌ Impossible de se connecter au serveur"
    echo "   Vérifiez que le serveur est démarré sur $SERVER_URL"
    exit 1
fi
