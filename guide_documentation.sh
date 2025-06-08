#!/bin/bash

# Script d'aide pour la navigation dans la documentation
echo "📚 DOCUMENTATION WEBSERV - GUIDE DE NAVIGATION"
echo "=============================================="
echo ""

# Couleurs
BLUE='\033[0;34m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

print_section() {
    printf "\n${BLUE}=== $1 ===${NC}\n"
}

print_file() {
    printf "${GREEN}📄 $1${NC}\n"
    printf "   $2\n"
}

print_script() {
    printf "${YELLOW}🔧 $1${NC}\n"
    printf "   $2\n"
}

print_section "DOCUMENTATION PRINCIPALE"
print_file "README_EVALUATION.md" "Guide principal pour l'évaluation École 42"
print_file "VALIDATION_FINALE_ECOLE42.md" "Rapport final de validation complète"
print_file "RESUME_EXECUTIF.md" "Résumé exécutif du projet"
print_file "ARCHITECTURE_FINALE_REPORT.md" "Rapport technique détaillé de l'architecture"

print_section "RAPPORTS DE VALIDATION"
print_file "VALIDATION_FINALE_COMPLETE.md" "Tests complets et résultats de validation"
print_file "FINAL_IO_ERROR_CORRECTIONS.md" "Corrections des violations I/O critiques"
print_file "FILE_UPLOAD_DOCUMENTATION.md" "Documentation du système d'upload"

print_section "DOCUMENTATION TECHNIQUE"
print_file "Lina.md" "Fichiers et fonctions CGI du projet (documentation complète)"
print_file "COMPLETE_WEBSERV_DOCUMENTATION.md" "Documentation technique complète"
print_file "ENHANCED_UTILITIES_REPORT.md" "Rapport sur les utilitaires améliorés"

print_section "SCRIPTS DE TEST ET VALIDATION"
print_script "demonstration_finale.sh" "Démonstration complète du projet"
print_script "final_validation.sh" "Validation finale pour l'évaluation"
print_script "evaluation_tests.sh" "Tests automatisés d'évaluation"
print_script "quick_test.sh" "Tests rapides de base"
print_script "demo_evaluation.sh" "Démonstration avec interface utilisateur"

print_section "SCRIPTS SPÉCIALISÉS"
print_script "memory_test.sh" "Tests de gestion mémoire"
print_script "siege_tests.sh" "Tests de charge avec Siege"
print_script "integration_test.sh" "Tests d'intégration"

print_section "CONFIGURATION"
print_file "configs/default.conf" "Configuration principale du serveur"
print_file "configs/ubuntu_test.conf" "Configuration pour tests Ubuntu"
print_file "configs/cgi_test.conf" "Configuration spécifique CGI"

print_section "FICHIERS WEB"
print_file "www/index.html" "Page d'accueil du serveur"
print_file "www/upload.html" "Interface d'upload de fichiers"
print_file "www/api-status.html" "Page de statut de l'API"
print_file "www/cgi-bin/index.py" "Script CGI Python principal"

echo ""
echo "💡 RECOMMANDATIONS POUR L'ÉVALUATION:"
echo "   1. Lisez d'abord: README_EVALUATION.md"
echo "   2. Exécutez: bash demonstration_finale.sh"
echo "   3. Tests complets: bash evaluation_tests.sh"
echo "   4. Validation: bash final_validation.sh"
echo "   5. CGI details: Lina.md"
echo ""
echo "🎯 Tous les fichiers sont prêts pour l'évaluation École 42 !"
echo ""
