#!/bin/bash

# Tests de performance avec Siege
echo "🚀 Tests de performance avec Siege"

# Vérifier si siege est installé
if ! command -v siege &> /dev/null; then
    echo "❌ Siege n'est pas installé. Installation..."
    # Sur Ubuntu/Debian
    if command -v apt-get &> /dev/null; then
        sudo apt-get update && sudo apt-get install -y siege
    # Sur macOS
    elif command -v brew &> /dev/null; then
        brew install siege
    # Sur CentOS/RHEL
    elif command -v yum &> /dev/null; then
        sudo yum install -y siege
    else
        echo "❌ Impossible d'installer siege automatiquement"
        exit 1
    fi
fi

# Configuration de siege
echo "📝 Configuration de Siege..."
cat > ~/.siegerc << 'EOF'
# Siege configuration
verbose = true
csv = false
timestamp = true
limit = 1000
EOF

SERVER_URL="http://localhost:8080"

# Fonction pour attendre le serveur
wait_for_server() {
    echo "⏳ Attente du serveur..."
    for i in {1..15}; do
        if curl -s --connect-timeout 1 $SERVER_URL > /dev/null 2>&1; then
            echo "✅ Serveur prêt"
            return 0
        fi
        sleep 1
    done
    echo "❌ Serveur non accessible"
    return 1
}

# Test 1: Disponibilité simple
test_availability() {
    echo "📊 Test 1: Disponibilité avec siege (60 secondes)"
    siege -c 10 -t 60s --log=/tmp/siege.log $SERVER_URL/ 2>&1 | tee /tmp/siege_output.txt
    
    # Analyser les résultats
    if [ -f /tmp/siege_output.txt ]; then
        availability=$(grep "Availability" /tmp/siege_output.txt | awk '{print $2}' | tr -d '%')
        if [[ "$availability" =~ ^[0-9]+\.?[0-9]*$ ]] && (( $(echo "$availability >= 99.5" | bc -l) )); then
            echo "✅ Disponibilité: $availability% (≥ 99.5%)"
        else
            echo "❌ Disponibilité: $availability% (< 99.5%)"
        fi
    fi
}

# Test 2: Stress test avec benchmark
test_benchmark() {
    echo "📊 Test 2: Benchmark de performance"
    siege -c 25 -r 100 --log=/tmp/siege_bench.log $SERVER_URL/ 2>&1 | tee /tmp/siege_bench.txt
    
    # Analyser les résultats
    if [ -f /tmp/siege_bench.txt ]; then
        echo "=== Résultats du benchmark ==="
        grep -E "(Transaction rate|Response time|Availability)" /tmp/siege_bench.txt
    fi
}

# Test 3: Test de connexions simultanées
test_concurrent() {
    echo "📊 Test 3: Connexions simultanées"
    siege -c 50 -t 30s --log=/tmp/siege_concurrent.log $SERVER_URL/ 2>&1 | tee /tmp/siege_concurrent.txt
    
    if [ -f /tmp/siege_concurrent.txt ]; then
        failed=$(grep "Failed transactions" /tmp/siege_concurrent.txt | awk '{print $3}')
        if [ "$failed" = "0" ] || [ -z "$failed" ]; then
            echo "✅ Aucune transaction échouée"
        else
            echo "⚠️  Transactions échouées: $failed"
        fi
    fi
}

# Test 4: Surveillance mémoire pendant le stress test
test_memory() {
    echo "📊 Test 4: Surveillance mémoire"
    
    # Démarrer la surveillance en arrière-plan
    (
        echo "PID,TIME,VSZ,RSS" > /tmp/memory_usage.csv
        while sleep 1; do
            ps aux | grep "[w]ebserv" | awk '{print $2","strftime("%H:%M:%S")","$5","$6}' >> /tmp/memory_usage.csv
        done
    ) &
    MONITOR_PID=$!
    
    # Lancer siege
    siege -c 20 -t 30s $SERVER_URL/ > /dev/null 2>&1
    
    # Arrêter la surveillance
    kill $MONITOR_PID 2>/dev/null
    
    # Analyser l'usage mémoire
    if [ -f /tmp/memory_usage.csv ]; then
        echo "📈 Usage mémoire pendant le test:"
        tail -5 /tmp/memory_usage.csv
        
        # Vérifier s'il y a une fuite (croissance continue)
        start_rss=$(tail -20 /tmp/memory_usage.csv | head -1 | cut -d',' -f4)
        end_rss=$(tail -1 /tmp/memory_usage.csv | cut -d',' -f4)
        
        if [[ "$start_rss" =~ ^[0-9]+$ ]] && [[ "$end_rss" =~ ^[0-9]+$ ]]; then
            growth=$((end_rss - start_rss))
            if [ $growth -lt 10000 ]; then  # Moins de 10MB de croissance
                echo "✅ Mémoire stable (croissance: ${growth}KB)"
            else
                echo "⚠️  Possible fuite mémoire (croissance: ${growth}KB)"
            fi
        fi
    fi
}

# Exécution des tests
if wait_for_server; then
    test_availability
    echo ""
    test_benchmark
    echo ""
    test_concurrent
    echo ""
    test_memory
    echo "🏁 Tests de performance terminés"
else
    echo "❌ Impossible de se connecter au serveur pour les tests"
    exit 1
fi
