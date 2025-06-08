# 🌐 **HTTP CORE & SERVER - PROJET WEBSERV**

## 📁 **FICHIERS HTTP CORE DU PROJET**

### 🔧 **Fichiers de Code Source**

#### **1. Fichiers d'en-tête (.hpp)**
```
includes/
├── Server.hpp              # Architecture serveur principal
├── Socket.hpp              # Gestion des sockets TCP
├── HttpRequest.hpp         # Parsing des requêtes HTTP
├── HttpResponse.hpp        # Génération des réponses HTTP
└── webserv.hpp             # Headers globaux et définitions
```

#### **2. Fichiers d'implémentation (.cpp)**
```
src/
├── main.cpp                # Point d'entrée de l'application
├── server/Server.cpp       # Logique serveur HTTP/1.1
├── socket/Socket.cpp       # Implémentation sockets
├── http/HttpRequest.cpp    # Parser requêtes HTTP
└── http/HttpResponse.cpp   # Constructeur réponses HTTP
```

### 🌐 **Pages Web et Assets**

#### **Interface Web**
```
www/
├── index.html              # Page d'accueil principale
├── styleIndex.css          # Styles CSS modernes
├── about.html              # Page à propos
├── contact.html            # Formulaire de contact
├── upload.html             # Interface d'upload de fichiers
└── error/                  # Pages d'erreur personnalisées
    ├── 404.html
    ├── 500.html
    └── 503.html
```

---

## 🛠️ **FONCTIONS HTTP CORE PRINCIPALES**

### **1. Server.cpp - Architecture Non-Blocking**

```cpp
// Boucle principale du serveur
void Server::run() {
    while (_running) {
        // Initialisation des fd_sets
        FD_ZERO(&_read_fds);
        FD_ZERO(&_write_fds);
        
        // Configuration des sockets d'écoute
        setupFileDescriptors();
        
        // select() avec timeout
        int activity = select(_max_fd + 1, &_read_fds, &_write_fds, NULL, &timeout);
        
        // Traitement des événements
        handleSocketEvents(activity);
    }
}

// Configuration des sockets d'écoute
void Server::setupSockets() {
    for (size_t i = 0; i < _configs.size(); ++i) {
        const ServerConfig& config = _configs[i];
        Socket* socket = new Socket(config.port, config.host);
        _sockets.push_back(socket);
    }
}

// Gestion des nouvelles connexions
void Server::handleNewConnection(Socket& listening_socket) {
    int client_fd = listening_socket.acceptConnection();
    if (client_fd > 0) {
        Socket* client_socket = new Socket(client_fd);
        _client_sockets[client_fd] = client_socket;
    }
}

// Traitement des requêtes clients
void Server::handleClientRequest(int client_fd) {
    char buffer[4096];
    ssize_t bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    
    if (bytes_read > 0) {
        processIncomingData(client_fd, buffer, bytes_read);
    } else if (bytes_read == 0) {
        closeConnection(client_fd);
    }
}

// Système de files d'attente pour l'écriture
void Server::queueResponse(int client_fd, const std::string& response) {
    _write_buffers[client_fd] = response;
    _write_positions[client_fd] = 0;
}

// Écriture non-blocking des réponses
void Server::handlePendingWrites(int client_fd) {
    if (_write_buffers.find(client_fd) == _write_buffers.end()) return;
    
    const std::string& data = _write_buffers[client_fd];
    size_t& position = _write_positions[client_fd];
    
    ssize_t bytes_written = send(client_fd, data.c_str() + position, 
                                data.length() - position, 0);
    
    if (bytes_written > 0) {
        position += bytes_written;
        if (position >= data.length()) {
            _write_buffers.erase(client_fd);
            _write_positions.erase(client_fd);
            closeConnection(client_fd);
        }
    }
}
```

### **2. Socket.cpp - Gestion TCP**

```cpp
// Constructeur socket d'écoute
Socket::Socket(int port, const std::string& host) {
    _fd = socket(AF_INET, SOCK_STREAM, 0);
    
    // Configuration SO_REUSEADDR
    int opt = 1;
    setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    // Configuration non-blocking
    fcntl(_fd, F_SETFL, O_NONBLOCK);
    
    // Binding et listening
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(host.c_str());
    address.sin_port = htons(port);
    
    bind(_fd, (struct sockaddr*)&address, sizeof(address));
    listen(_fd, SOMAXCONN);
}

// Acceptation des connexions
int Socket::acceptConnection() {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    
    int client_fd = accept(_fd, (struct sockaddr*)&client_addr, &client_len);
    
    if (client_fd > 0) {
        // Configuration non-blocking du client
        fcntl(client_fd, F_SETFL, O_NONBLOCK);
    }
    
    return client_fd;
}

// Vérification de l'état du socket
bool Socket::isReady(fd_set& read_fds) const {
    return FD_ISSET(_fd, &read_fds);
}
```

### **3. HttpRequest.cpp - Parser HTTP/1.1**

```cpp
// Parsing principal des requêtes
bool HttpRequest::parseRequest(const std::string& raw_request) {
    // Séparation headers/body
    size_t headers_end = raw_request.find("\r\n\r\n");
    if (headers_end == std::string::npos) {
        headers_end = raw_request.find("\n\n");
    }
    
    // Parsing ligne par ligne
    std::vector<std::string> lines = splitLines(raw_request);
    
    // Request line: GET /path HTTP/1.1
    parseRequestLine(lines[0]);
    
    // Headers
    for (size_t i = 1; i < lines.size() && !lines[i].empty(); ++i) {
        parseHeader(lines[i]);
    }
    
    // Body (si présent)
    extractBody(raw_request, headers_end);
    
    return validateRequest();
}

// Parsing de la ligne de requête
void HttpRequest::parseRequestLine(const std::string& line) {
    std::istringstream iss(line);
    iss >> _method >> _path >> _version;
    
    // Normalisation de la méthode
    std::transform(_method.begin(), _method.end(), _method.begin(), ::toupper);
    
    // Validation de la version HTTP
    if (_version != "HTTP/1.1" && _version != "HTTP/1.0") {
        throw std::runtime_error("Unsupported HTTP version");
    }
}

// Parsing des headers HTTP
void HttpRequest::parseHeader(const std::string& line) {
    size_t colon_pos = line.find(':');
    if (colon_pos == std::string::npos) return;
    
    std::string name = line.substr(0, colon_pos);
    std::string value = line.substr(colon_pos + 1);
    
    // Nettoyage des espaces
    name = trim(name);
    value = trim(value);
    
    // Conversion en minuscules pour la clé
    std::transform(name.begin(), name.end(), name.begin(), ::tolower);
    
    _headers[name] = value;
}

// Getters spécialisés HTTP
size_t HttpRequest::getContentLength() const {
    std::string cl = getHeader("content-length");
    return cl.empty() ? 0 : std::atoi(cl.c_str());
}

std::string HttpRequest::getHost() const {
    return getHeader("host");
}

std::string HttpRequest::getConnection() const {
    return getHeader("connection");
}
```

### **4. HttpResponse.cpp - Génération HTTP/1.1**

```cpp
// Construction de réponse standard
void HttpResponse::setResponse(int status_code, const std::string& content_type, 
                              const std::string& body) {
    _status_code = status_code;
    _status_text = getStatusText(status_code);
    
    setHeader("Content-Type", content_type);
    setHeader("Content-Length", toString(body.length()));
    setHeader("Connection", "close");
    setHeader("Server", "Webserv/1.0");
    
    _body = body;
}

// Service de fichiers statiques
bool HttpResponse::serveFile(const std::string& file_path) {
    std::ifstream file(file_path.c_str(), std::ios::binary);
    if (!file.is_open()) {
        serveErrorPage(404);
        return false;
    }
    
    // Lecture du contenu
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    
    // Détection du type MIME
    std::string mime_type = getMimeType(file_path);
    
    setResponse(200, mime_type, content);
    return true;
}

// Pages d'erreur personnalisées
void HttpResponse::serveErrorPage(int error_code, const std::string& custom_page) {
    if (!custom_page.empty() && fileExists(custom_page)) {
        serveFile(custom_page);
        _status_code = error_code;
        _status_text = getStatusText(error_code);
        return;
    }
    
    // Page d'erreur par défaut
    std::string error_html = generateDefaultErrorPage(error_code);
    setResponse(error_code, "text/html", error_html);
}

// Formatage de la réponse HTTP finale
std::string HttpResponse::toString() const {
    std::ostringstream response;
    
    // Status line
    response << "HTTP/1.1 " << _status_code << " " << _status_text << "\r\n";
    
    // Headers
    for (std::map<std::string, std::string>::const_iterator it = _headers.begin();
         it != _headers.end(); ++it) {
        response << it->first << ": " << it->second << "\r\n";
    }
    
    // Séparateur headers/body
    response << "\r\n";
    
    // Body
    response << _body;
    
    return response.str();
}

// Types MIME supportés
std::string HttpResponse::getMimeType(const std::string& file_path) const {
    std::string extension = getFileExtension(file_path);
    
    if (extension == ".html" || extension == ".htm") return "text/html";
    if (extension == ".css") return "text/css";
    if (extension == ".js") return "application/javascript";
    if (extension == ".json") return "application/json";
    if (extension == ".png") return "image/png";
    if (extension == ".jpg" || extension == ".jpeg") return "image/jpeg";
    if (extension == ".gif") return "image/gif";
    if (extension == ".svg") return "image/svg+xml";
    if (extension == ".pdf") return "application/pdf";
    if (extension == ".txt") return "text/plain";
    
    return "application/octet-stream";
}
```

---

## 🔄 **FLUX DE TRAITEMENT HTTP**

### **1. Acceptation des Connexions**
```cpp
// Dans la boucle principale select()
for (size_t i = 0; i < _sockets.size(); ++i) {
    int fd = _sockets[i]->getFd();
    if (FD_ISSET(fd, &_read_fds)) {
        handleNewConnection(*_sockets[i]);
    }
}
```

### **2. Lecture des Requêtes**
```cpp
// Gestion des requêtes partielles
std::string raw_request;
if (_partial_requests.find(client_fd) != _partial_requests.end()) {
    raw_request = _partial_requests[client_fd];
}

// Lecture non-blocking
ssize_t bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
raw_request.append(buffer, bytes_read);

// Vérification de complétude
if (isRequestComplete(raw_request)) {
    processHttpRequest(client_fd, raw_request);
}
```

### **3. Dispatch des Méthodes**
```cpp
void Server::processHttpRequest(int client_fd, const HttpRequest& request) {
    // Recherche de la configuration serveur
    const ServerConfig* config = findServerConfig(request.getHost());
    const LocationConfig* location = findLocationConfig(*config, request.getPath());
    
    // Vérification des méthodes autorisées
    if (!isMethodAllowed(request.getMethod(), *location)) {
        sendErrorResponse(client_fd, 405, "Method Not Allowed");
        return;
    }
    
    // Dispatch selon la méthode
    if (request.getMethod() == "GET") {
        handleGetRequest(client_fd, request, *config, *location);
    } else if (request.getMethod() == "POST") {
        handlePostRequest(client_fd, request, *config, *location);
    } else if (request.getMethod() == "DELETE") {
        handleDeleteRequest(client_fd, request, *config, *location);
    }
}
```

### **4. Écriture des Réponses**
```cpp
// Système de queue pour conformité select()
void Server::queueResponse(int client_fd, const std::string& response) {
    _write_buffers[client_fd] = response;
    _write_positions[client_fd] = 0;
}

// Écriture progressive non-blocking
void Server::handlePendingWrites(int client_fd) {
    if (!hasDataToWrite(client_fd)) return;
    
    const std::string& data = _write_buffers[client_fd];
    size_t& position = _write_positions[client_fd];
    
    ssize_t bytes_written = send(client_fd, data.c_str() + position, 
                                data.length() - position, 0);
    
    if (bytes_written > 0) {
        position += bytes_written;
        if (position >= data.length()) {
            cleanupClientBuffers(client_fd);
        }
    }
}
```

---

## 📊 **GESTION DES ERREURS HTTP**

### **Codes de Statut Implémentés**
- **200 OK** → Requête traitée avec succès
- **301 Moved Permanently** → Redirection permanente
- **302 Found** → Redirection temporaire
- **400 Bad Request** → Requête malformée
- **403 Forbidden** → Accès interdit
- **404 Not Found** → Ressource introuvable
- **405 Method Not Allowed** → Méthode non autorisée
- **413 Payload Too Large** → Body trop volumineux
- **500 Internal Server Error** → Erreur serveur
- **502 Bad Gateway** → Erreur CGI
- **503 Service Unavailable** → Serveur surchargé

### **Pages d'Erreur Personnalisées**
```cpp
// Configuration dans le fichier de config
server {
    error_page 404 /error/404.html;
    error_page 500 502 503 /error/50x.html;
}

// Génération automatique si pas de page personnalisée
std::string generateDefaultErrorPage(int code) {
    std::ostringstream html;
    html << "<!DOCTYPE html><html><head>";
    html << "<title>" << code << " " << getStatusText(code) << "</title>";
    html << "<style>body{font-family:Arial;text-align:center;margin:50px;}</style>";
    html << "</head><body>";
    html << "<h1>" << code << " " << getStatusText(code) << "</h1>";
    html << "<p>Webserv/1.0 Server</p>";
    html << "</body></html>";
    return html.str();
}
```

---

## 🎯 **FONCTIONNALITÉS AVANCÉES**

### **Upload de Fichiers Multi-part**
```cpp
void Server::handleFileUpload(int client_fd, const HttpRequest& request, 
                             const LocationConfig& location) {
    std::string content_type = request.getHeader("content-type");
    
    if (content_type.find("multipart/form-data") == std::string::npos) {
        sendErrorResponse(client_fd, 400, "Expected multipart/form-data");
        return;
    }
    
    // Extraction du boundary
    std::string boundary = extractBoundary(content_type);
    
    // Parsing des données multipart
    std::vector<std::string> uploaded_files;
    if (parseMultipartData(request.getBody(), boundary, 
                          location.upload_path, uploaded_files)) {
        sendUploadSuccessResponse(client_fd, uploaded_files);
    } else {
        sendErrorResponse(client_fd, 500, "Upload failed");
    }
}
```

### **Gestion des Redirections**
```cpp
// Configuration des redirections
location /old-page {
    return 301 /new-page;
}

// Implémentation
if (!location.redirect_url.empty()) {
    HttpResponse response;
    response.setStatus(location.redirect_code, "");
    response.setHeader("Location", location.redirect_url);
    queueResponse(client_fd, response.toString());
}
```

### **Virtual Hosts**
```cpp
const ServerConfig* Server::findServerConfig(const std::string& host, int port) const {
    // Recherche par host:port exact
    for (size_t i = 0; i < _configs.size(); ++i) {
        if (_configs[i].server_name == host && _configs[i].port == port) {
            return &_configs[i];
        }
    }
    
    // Fallback sur le premier serveur du port
    for (size_t i = 0; i < _configs.size(); ++i) {
        if (_configs[i].port == port) {
            return &_configs[i];
        }
    }
    
    return NULL;
}
```

---

## 🧪 **TESTS ET VALIDATION**

### **Tests de Conformité HTTP/1.1**
- **Telnet manuel** : `telnet localhost 8080`
- **Curl avancé** : `curl -v -H "Host: example.com" http://localhost:8080/`
- **Comparaison NGINX** : Comportement identique attendu
- **Tests de charge** : `siege -c 100 -t 60s http://localhost:8080/`

### **Scénarios de Test**
1. **Requêtes simples** GET/POST/DELETE
2. **Headers malformés** et gestion d'erreurs
3. **Connexions simultanées** multiples
4. **Upload de fichiers** binaires volumineux
5. **Timeouts** et fermetures de connexion
6. **Pages d'erreur** personnalisées
7. **Virtual hosts** et redirections

---

Cette architecture HTTP/1.1 robuste assure une **conformité stricte aux standards** avec une **performance optimale** grâce à l'architecture non-blocking basée sur `select()`.

🚀 **Le cœur HTTP de Webserv est prêt pour la production !**
