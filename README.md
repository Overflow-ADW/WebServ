# WebServ

A small HTTP server written from scratch in **C++98** as part of the 42 curriculum.

The goal is not to replace a production web server such as Nginx. The point is to understand what happens underneath it: sockets, HTTP parsing, routing, responses, uploads, CGI execution and configuration.

## Highlights

- TCP socket creation and connection handling
- HTTP request parsing and response generation
- Configurable virtual servers and locations
- `GET`, `POST` and `DELETE` handling
- Custom error pages
- Request body size limits
- File uploads
- CGI execution
- Redirects
- Directory auto-index support
- Multiple server blocks / listening ports

## Request flow

```mermaid
flowchart LR
    A[Client] --> B[TCP socket]
    B --> C[HTTP request parser]
    C --> D{Route / location}
    D --> E[Static file]
    D --> F[Upload handler]
    D --> G[CGI process]
    E --> H[Response builder]
    F --> H
    G --> H
    H --> I[HTTP response]
    I --> A
```

## Project structure

```text
WebServ/
├── configs/         # Server configuration files
├── includes/        # C++ interfaces
├── src/
│   ├── cgi/
│   ├── config/
│   ├── fileupload/
│   ├── http/
│   ├── response/
│   ├── server/
│   └── utils/
├── www/             # Example web root
└── www_example/     # Additional virtual-server content
```

## Build

Requirements: a C++ compiler with C++98 support, GNU Make and a Unix-like environment.

```bash
make
```

Then start the server with a configuration file:

```bash
./webserv configs/default.conf
```

The included example configuration exposes servers on ports `8080`, `8081` and `8082`.

## Example configuration

```nginx
server {
    listen 8080;
    server_name localhost;
    root ./www;
    index index.html;

    location / {
        allowed_methods GET POST;
        autoindex off;
    }

    location /cgi-bin {
        allowed_methods GET POST;
        cgi_extension .py;
        cgi_path /usr/bin/python3;
    }
}
```

## What this project demonstrates

WebServ is primarily a systems and networking project. It required turning a textual protocol into a working server architecture while dealing with malformed input, I/O, configuration, resource management and the boundary between the server and external CGI programs.

---

Part of my developer portfolio: **[github.com/Overflow-ADW](https://github.com/Overflow-ADW)**  
Professional work: **[Avenue du Web](https://avenueduweb.be)**
