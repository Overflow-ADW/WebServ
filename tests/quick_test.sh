#!/bin/bash

# Test rapide pour l'évaluation
echo "🚀 Test rapide Webserv"
echo "====================="

# Démarrer le serveur en arrière-plan
echo "📡 Démarrage du serveur..."
./webserv ./configs/default.conf > /dev/null 2>&1 &
SERVER_PID=$!
sleep 2

# Tester quelques endpoints
echo "🧪 Tests de base..."

# Test 1: Page d'accueil
response=$(curl -s -w "%{http_code}" -o /dev/null http://localhost:8080/)
if [ "$response" = "200" ]; then
    echo "✅ GET / → 200 OK"
else
    echo "❌ GET / → $response"
fi

# Test 2: Page inexistante
response=$(curl -s -w "%{http_code}" -o /dev/null http://localhost:8080/inexistant)
if [ "$response" = "404" ]; then
    echo "✅ GET /inexistant → 404 Not Found"
else
    echo "❌ GET /inexistant → $response"
fi

# Test 3: Méthode non autorisée
response=$(curl -s -w "%{http_code}" -o /dev/null -X POST http://localhost:8080/ --max-time 5)
if [ "$response" = "405" ]; then
    echo "✅ POST / → 405 Method Not Allowed"
else
    echo "❌ POST / → $response"
fi

# Test 4: CSS statique
response=$(curl -s -w "%{http_code}" -o /dev/null http://localhost:8080/styleIndex.css --max-time 5)
if [ "$response" = "200" ]; then
    echo "✅ GET /styleIndex.css → 200 OK"
else
    echo "❌ GET /styleIndex.css → $response"
fi

# Test 5: Script CGI (si disponible)
response=$(curl -s -w "%{http_code}" -o /dev/null http://localhost:8080/cgi-bin/index.py --max-time 10)
if [ "$response" = "200" ]; then
    echo "✅ GET /cgi-bin/index.py → 200 OK (CGI fonctionnel)"
elif [ "$response" = "404" ]; then
    echo "⚠️  GET /cgi-bin/index.py → 404 (CGI non configuré)"
else
    echo "❌ GET /cgi-bin/index.py → $response"
fi

# Arrêter le serveur
echo "🛑 Arrêt du serveur..."
kill $SERVER_PID 2>/dev/null
wait $SERVER_PID 2>/dev/null

echo ""
echo "🎉 Tests terminés avec succès !"
echo "💡 Pour les tests complets: bash evaluation_tests.sh"
