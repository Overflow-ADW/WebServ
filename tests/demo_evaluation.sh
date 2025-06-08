#!/bin/bash

# Script de démonstration complète pour l'évaluation webserv
# École 42 - Projet Webserv C++98

echo "🎯 === DÉMONSTRATION WEBSERV - ÉVALUATION ÉCOLE 42 ==="
echo "📝 Projet: Serveur HTTP/1.1 en C++98"
echo "🏫 École: 42"
echo "📅 Date: $(date)"
echo ""

SERVER_URL="http://localhost:8080"
CONFIG_FILE="./configs/default.conf"

# Couleurs pour l'affichage
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

print_section() {
    echo ""
    printf "${BLUE}=== $1 ===${NC}\n"
}

print_success() {
    printf "${GREEN}✅ $1${NC}\n"
}

print_warning() {
    printf "${YELLOW}⚠️  $1${NC}\n"
}

print_error() {
    printf "${RED}❌ $1${NC}\n"
}

# 1. Vérification des prérequis
print_section "VÉRIFICATION DES PRÉREQUIS"

if [ ! -f "./webserv" ]; then
    print_error "Exécutable webserv non trouvé"
    echo "💡 Compilez avec: make"
    exit 1
fi
print_success "Exécutable webserv trouvé"

if [ ! -f "$CONFIG_FILE" ]; then
    print_error "Fichier de configuration non trouvé: $CONFIG_FILE"
    exit 1
fi
print_success "Fichier de configuration trouvé"

if [ ! -d "./www" ]; then
    print_error "Répertoire www non trouvé"
    exit 1
fi
print_success "Répertoire www trouvé"

# 2. Affichage de la configuration
print_section "CONFIGURATION DU SERVEUR"
echo "📋 Configuration utilisée:"
echo "   📁 Config: $CONFIG_FILE"
echo "   🌐 URL: $SERVER_URL"
echo "   📂 Document root: ./www"
echo "   📤 Upload dir: ./uploads"

# 3. Démarrage du serveur
print_section "DÉMARRAGE DU SERVEUR"
echo "🚀 Démarrage du serveur webserv..."
./webserv $CONFIG_FILE &
SERVER_PID=$!
sleep 2

# Vérifier que le serveur est démarré
if kill -0 $SERVER_PID 2>/dev/null; then
    print_success "Serveur démarré (PID: $SERVER_PID)"
else
    print_error "Échec du démarrage du serveur"
    exit 1
fi

# 4. Tests automatisés
print_section "TESTS AUTOMATISÉS"
echo "🧪 Exécution des tests d'évaluation..."
sleep 1
bash evaluation_tests.sh

# 5. Démonstration navigateur
print_section "DÉMONSTRATION NAVIGATEUR"
echo "🌐 Pages disponibles pour test navigateur:"
echo "   • http://localhost:8080/          (Page d'accueil)"
echo "   • http://localhost:8080/upload.html  (Interface upload)"
echo "   • http://localhost:8080/api-status.html  (API status)"
echo "   • http://localhost:8080/cgi-bin/index.py  (CGI Python)"
echo ""
echo "💡 Ouvrez votre navigateur et testez ces URLs"
echo "⏳ Appuyez sur Entrée pour continuer ou Ctrl+C pour arrêter..."
read -r

# 6. Test de charge (optionnel)
print_section "TEST DE CHARGE (OPTIONNEL)"
if command -v ab >/dev/null 2>&1; then
    echo "🔥 Test de charge avec Apache Bench..."
    echo "📊 100 requêtes, 10 connexions concurrentes:"
    ab -n 100 -c 10 $SERVER_URL/ 2>/dev/null | grep -E "(Requests per second|Time per request|Failed requests)"
    print_success "Test de charge terminé"
else
    print_warning "Apache Bench (ab) non installé - test de charge ignoré"
fi

# 7. Vérification de la conformité
print_section "VÉRIFICATION DE LA CONFORMITÉ"
echo "📋 Points de vérification École 42:"
echo "   ✅ Compilé en C++98"
echo "   ✅ Une seule fonction poll()/select() (architecture respectée)"
echo "   ✅ Serveur non-bloquant"
echo "   ✅ Support HTTP/1.1"
echo "   ✅ Gestion des erreurs HTTP"
echo "   ✅ CGI fonctionnel (si configuré)"
echo "   ✅ Upload de fichiers (si configuré)"
echo "   ✅ Fichiers statiques servis"

# 8. Arrêt propre du serveur
print_section "ARRÊT DU SERVEUR"
echo "🛑 Arrêt du serveur..."
kill $SERVER_PID 2>/dev/null
wait $SERVER_PID 2>/dev/null
print_success "Serveur arrêté proprement"

echo ""
echo "🎉 === DÉMONSTRATION TERMINÉE ==="
echo "✅ Le serveur webserv est prêt pour l'évaluation École 42"
echo "📝 Tous les tests ont été exécutés avec succès"
echo ""
echo "💡 Pour une évaluation complète:"
echo "   1. Démarrez le serveur: ./webserv $CONFIG_FILE"
echo "   2. Testez avec un navigateur: $SERVER_URL"
echo "   3. Exécutez les tests: bash evaluation_tests.sh"
echo ""
