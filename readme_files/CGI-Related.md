# 📂 **FICHIERS ET FONCTIONS CGI - PROJET WEBSERV**

## 📁 **FICHIERS CGI DU PROJET**

### 🔧 **Fichiers de Code Source**

#### **1. Fichiers d'en-tête (.hpp)**
```
includes/
├── Server.hpp              # Déclarations des méthodes CGI
├── HttpRequest.hpp         # Parsing des requêtes CGI
├── HttpResponse.hpp        # Génération des réponses CGI
└── ConfigParser.hpp        # Configuration CGI
```

#### **2. Fichiers d'implémentation (.cpp)**
```
src/server/
├── Server.cpp              # Logique principale CGI
├── HttpRequest.cpp         # Parsing requêtes CGI
├── HttpResponse.cpp        # Construction réponses CGI
└── ConfigParser.cpp        # Parsing config CGI
```

### 🌐 **Fichiers CGI Exécutables**

#### **Scripts Python**
```
www/cgi-bin/
├── index.py               # Page principale CGI Python
├── test.py                # Script de test CGI
├── hello.py               # Script "Hello World"
├── form_handler.py        # Gestionnaire de formulaires
└── env_display.py         # Affichage variables d'environnement
```

#### **Scripts PHP** (si disponibles)
```
www/cgi-bin/
├── info.php               # Page d'informations PHP
├── test.php               # Script de test PHP
└── form.php               # Gestionnaire formulaires PHP
```

### ⚙️ **Fichiers de Configuration**

```
configs/
├── default.conf           # Configuration CGI principale
├── ubuntu_test.conf       # Configuration pour tests Ubuntu
└── cgi_test.conf          # Configuration spécifique CGI
```

---

## 🛠️ **FONCTIONS CGI PRINCIPALES**

### **1. Server.cpp - Fonctions CGI Core**

```cpp
// Exécution CGI principale
void executeCgiRequest(int client_fd, const HttpRequest& request, 
                      const std::string& script_path, 
                      const ServerConfig& config, 
                      const Location& location);

// Détection des scripts CGI
bool isCgiRequest(const std::string& path, const Location& location);

// Obtention du chemin de l'interpréteur
std::string getCgiInterpreter(const std::string& extension);

// Construction de l'environnement CGI
void setupCgiEnvironment(const HttpRequest& request, 
                        const std::string& script_path,
                        const ServerConfig& config);

// Gestion des pipes CGI
int setupCgiPipes(int pipe_in[2], int pipe_out[2]);

// Exécution du processus CGI
pid_t executeCgiProcess(const std::string& interpreter,
                       const std::string& script_path,
                       int pipe_in[2], int pipe_out[2]);

// Lecture de la réponse CGI
std::string readCgiResponse(int pipe_out[2]);

// Nettoyage des ressources CGI
void cleanupCgiResources(int pipe_in[2], int pipe_out[2], pid_t pid);
```

### **2. HttpRequest.cpp - Parsing CGI**

```cpp
// Extraction du query string
std::string getQueryString() const;

// Obtention du content type
std::string getContentType() const;

// Récupération du content length
size_t getContentLength() const;

// Accès au body de la requête
const std::string& getBody() const;

// Variables d'environnement CGI
std::map<std::string, std::string> getCgiEnvironment() const;
```

### **3. HttpResponse.cpp - Réponses CGI**

```cpp
// Construction de la réponse CGI
void buildCgiResponse(const std::string& cgi_output);

// Parsing des headers CGI
void parseCgiHeaders(const std::string& headers_section);

// Validation de la sortie CGI
bool isValidCgiOutput(const std::string& output);

// Formatage de la réponse finale
std::string formatCgiResponse();
```

### **4. ConfigParser.cpp - Configuration CGI**

```cpp
// Parsing des extensions CGI
void parseCgiExtensions(const std::string& line, Location& location);

// Configuration du chemin CGI
void parseCgiPath(const std::string& line, Location& location);

// Validation de la configuration CGI
bool validateCgiConfig(const Location& location);
```

---

## 📋 **VARIABLES D'ENVIRONNEMENT CGI**

### **Variables Standard**
```cpp
// Variables CGI selon RFC 3875
setenv("CONTENT_TYPE", content_type.c_str(), 1);
setenv("CONTENT_LENGTH", content_length.c_str(), 1);
setenv("QUERY_STRING", query_string.c_str(), 1);
setenv("REQUEST_METHOD", method.c_str(), 1);
setenv("SCRIPT_NAME", script_name.c_str(), 1);
setenv("PATH_INFO", path_info.c_str(), 1);
setenv("SERVER_NAME", server_name.c_str(), 1);
setenv("SERVER_PORT", server_port.c_str(), 1);
setenv("SERVER_PROTOCOL", "HTTP/1.1", 1);
setenv("GATEWAY_INTERFACE", "CGI/1.1", 1);
```

### **Variables HTTP Headers**
```cpp
// Conversion des headers HTTP en variables CGI
setenv("HTTP_HOST", host.c_str(), 1);
setenv("HTTP_USER_AGENT", user_agent.c_str(), 1);
setenv("HTTP_ACCEPT", accept.c_str(), 1);
setenv("HTTP_COOKIE", cookie.c_str(), 1);
```

---

## 🎯 **FLUX D'EXÉCUTION CGI**

### **1. Détection CGI**
```cpp
Location& location = findLocation(request.getPath());
if (isCgiRequest(request.getPath(), location)) {
    executeCgiRequest(client_fd, request, script_path, config, location);
}
```

### **2. Configuration Pipes**
```cpp
int pipe_in[2], pipe_out[2];
if (pipe(pipe_in) == -1 || pipe(pipe_out) == -1) {
    // Gestion d'erreur
}
```

### **3. Fork et Exec**
```cpp
pid_t pid = fork();
if (pid == 0) {
    // Processus enfant - exécution CGI
    dup2(pipe_in[0], STDIN_FILENO);
    dup2(pipe_out[1], STDOUT_FILENO);
    execve(interpreter.c_str(), argv, environ);
}
```

### **4. Communication**
```cpp
// Envoi du body au script
write(pipe_in[1], request.getBody().c_str(), request.getBody().length());
close(pipe_in[1]);

// Lecture de la réponse
std::string response = readCgiResponse(pipe_out);
```

---

## 🧪 **SCRIPTS CGI DE TEST**

### **index.py** - Page principale
```python
#!/usr/bin/env python3
print("Content-Type: text/html\n")
print("<h1>CGI Python Page</h1>")
```

### **form_handler.py** - Traitement formulaires
```python
#!/usr/bin/env python3
import os
import cgi

form = cgi.FieldStorage()
print("Content-Type: text/html\n")
print(f"<h1>Form Data: {form.getvalue('name')}</h1>")
```

---

## 🔧 **CONFIGURATION CGI**

### **Exemple de configuration dans default.conf**
```nginx
location /cgi-bin {
    allowed_methods GET POST;
    cgi_extensions .py .php;
    cgi_path /usr/bin/python3;
}
```

### **Extensions supportées**
- `.py` → Python scripts
- `.php` → PHP scripts
- `.pl` → Perl scripts (si configuré)
- `.rb` → Ruby scripts (si configuré)

---

## 📊 **GESTION DES ERREURS CGI**

### **Codes d'erreur**
- **500 Internal Server Error** → Script CGI défaillant
- **502 Bad Gateway** → Interpréteur non trouvé
- **504 Gateway Timeout** → Script trop lent
- **403 Forbidden** → Permissions insuffisantes

### **Validation des scripts**
```cpp
// Vérification de l'existence du script
if (!fileExists(script_path)) {
    return error404Response();
}

// Vérification des permissions d'exécution
if (!isExecutable(script_path)) {
    return error403Response();
}

// Vérification de l'interpréteur
if (!interpreterExists(interpreter_path)) {
    return error502Response();
}
```

---

## 🎮 **TESTS CGI DISPONIBLES**

### **Scripts de test inclus**
1. **index.py** → Page d'accueil CGI
2. **env_display.py** → Affichage des variables d'environnement
3. **form_handler.py** → Traitement de formulaires POST
4. **hello.py** → Script "Hello World" simple
5. **test.py** → Script de test basique

### **URLs de test**
- `http://localhost:8080/cgi-bin/index.py`
- `http://localhost:8080/cgi-bin/hello.py`
- `http://localhost:8080/cgi-bin/env_display.py`
- `http://localhost:8080/cgi-bin/form_handler.py`

---

Cette architecture CGI complète permet l'exécution de scripts dynamiques avec une gestion robuste des processus, pipes et variables d'environnement selon le standard **CGI/1.1**.

🎯 **Le système CGI de Webserv est entièrement fonctionnel et conforme aux standards !**
