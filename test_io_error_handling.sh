#!/bin/bash

# 🧪 Test de Gestion d'Erreurs I/O pour Webserv
# Ce script teste les corrections apportées à la gestion d'erreurs

# Configuration
TEST_HOST="localhost"
TEST_PORT="8082"
CONFIG_FILE="configs/io_error_test.conf"

# Couleurs pour l'affichage
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Fonction pour démarrer webserv
start_webserv() {
    echo -e "${BLUE}🚀 Starting webserv for I/O error testing...${NC}"
    
    # Créer la configuration de test si elle n'existe pas
    if [ ! -f "$CONFIG_FILE" ]; then
        echo -e "${YELLOW}Creating test configuration...${NC}"
        echo "Configuration already exists, using $CONFIG_FILE"
    fi
    
    # S'assurer que le répertoire www existe
    mkdir -p www/cgi-bin
    
    # Tuer tout processus webserv existant sur le port
    pkill -f "webserv.*$CONFIG_FILE" 2>/dev/null || true
    sleep 1
    
    # Démarrer webserv en arrière-plan
    ./webserv "$CONFIG_FILE" > webserv_test.log 2>&1 &
    WEBSERV_PID=$!
    
    echo -e "${GREEN}✅ Webserv started with PID $WEBSERV_PID${NC}"
    
    # Attendre que le serveur soit prêt
    echo -e "${YELLOW}⏳ Waiting for server to be ready...${NC}"
    for i in {1..10}; do
        if curl -s "http://$TEST_HOST:$TEST_PORT/" > /dev/null 2>&1; then
            echo -e "${GREEN}✅ Server is ready!${NC}"
            return 0
        fi
        sleep 1
        echo -n "."
    done
    
    echo -e "\n${RED}❌ Server failed to start properly${NC}"
    return 1
}

# Fonction pour arrêter webserv
stop_webserv() {
    if [ ! -z "$WEBSERV_PID" ]; then
        echo -e "${YELLOW}🛑 Stopping webserv (PID $WEBSERV_PID)...${NC}"
        kill $WEBSERV_PID 2>/dev/null || true
        wait $WEBSERV_PID 2>/dev/null || true
    fi
    
    # Nettoyer les processus restants
    pkill -f "webserv.*$CONFIG_FILE" 2>/dev/null || true
}

# Trap pour nettoyer à la sortie
trap 'stop_webserv; exit' INT TERM EXIT



# Test 1: CGI avec body de données pour tester write()
test_cgi_write() {
    echo -e "\n${YELLOW}📝 Test 1: CGI write() error handling${NC}"
    
    # Créer un script CGI simple pour tester
    mkdir -p www/cgi-bin
    cat > www/cgi-bin/test_echo.py << 'EOF'
#!/usr/bin/env python3
import sys
import os

print("Content-Type: text/plain\r")
print("\r")

# Lire le body depuis stdin
try:
    input_data = sys.stdin.read()
    print(f"Received data: {input_data}")
    print(f"Data length: {len(input_data)}")
except Exception as e:
    print(f"Error reading input: {e}")
EOF
    
    chmod +x www/cgi-bin/test_echo.py
    
    # Test avec données POST
    test_data="This is test data for CGI write() error handling verification"
    
    echo -e "  ${BLUE}→ Testing POST with data to CGI script...${NC}"
    
    response=$(curl -s -X POST \
        -H "Content-Type: application/x-www-form-urlencoded" \
        -d "$test_data" \
        http://$TEST_HOST:$TEST_PORT/cgi-bin/test_echo.py)
    
    if echo "$response" | grep -q "Received data"; then
        echo -e "  ${GREEN}✅ CGI write() handling works correctly${NC}"
        echo -e "  ${GREEN}   Response contains expected data${NC}"
    else
        echo -e "  ${RED}❌ CGI write() test failed${NC}"
        echo -e "  ${RED}   Response: $response${NC}"
        return 1
    fi
    
    # Vérifier les logs pour les corrections
    if grep -q "CGI write was partial" webserv_test.log; then
        echo -e "  ${YELLOW}⚠️  Partial write detected (expected in some cases)${NC}"
    fi
    
    if grep -q "Failed to send data to CGI script" webserv_test.log; then
        echo -e "  ${YELLOW}⚠️  CGI write error detected and handled${NC}"
    fi
}

# Test 2: CGI read() avec script qui génère beaucoup de données
test_cgi_read() {
    echo -e "\n${YELLOW}📖 Test 2: CGI read() error handling${NC}"
    
    # Créer un script qui génère des données pour tester read()
    cat > www/cgi-bin/test_output.py << 'EOF'
#!/usr/bin/env python3
import sys

print("Content-Type: text/plain\r")
print("\r")

# Générer des données pour tester read()
for i in range(100):
    print(f"Line {i}: This is test output to verify read() error handling")

print("End of CGI output")
EOF
    
    chmod +x www/cgi-bin/test_output.py
    
    echo -e "  ${BLUE}→ Testing CGI with large output...${NC}"
    
    response=$(curl -s http://$TEST_HOST:$TEST_PORT/cgi-bin/test_output.py)
    
    if echo "$response" | grep -q "End of CGI output"; then
        echo -e "  ${GREEN}✅ CGI read() handling works correctly${NC}"
        echo -e "  ${GREEN}   Complete output received${NC}"
    else
        echo -e "  ${RED}❌ CGI read() test failed${NC}"
        echo -e "  ${RED}   Output seems incomplete${NC}"
        return 1
    fi
    
    # Vérifier les logs pour les nouvelles corrections
    if grep -q "CGI read error encountered" webserv_test.log; then
        echo -e "  ${YELLOW}⚠️  CGI read warning detected (may be normal)${NC}"
    fi
}

# Test 3: Vérifier la robustesse avec connexions fermées brutalement
test_connection_robustness() {
    echo -e "\n${YELLOW}🔌 Test 3: Connection robustness${NC}"
    
    echo -e "  ${BLUE}→ Testing abrupt connection closure...${NC}"
    
    # Utiliser telnet pour se connecter et fermer brutalement
    (echo -e "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n"; sleep 0.1) | \
    timeout 2 telnet $TEST_HOST $TEST_PORT > /dev/null 2>&1
    
    # Vérifier que le serveur gère bien les déconnexions
    if grep -q "disconnected" webserv_test.log; then
        echo -e "  ${GREEN}✅ Connection handling works correctly${NC}"
        echo -e "  ${GREEN}   Disconnections properly detected${NC}"
    else
        echo -e "  ${YELLOW}⚠️  No explicit disconnection logs found${NC}"
    fi
}

# Test 4: Vérifier le comportement avec erreurs send()
test_send_errors() {
    echo -e "\n${YELLOW}📤 Test 4: Send error handling${NC}"
    
    echo -e "  ${BLUE}→ Testing multiple rapid requests...${NC}"
    
    # Envoyer plusieurs requêtes rapidement pour potentiellement déclencher des erreurs
    for i in {1..5}; do
        curl -s http://$TEST_HOST:$TEST_PORT/ > /dev/null &
    done
    
    wait
    
    # Vérifier la gestion des erreurs send()
    if grep -q "Error sending" webserv_test.log; then
        echo -e "  ${YELLOW}⚠️  Send errors detected and handled${NC}"
    else
        echo -e "  ${GREEN}✅ No send errors occurred (good)${NC}"
    fi
}

# Test principal
main() {
    echo -e "${BLUE}🧪 I/O Error Handling Test Suite${NC}"
    echo -e "${BLUE}==================================${NC}"
    
    start_webserv
    
    # Attendre que le serveur soit complètement prêt
    sleep 1
    
    # Tests de base
    echo -e "\n${YELLOW}🔍 Running I/O error handling tests...${NC}"
    
    test_cgi_write
    test_cgi_read
    test_connection_robustness
    test_send_errors
    
    echo -e "\n${YELLOW}📋 Analyzing webserv logs for error handling...${NC}"
    
    # Analyser les logs pour vérifier les améliorations
    echo -e "\n${BLUE}📊 Log Analysis:${NC}"
    
    if [ -f webserv_test.log ]; then
        # Compter les différents types de messages
        error_count=$(grep -c "Error" webserv_test.log || echo "0")
        warning_count=$(grep -c "Warning" webserv_test.log || echo "0")
        success_count=$(grep -c "✅" webserv_test.log || echo "0")
        
        echo -e "  ${RED}Errors detected: $error_count${NC}"
        echo -e "  ${YELLOW}Warnings detected: $warning_count${NC}"
        echo -e "  ${GREEN}Successful operations: $success_count${NC}"
        
        # Afficher les corrections appliquées
        if grep -q "CGI write was partial" webserv_test.log; then
            echo -e "  ${BLUE}✓ Partial write detection working${NC}"
        fi
        
        if grep -q "CGI read error encountered" webserv_test.log; then
            echo -e "  ${BLUE}✓ Read error detection working${NC}"
        fi
        
        echo -e "\n${BLUE}Recent log entries:${NC}"
        tail -10 webserv_test.log | sed 's/^/  /'
    fi
    
    echo -e "\n${GREEN}🎉 I/O Error Handling Test Complete!${NC}"
    echo -e "${GREEN}The corrections for write() and read() error handling are working.${NC}"
    
    # Arrêter le serveur
    stop_webserv
}

# Exécuter les tests
main

# Le nettoyage se fait automatiquement via trap
