#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>
#include <sstream>
#include <fstream>

class HTTPServer {
private:
    int server_fd;
    struct sockaddr_in address;
    int port;
    
    std::string getContentType(const std::string& filename) {
        if (filename.ends_with(".html")) return "text/html";
        if (filename.ends_with(".css")) return "text/css";
        if (filename.ends_with(".js")) return "application/javascript";
        if (filename.ends_with(".json")) return "application/json";
        if (filename.ends_with(".png")) return "image/png";
        if (filename.ends_with(".jpg") || filename.ends_with(".jpeg")) return "image/jpeg";
        return "text/plain";
    }
    
    std::string readFile(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return "";
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }
    
    std::string parseRequest(const std::string& request) {
        std::istringstream iss(request);
        std::string method, path, version;
        iss >> method >> path >> version;
        
        if (method != "GET") {
            return "/405"; // Method not allowed
        }
        
        if (path == "/") {
            path = "/index.html";
        }
        
        return path;
    }
    
    std::string createResponse(const std::string& path) {
        std::string response;
        std::string body;
        std::string contentType = "text/html";
        int statusCode = 200;
        std::string statusText = "OK";
        
        if (path == "/405") {
            statusCode = 405;
            statusText = "Method Not Allowed";
            body = "<html><body><h1>405 Method Not Allowed</h1></body></html>";
        } else {
            // Remove leading slash for file path
            std::string filepath = path.substr(1);
            
            // Simple security check - prevent directory traversal
            if (filepath.find("..") != std::string::npos) {
                statusCode = 403;
                statusText = "Forbidden";
                body = "<html><body><h1>403 Forbidden</h1></body></html>";
            } else {
                body = readFile(filepath);
                
                if (body.empty()) {
                    // File not found or couldn't read
                    statusCode = 404;
                    statusText = "Not Found";
                    body = "<html><body><h1>404 Not Found</h1><p>The requested file was not found.</p></body></html>";
                } else {
                    contentType = getContentType(filepath);
                }
            }
        }
        
        // Build HTTP response
        response = "HTTP/1.1 " + std::to_string(statusCode) + " " + statusText + "\r\n";
        response += "Content-Type: " + contentType + "\r\n";
        response += "Content-Length: " + std::to_string(body.length()) + "\r\n";
        response += "Connection: close\r\n";
        response += "\r\n";
        response += body;
        
        return response;
    }
    
    void handleClient(int client_socket) {
        char buffer[4096] = {0};
        int bytes_read = read(client_socket, buffer, 4096);
        
        if (bytes_read > 0) {
            std::string request(buffer);
            std::cout << "Request received:\n" << request << std::endl;
            
            std::string path = parseRequest(request);
            std::string response = createResponse(path);
            
            send(client_socket, response.c_str(), response.length(), 0);
        }
        
        close(client_socket);
    }
    
public:
    HTTPServer(int port) : port(port) {
        server_fd = 0;
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port);
    }
    
    ~HTTPServer() {
        if (server_fd > 0) {
            close(server_fd);
        }
    }
    
    bool start() {
        // Create socket
        server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd == 0) {
            std::cerr << "Failed to create socket" << std::endl;
            return false;
        }
        
        // Set socket options
        int opt = 1;
        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
            std::cerr << "Failed to set socket options" << std::endl;
            return false;
        }
        
        // Bind socket
        if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
            std::cerr << "Failed to bind socket" << std::endl;
            return false;
        }
        
        // Listen for connections
        if (listen(server_fd, 10) < 0) {
            std::cerr << "Failed to listen on socket" << std::endl;
            return false;
        }
        
        std::cout << "Server started on port " << port << std::endl;
        std::cout << "Access it at: http://localhost:" << port << std::endl;
        
        return true;
    }
    
    void run() {
        while (true) {
            struct sockaddr_in client_addr;
            socklen_t client_len = sizeof(client_addr);
            
            int client_socket = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
            if (client_socket < 0) {
                std::cerr << "Failed to accept connection" << std::endl;
                continue;
            }
            
            std::cout << "New connection from " << inet_ntoa(client_addr.sin_addr) 
                      << ":" << ntohs(client_addr.sin_port) << std::endl;
            
            // Handle client in separate thread
            std::thread client_thread(&HTTPServer::handleClient, this, client_socket);
            client_thread.detach();
        }
    }
};

int main() {
    HTTPServer server(8080);
    
    if (!server.start()) {
        return 1;
    }
    
    std::cout << "Press Ctrl+C to stop the server" << std::endl;
    server.run();
    
    return 0;
}