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
    if [ "$response" = "200" ] || [ "$response" = "405" ]; then
        echo "✅ POST / -> $response"
    else
        echo "❌ POST / -> $response"
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
    response=$(curl -s -w "%{http_code}" -o /dev/null -X PUT --data-binary @/tmp/test_upload.txt $SERVER_URL/upload/test_upload.txt)
    if [ "$response" = "200" ] || [ "$response" = "201" ] || [ "$response" = "404" ]; then
        echo "✅ PUT upload -> $response"
        if [ -f "$UPLOAD_DIR/test_upload.txt" ]; then
            echo "✅ File uploaded successfully"
        fi
    else
        echo "❌ PUT upload -> $response"
    fi
    rm -f /tmp/test_upload.txt
}

# Test 7: Body Size Limit
test_body_limit() {
    echo "📝 Test 7: Body Size Limit"
    # Créer un fichier de 200 bytes (plus grand que la limite si configurée à 100)
    dd if=/dev/zero of=/tmp/large_body.txt bs=200 count=1 2>/dev/null
    response=$(curl -s -w "%{http_code}" -o /dev/null -X POST --data-binary @/tmp/large_body.txt $SERVER_URL/)
    if [ "$response" = "413" ] || [ "$response" = "200" ]; then
        echo "✅ Large body -> $response"
    else
        echo "❌ Large body -> $response"
    fi
    rm -f /tmp/large_body.txt
}

# Exécution des tests
if wait_for_server; then
    test_get
    test_post
    test_delete
    test_unknown_method
    test_404
    test_upload
    test_body_limit
    echo "🏁 Tests terminés"
else
    echo "❌ Impossible de se connecter au serveur"
    exit 1
fi
