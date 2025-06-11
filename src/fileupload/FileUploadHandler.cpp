#include "FileUploadHandler.hpp"

FileUploadHandler::FileUploadHandler() {
}

FileUploadHandler::~FileUploadHandler() {
}

bool FileUploadHandler::isFileUpload(const HttpRequest& request, const LocationConfig& location_config) {
    return (request.getMethod() == "POST" && !location_config.upload_path.empty());
}

bool FileUploadHandler::handleFileUpload(const HttpRequest& request, const LocationConfig& location_config, std::string& response_html) {
    try {
        std::string content_type = request.getHeader("content-type");
        if (content_type.find("multipart/form-data") == std::string::npos) {
            response_html = "<html><body><h1>400 Bad Request</h1><p>Expected multipart/form-data</p></body></html>";
            return false;
        }
        
        std::string boundary = extractBoundary(content_type);
        if (boundary.empty()) {
            response_html = "<html><body><h1>400 Bad Request</h1><p>Missing boundary in multipart data</p></body></html>";
            return false;
        }
        
        const std::string& body = request.getBody();
        if (body.empty()) {
            response_html = "<html><body><h1>400 Bad Request</h1><p>Empty request body</p></body></html>";
            return false;
        }
        
        std::vector<std::string> uploaded_files;
        if (!parseMultipartData(body, boundary, location_config.upload_path, uploaded_files)) {
            response_html = "<html><body><h1>500 Internal Server Error</h1><p>Failed to process upload</p></body></html>";
            return false;
        }
        
        if (uploaded_files.empty()) {
            response_html = "<html><body><h1>400 Bad Request</h1><p>No files uploaded</p></body></html>";
            return false;
        }
        
        response_html = "<html><body><h1>Upload Successful</h1><p>Files uploaded:</p><ul>";
        for (size_t i = 0; i < uploaded_files.size(); ++i)
            response_html += "<li>" + uploaded_files[i] + "</li>";
        response_html += "</ul></body></html>";
        
        std::cout << GREEN << "Upload successful: " << uploaded_files.size() << " file(s)" << RESET << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        response_html = "<html><body><h1>500 Internal Server Error</h1><p>" + std::string(e.what()) + "</p></body></html>";
        return false;
    }
}

std::string FileUploadHandler::extractBoundary(const std::string& content_type) {
    size_t boundary_pos = content_type.find("boundary=");
    if (boundary_pos == std::string::npos)
        return "";
    
    std::string boundary = content_type.substr(boundary_pos + 9);
    
    if (!boundary.empty() && boundary[0] == '"' && boundary[boundary.length() - 1] == '"')
        boundary = boundary.substr(1, boundary.length() - 2);
    
    return boundary;
}

bool FileUploadHandler::parseMultipartData(const std::string& body, const std::string& boundary, const std::string& upload_path, std::vector<std::string>& uploaded_files) {
    std::string delimiter = "--" + boundary;
    
    size_t pos = 0;
    while ((pos = body.find(delimiter, pos)) != std::string::npos) {
        pos += delimiter.length();
        
        if (body.substr(pos, 2) == "--")
            break;
        
        if (pos < body.length() && body[pos] == '\r') 
            pos++;
        if (pos < body.length() && body[pos] == '\n') 
            pos++;
        
        size_t headers_end = body.find("\r\n\r\n", pos);
        if (headers_end == std::string::npos)
            continue;
        
        std::string headers = body.substr(pos, headers_end - pos);
        
        std::string filename;
        size_t disp_pos = headers.find("Content-Disposition:");
        if (disp_pos != std::string::npos) {
            size_t filename_pos = headers.find("filename=\"", disp_pos);
            if (filename_pos != std::string::npos) {
                filename_pos += 10;
                size_t filename_end = headers.find("\"", filename_pos);
                if (filename_end != std::string::npos)
                    filename = headers.substr(filename_pos, filename_end - filename_pos);
            }
        }
        
        if (body.find("\r\n\r\n", pos) == headers_end)
            pos = headers_end + 4;
        else
            pos = headers_end + 2;
        
        size_t content_end = body.find("\r\n--" + boundary, pos);
        if (content_end == std::string::npos) {
            content_end = body.find("\n--" + boundary, pos);
            if (content_end == std::string::npos)
                break;
        }
        
        std::string file_content = body.substr(pos, content_end - pos);
        
        if (!filename.empty()) {
            if (file_content.empty())
                std::cout << YELLOW << "Warning: Empty file content for " << filename << RESET << std::endl;
            if (saveUploadedFile(filename, file_content, upload_path))
                uploaded_files.push_back(filename);
        }
        
        pos = content_end;
    }
    
    return !uploaded_files.empty();
}

bool FileUploadHandler::saveUploadedFile(const std::string& filename, const std::string& content, const std::string& upload_path) {
    std::string file_path = upload_path + "/" + filename;
    
    std::ofstream file(file_path.c_str(), std::ios::binary);
    if (!file.is_open()) {
        std::cerr << RED << "Error: Could not open file for writing: " << file_path << RESET << std::endl;
        return false;
    }
    
    file.write(content.c_str(), content.length());
    file.close();
    
    if (file.fail()) {
        std::cerr << RED << "Error: Failed to write file: " << file_path << RESET << std::endl;
        return false;
    }
    
    std::cout << GREEN << "File uploaded successfully: " << file_path << RESET << std::endl;
    return true;
}
