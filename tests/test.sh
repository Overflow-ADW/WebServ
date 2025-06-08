#!/bin/bash

# Compilation du projet
cd /home/fvastena/Desktop/webserv
make clean && make

# Démarrage du serveur en arrière-plan
./webserv configs/default.conf &
SERVER_PID=$!
sleep 2

echo "=== EXECUTION DES TESTS UBUNTU ==="

# Test principal Ubuntu
echo "--- Test Ubuntu Principal ---"
cd test
./ubuntu_tester

echo "--- Test CGI Ubuntu ---"
./ubuntu_cgi_tester

# Arrêt du serveur
kill $SERVER_PID