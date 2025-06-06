#!/bin/bash

echo "🧪 Configuration automatique pour les tests Ubuntu de l'école..."

cd /home/fvastena/Desktop/webserv

# Vérifier que YoupiBanane existe déjà (sinon la créer)
if [ ! -d "YoupiBanane" ]; then
    echo "📁 Création de la structure YoupiBanane..."
    mkdir -p YoupiBanane/nop YoupiBanane/Yeah
    
    # Créer les fichiers requis avec du contenu basique
    echo "Content of youpi.bad_extension" > YoupiBanane/youpi.bad_extension
    echo "Content of youpi.bla for CGI processing" > YoupiBanane/youpi.bla
    echo "Content of youpi.bad_extension in nop" > YoupiBanane/nop/youpi.bad_extension
    echo "Content of other.pouic in nop" > YoupiBanane/nop/other.pouic
    echo "Content of not_happy.bad_extension in Yeah" > YoupiBanane/Yeah/not_happy.bad_extension
    
    echo "✅ Structure YoupiBanane créée"
else
    echo "✅ Structure YoupiBanane déjà présente"
fi

# Vérifier que cgi_test existe et est exécutable
if [ ! -x "cgi_test" ]; then
    echo "❌ cgi_test manquant ou non exécutable"
    echo "ℹ️  Le fichier cgi_test a été créé, vérifiez qu'il est exécutable"
    chmod +x cgi_test
fi

# Vérifier que le répertoire uploads existe
if [ ! -d "uploads" ]; then
    mkdir -p uploads
    echo "📁 Répertoire uploads créé"
fi

# Vérifier les permissions des testeurs
chmod +x test/ubuntu_tester 2>/dev/null || echo "⚠️  Testeur ubuntu_tester introuvable"
chmod +x test/ubuntu_cgi_tester 2>/dev/null || echo "⚠️  Testeur ubuntu_cgi_tester introuvable"

echo ""
echo "📋 Résumé de la configuration :"
echo "  🔧 Configuration : configs/ubuntu_test.conf"
echo "  📁 YoupiBanane : $(ls -la YoupiBanane/ 2>/dev/null | wc -l || echo '0') fichiers"
echo "  🔧 cgi_test : $(test -x cgi_test && echo 'OK' || echo 'MANQUANT')"
echo "  📁 uploads : $(test -d uploads && echo 'OK' || echo 'MANQUANT')"
echo ""
echo "✅ Configuration terminée ! Vous pouvez maintenant lancer :"
echo "   ./webserv configs/ubuntu_test.conf"
echo ""
echo "Et dans un autre terminal :"
echo "   ./test/ubuntu_tester http://localhost:8080"
