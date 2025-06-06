#!/bin/bash

# Test de fuites mémoire avec valgrind
echo "🔍 Test des fuites mémoire avec Valgrind"

# Créer une configuration simple pour valgrind
cat > configs/valgrind_test.conf << 'EOF'
server {
    listen 8080;
    server_name localhost;
    root ./www;
    index index.html;
    client_max_body_size 1000000;
    
    location / {
        allowed_methods GET POST DELETE;
        autoindex off;
    }
}
EOF

echo "📝 Lancement du serveur avec Valgrind..."
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind_log.txt ./webserv configs/valgrind_test.conf &
VALGRIND_PID=$!

echo "⏳ Attente du démarrage..."
sleep 3

echo "🧪 Exécution de quelques requêtes de test..."
curl -s http://localhost:8080/ > /dev/null
curl -s http://localhost:8080/nonexistent > /dev/null
curl -s -X POST -d "test" http://localhost:8080/ > /dev/null

echo "🛑 Arrêt du serveur..."
kill $VALGRIND_PID 2>/dev/null
wait $VALGRIND_PID 2>/dev/null

echo "📊 Analyse des fuites mémoire..."
if [ -f valgrind_log.txt ]; then
    echo "--- Rapport Valgrind ---"
    grep -E "(ERROR SUMMARY|definitely lost|indirectly lost|possibly lost)" valgrind_log.txt
    echo "--- Fin du rapport ---"
    
    # Vérifier s'il y a des fuites
    definitely_lost=$(grep "definitely lost" valgrind_log.txt | grep -o "[0-9,]* bytes" | head -1 | tr -d ',')
    if [[ "$definitely_lost" =~ ^[0-9]+$ ]] && [ "$definitely_lost" -gt 0 ]; then
        echo "❌ Fuites mémoire détectées: $definitely_lost bytes"
    else
        echo "✅ Aucune fuite mémoire détectée"
    fi
else
    echo "❌ Fichier de log Valgrind non trouvé"
fi
