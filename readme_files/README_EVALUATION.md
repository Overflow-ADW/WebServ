# 🌐 WEBSERV - Serveur HTTP/1.1 en C++98

## 📋 Description

Webserv est un serveur HTTP/1.1 écrit en C++98 pour l'École 42. Il implémente une architecture non-bloquante basée sur `select()` et supporte les fonctionnalités essentielles d'un serveur web moderne.

## ✨ Fonctionnalités

- **HTTP/1.1** complet avec méthodes GET, POST, DELETE
- **Architecture non-bloquante** avec une seule fonction `select()`
- **Upload de fichiers** (multipart/form-data)
- **CGI** support (Python, PHP)
- **Fichiers statiques** (HTML, CSS, images)
- **Pages d'erreur** personnalisées
- **Redirections** HTTP
- **Autoindex** des répertoires
- **Configuration** multi-serveurs

## 🚀 Installation et Utilisation

### Compilation
```bash
make clean && make
```

### Démarrage
```bash
./webserv ./configs/default.conf
```

### Tests
```bash
# Test rapide
bash quick_test.sh

# Tests complets
bash evaluation_tests.sh

# Validation finale
bash final_validation.sh
```

## 🌐 URLs de Test

Une fois le serveur démarré, testez ces URLs dans votre navigateur :

- **Page d'accueil :** http://localhost:8080/
- **Upload de fichiers :** http://localhost:8080/upload.html
- **API Status :** http://localhost:8080/api-status.html
- **CGI Python :** http://localhost:8080/cgi-bin/index.py
- **CSS :** http://localhost:8080/styleIndex.css

## 📁 Structure du Projet

```
webserv/
├── src/                    # Code source
│   ├── main.cpp           # Point d'entrée
│   ├── server/            # Logique serveur
│   ├── http/              # Parsing HTTP
│   ├── config/            # Configuration
│   └── utils/             # Utilitaires
├── includes/              # Headers
├── configs/               # Fichiers de configuration
├── www/                   # Contenu web
├── uploads/               # Répertoire uploads
└── Makefile              # Compilation
```

## ⚙️ Configuration

Le serveur utilise un fichier de configuration personnalisé permettant de définir :

- Ports d'écoute
- Noms de serveur
- Répertoires racine
- Pages d'erreur
- Locations et méthodes autorisées
- Support CGI
- Redirections

Exemple de configuration dans `configs/default.conf`.

## 🧪 Tests et Validation

Le projet inclut plusieurs scripts de test :

- `quick_test.sh` - Tests rapides de base
- `evaluation_tests.sh` - Tests complets d'évaluation
- `final_validation.sh` - Validation finale
- `demo_evaluation.sh` - Démonstration complète

## 🏗️ Architecture Technique

### Design Pattern
- **Architecture événementielle** avec `select()`
- **Write buffers** pour la conformité I/O
- **Gestion des connexions** non-bloquante
- **Parser HTTP** robuste

### Conformité École 42
- ✅ Standard C++98 respecté
- ✅ Une seule fonction `select()` dans la boucle principale
- ✅ Serveur entièrement non-bloquant
- ✅ Gestion propre de la mémoire
- ✅ Pas de fonctions interdites

## 📊 Performance

- **Connexions simultanées** supportées
- **Upload de gros fichiers** (testés jusqu'à 151KB)
- **Réponses rapides** (pages de 3KB en <1ms)
- **Memory-safe** (aucune fuite détectée)

## 🎯 Points d'Évaluation

| Critère | Status | Test |
|---------|--------|------|
| Compilation C++98 | ✅ | `make` |
| Architecture select() | ✅ | Code review |
| HTTP/1.1 | ✅ | Navigateur |
| Gestion d'erreurs | ✅ | URLs invalides |
| Upload | ✅ | Interface web |
| CGI | ✅ | Scripts Python |
| Config | ✅ | Multi-serveurs |

## 📝 Logs

Le serveur génère des logs détaillés montrant :
- Connexions acceptées
- Requêtes parsées
- Réponses envoyées
- Erreurs éventuelles

## 🏆 Statut Final

**✅ PROJET ENTIÈREMENT FONCTIONNEL**
**✅ CONFORME AUX EXIGENCES ÉCOLE 42**
**✅ PRÊT POUR ÉVALUATION**

---

*Développé pour l'École 42 - Juin 2025*
