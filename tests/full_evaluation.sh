#!/bin/bash

# Script principal d'évaluation webserv
# Conforme aux critères de l'école 42

echo "🎯 === ÉVALUATION COMPLÈTE WEBSERV ==="
echo "Date: $(date)"
echo "========================================="

# Couleurs pour l'affichage
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Variables globales
SCORE=0
MAX_SCORE=100
SERVER_PID=""

# Fonction d'affichage coloré
print_status() {
    local status=$1
    local message=$2
    if [ "$status" = "OK" ]; then
        echo -e "${GREEN}✅ $message${NC}"
        SCORE=$((SCORE + 5))
    elif [ "$status" = "WARNING" ]; then
        echo -e "${YELLOW}⚠️  $message${NC}"
        SCORE=$((SCORE + 2))
    else
        echo -e "${RED}❌ $message${NC}"
    fi
}

# 1. Vérifications préliminaires
echo -e "${BLUE}📋 1. VÉRIFICATIONS PRÉLIMINAIRES${NC}"

# Compilation
echo "🔧 Test de compilation..."
if make clean && make; then
    print_status "OK" "Compilation réussie"
else
    print_status "ERROR" "Échec de compilation"
    exit 1
fi

# Vérification des fichiers requis
echo "📁 Vérification des fichiers..."
required_files="webserv configs/default.conf Makefile"
for file in $required_files; do
    if [ -f "$file" ]; then
        print_status "OK" "Fichier $file présent"
    else
        print_status "ERROR" "Fichier $file manquant"
    fi
done

# 2. Tests de configuration
echo -e "\n${BLUE}📋 2. TESTS DE CONFIGURATION${NC}"

# Test de configuration invalide (même port)
echo "🔧 Test de configuration avec même port..."
if ./webserv configs/same_port_error_test.conf 2>&1 | grep -q "Address already in use\|Failed to bind"; then
    print_status "OK" "Détection correcte des ports dupliqués"
else
    print_status "WARNING" "Détection des ports dupliqués à vérifier"
fi

# 3. Démarrage du serveur pour les tests
echo -e "\n${BLUE}🚀 3. DÉMARRAGE DU SERVEUR${NC}"

./webserv configs/default.conf &
SERVER_PID=$!
sleep 3

# Vérifier que le serveur fonctionne
if ps -p $SERVER_PID > /dev/null; then
    print_status "OK" "Serveur démarré (PID: $SERVER_PID)"
else
    print_status "ERROR" "Échec du démarrage du serveur"
    exit 1
fi

# 4. Tests basiques HTTP
echo -e "\n${BLUE}🌐 4. TESTS HTTP BASIQUES${NC}"

# GET Request
if curl -s --connect-timeout 5 http://localhost:8080/ > /dev/null; then
    print_status "OK" "GET request fonctionne"
else
    print_status "ERROR" "GET request échoue"
fi

# POST Request
post_response=$(curl -s -w "%{http_code}" -o /dev/null -X POST -d "test" http://localhost:8080/)
if [ "$post_response" = "200" ] || [ "$post_response" = "405" ]; then
    print_status "OK" "POST request gérée (code: $post_response)"
else
    print_status "WARNING" "POST request: code $post_response"
fi

# DELETE Request
delete_response=$(curl -s -w "%{http_code}" -o /dev/null -X DELETE http://localhost:8080/)
if [ "$delete_response" = "200" ] || [ "$delete_response" = "405" ] || [ "$delete_response" = "501" ]; then
    print_status "OK" "DELETE request gérée (code: $delete_response)"
else
    print_status "WARNING" "DELETE request: code $delete_response"
fi

# Unknown Method
patch_response=$(curl -s -w "%{http_code}" -o /dev/null -X PATCH http://localhost:8080/)
if [ "$patch_response" = "405" ] || [ "$patch_response" = "501" ]; then
    print_status "OK" "Méthode inconnue gérée (code: $patch_response)"
else
    print_status "WARNING" "Méthode inconnue: code $patch_response"
fi

# 404 Error
notfound_response=$(curl -s -w "%{http_code}" -o /dev/null http://localhost:8080/nonexistent)
if [ "$notfound_response" = "404" ]; then
    print_status "OK" "404 Not Found correctement renvoyé"
else
    print_status "WARNING" "404 attendu, reçu: $notfound_response"
fi

# 5. Test avec telnet (simulation)
echo -e "\n${BLUE}📞 5. TEST TELNET${NC}"
(
    echo -e "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n"
    sleep 1
) | nc localhost 8080 > /tmp/telnet_test.txt 2>/dev/null

if grep -q "HTTP/1.1" /tmp/telnet_test.txt; then
    print_status "OK" "Réponse HTTP valide via telnet"
else
    print_status "WARNING" "Test telnet à vérifier manuellement"
fi

# 6. Tests de stress rapides
echo -e "\n${BLUE}⚡ 6. TESTS DE PERFORMANCE${NC}"

# Test de disponibilité avec curl en boucle
echo "🔄 Test de disponibilité (20 requêtes)..."
success_count=0
for i in {1..20}; do
    if curl -s --connect-timeout 1 http://localhost:8080/ > /dev/null; then
        success_count=$((success_count + 1))
    fi
done

availability=$((success_count * 100 / 20))
if [ $availability -ge 95 ]; then
    print_status "OK" "Disponibilité: $availability%"
else
    print_status "WARNING" "Disponibilité: $availability% (< 95%)"
fi

# 7. Vérification des fuites mémoire basique
echo -e "\n${BLUE}🧠 7. TEST MÉMOIRE${NC}"

initial_memory=$(ps -o rss= -p $SERVER_PID)
# Générer quelques requêtes
for i in {1..50}; do
    curl -s http://localhost:8080/ > /dev/null &
done
wait

sleep 2
final_memory=$(ps -o rss= -p $SERVER_PID)

if [ -n "$initial_memory" ] && [ -n "$final_memory" ]; then
    memory_growth=$((final_memory - initial_memory))
    if [ $memory_growth -lt 10000 ]; then  # Moins de 10MB
        print_status "OK" "Mémoire stable (croissance: ${memory_growth}KB)"
    else
        print_status "WARNING" "Croissance mémoire: ${memory_growth}KB"
    fi
else
    print_status "WARNING" "Surveillance mémoire à vérifier manuellement"
fi

# 8. Nettoyage
echo -e "\n${BLUE}🧹 8. NETTOYAGE${NC}"

if [ -n "$SERVER_PID" ]; then
    kill $SERVER_PID 2>/dev/null
    sleep 1
    if ps -p $SERVER_PID > /dev/null 2>&1; then
        kill -9 $SERVER_PID 2>/dev/null
    fi
    print_status "OK" "Serveur arrêté proprement"
fi

# Résumé final
echo -e "\n${BLUE}📊 RÉSUMÉ DE L'ÉVALUATION${NC}"
echo "================================"
echo "Score approximatif: $SCORE/$MAX_SCORE"

if [ $SCORE -ge 80 ]; then
    echo -e "${GREEN}🎉 Projet prêt pour l'évaluation${NC}"
elif [ $SCORE -ge 60 ]; then
    echo -e "${YELLOW}⚠️  Quelques améliorations recommandées${NC}"
else
    echo -e "${RED}❌ Corrections nécessaires avant l'évaluation${NC}"
fi

echo ""
echo "📝 Points à vérifier manuellement:"
echo "   - Tests avec navigateur web"
echo "   - Tests CGI approfondis" 
echo "   - Tests siege complets"
echo "   - Gestion des erreurs edge cases"
echo "   - Conformité HTTP/1.1 complète"

# Nettoyer les fichiers temporaires
rm -f /tmp/telnet_test.txt /tmp/memory_usage.csv 2>/dev/null

echo -e "\n✅ Évaluation terminée"
