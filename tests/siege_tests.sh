#!/bin/bash

# Tests de performance avec curl (sans nécessiter siege)
echo "🚀 Tests de performance Webserv avec curl"
echo "========================================"

# Couleurs pour l'affichage
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

SERVER_URL="http://localhost:8080"

# Fonction pour attendre le serveur
wait_for_server() {
    echo "⏳ Attente du serveur..."
    i=1
    while [ $i -le 15 ]; do
        if curl -s --connect-timeout 1 --max-time 2 $SERVER_URL > /dev/null 2>&1; then
            echo -e "${GREEN}✅ Serveur prêt${NC}"
            return 0
        fi
        sleep 1
        i=$((i + 1))
    done
    echo -e "${RED}❌ Serveur non accessible${NC}"
    return 1
}

# Test 1: Test de disponibilité basique
test_availability() {
    echo -e "\n${BLUE}📊 Test 1: Disponibilité et temps de réponse${NC}"
    
    success=0
    total=50
    total_time=0
    
    for i in $(seq 1 $total); do
        start_time=$(date +%s%N)
        if curl -s --connect-timeout 2 --max-time 5 -o /dev/null -w "%{http_code}" $SERVER_URL/ | grep -q "200"; then
            success=$((success + 1))
        fi
        end_time=$(date +%s%N)
        duration=$(( (end_time - start_time) / 1000000 )) # ms
        total_time=$((total_time + duration))
        
        # Affichage du progrès
        if [ $((i % 10)) -eq 0 ]; then
            echo -n "."
        fi
    done
    echo ""
    
    availability=$((success * 100 / total))
    avg_time=$((total_time / total))
    
    if [ $availability -ge 95 ]; then
        echo -e "${GREEN}✅ Disponibilité: $availability% ($success/$total)${NC}"
    else
        echo -e "${RED}❌ Disponibilité: $availability% ($success/$total) - Attendu ≥ 95%${NC}"
    fi
    
    echo -e "${BLUE}⏱️  Temps de réponse moyen: ${avg_time}ms${NC}"
}

# Test 2: Test de charge avec requêtes simultanées
test_concurrent_load() {
    echo -e "\n${BLUE}📊 Test 2: Charge avec requêtes simultanées${NC}"
    
    # Créer des requêtes en parallèle
    start_time=$(date +%s)
    
    # Lancer 20 requêtes en arrière-plan
    pids=""
    i=1
    while [ $i -le 20 ]; do
        (
            j=1
            while [ $j -le 5 ]; do
                curl -s --connect-timeout 2 --max-time 5 -o /dev/null $SERVER_URL/ 
                sleep 0.1
                j=$((j + 1))
            done
        ) &
        if [ -z "$pids" ]; then
            pids="$!"
        else
            pids="$pids $!"
        fi
        i=$((i + 1))
    done
    
    # Attendre la fin de toutes les requêtes
    success=0
    for pid in $pids; do
        if wait $pid; then
            success=$((success + 1))
        fi
    done
    
    end_time=$(date +%s)
    duration=$((end_time - start_time))
    
    echo -e "${GREEN}✅ Processus terminés avec succès: $success/20${NC}"
    echo -e "${BLUE}⏱️  Durée totale: ${duration}s${NC}"
}

# Test 3: Test de différents types de requêtes
test_various_requests() {
    echo -e "\n${BLUE}📊 Test 3: Variété de requêtes HTTP${NC}"
    
    # Test GET root
    if curl -s --connect-timeout 2 --max-time 5 -o /dev/null -w "%{http_code}" $SERVER_URL/ | grep -q "200"; then
        echo -e "${GREEN}✅ GET / → 200 OK${NC}"
    else
        echo -e "${RED}❌ GET / échoué${NC}"
    fi
    
    # Test GET CSS
    if curl -s --connect-timeout 2 --max-time 5 -o /dev/null -w "%{http_code}" $SERVER_URL/styleIndex.css | grep -q "200"; then
        echo -e "${GREEN}✅ GET /styleIndex.css → 200 OK${NC}"
    else
        echo -e "${YELLOW}⚠️  GET /styleIndex.css échoué (fichier peut-être absent)${NC}"
    fi
    
    # Test 404
    if curl -s --connect-timeout 2 --max-time 5 -o /dev/null -w "%{http_code}" $SERVER_URL/nonexistent | grep -q "404"; then
        echo -e "${GREEN}✅ GET /nonexistent → 404 Not Found${NC}"
    else
        echo -e "${RED}❌ GET /nonexistent n'a pas retourné 404${NC}"
    fi
    
    # Test POST
    post_code=$(curl -s --connect-timeout 2 --max-time 5 -o /dev/null -w "%{http_code}" -X POST -d "test=data" $SERVER_URL/)
    if [ "$post_code" = "200" ] || [ "$post_code" = "405" ]; then
        echo -e "${GREEN}✅ POST / → $post_code (attendu 200 ou 405)${NC}"
    else
        echo -e "${YELLOW}⚠️  POST / → $post_code${NC}"
    fi
}

# Test 4: Monitoring des ressources serveur
test_resource_monitoring() {
    echo -e "\n${BLUE}📊 Test 4: Surveillance des ressources${NC}"
    
    # Trouver le PID du serveur webserv
    WEBSERV_PID=$(pgrep -f "webserv.*conf" | head -1)
    
    if [ -z "$WEBSERV_PID" ]; then
        echo -e "${YELLOW}⚠️  Impossible de trouver le processus webserv${NC}"
        return
    fi
    
    echo "🔍 PID du serveur: $WEBSERV_PID"
    
    # Capturer l'état initial
    initial_mem=$(ps -p $WEBSERV_PID -o rss= 2>/dev/null)
    initial_cpu=$(ps -p $WEBSERV_PID -o %cpu= 2>/dev/null)
    
    if [ -z "$initial_mem" ]; then
        echo -e "${YELLOW}⚠️  Impossible de lire les statistiques du processus${NC}"
        return
    fi
    
    echo "📈 Mémoire initiale: ${initial_mem}KB"
    echo "🖥️  CPU initial: ${initial_cpu}%"
    
    # Faire quelques requêtes et surveiller
    echo "🔄 Génération de charge pendant 10 secondes..."
    i=1
    while [ $i -le 20 ]; do
        curl -s --connect-timeout 1 --max-time 2 -o /dev/null $SERVER_URL/ &
        if [ $((i % 5)) -eq 0 ]; then
            sleep 0.5
        fi
        i=$((i + 1))
    done
    
    # Attendre un peu pour que les requêtes se terminent
    sleep 2
    
    # Capturer l'état final
    final_mem=$(ps -p $WEBSERV_PID -o rss= 2>/dev/null)
    final_cpu=$(ps -p $WEBSERV_PID -o %cpu= 2>/dev/null)
    
    if [ -n "$final_mem" ]; then
        mem_diff=$((final_mem - initial_mem))
        echo "📈 Mémoire finale: ${final_mem}KB"
        echo "📊 Différence mémoire: ${mem_diff}KB"
        
        if [ $mem_diff -lt 5000 ]; then  # Moins de 5MB d'augmentation
            echo -e "${GREEN}✅ Usage mémoire stable${NC}"
        else
            echo -e "${YELLOW}⚠️  Augmentation mémoire significative: ${mem_diff}KB${NC}"
        fi
    fi
}

# Test 5: Test de persistance des connexions
test_connection_handling() {
    echo -e "\n${BLUE}📊 Test 5: Gestion des connexions${NC}"
    
    # Test de multiples requêtes rapides
    echo "🔄 Test de 10 requêtes rapides..."
    success=0
    i=1
    while [ $i -le 10 ]; do
        if curl -s --connect-timeout 1 --max-time 3 -o /dev/null $SERVER_URL/ 2>/dev/null; then
            success=$((success + 1))
        fi
        i=$((i + 1))
    done
    
    echo -e "${GREEN}✅ Requêtes réussies: $success/10${NC}"
    
    # Vérifier que le serveur répond toujours
    if curl -s --connect-timeout 2 --max-time 5 -o /dev/null $SERVER_URL/ 2>/dev/null; then
        echo -e "${GREEN}✅ Serveur toujours opérationnel après les tests${NC}"
    else
        echo -e "${RED}❌ Serveur ne répond plus après les tests${NC}"
    fi
}

# Fonction principale d'exécution des tests
run_performance_tests() {
    echo -e "${BLUE}🎯 Démarrage des tests de performance...${NC}"
    echo "Serveur testé: $SERVER_URL"
    echo ""
    
    if wait_for_server; then
        test_availability
        test_concurrent_load  
        test_various_requests
        test_resource_monitoring
        test_connection_handling
        
        echo -e "\n${GREEN}🏁 Tests de performance terminés avec succès!${NC}"
        echo -e "${BLUE}💡 Tous les tests ont été effectués avec curl (pas besoin de siege)${NC}"
    else
        echo -e "${RED}❌ Impossible de se connecter au serveur pour les tests${NC}"
        exit 1
    fi
}

# Exécution des tests
run_performance_tests
