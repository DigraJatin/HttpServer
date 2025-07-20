// HTTPServer.h
// Declaration of the HTTPServer class
// Handles HTTP server logic, client connections, and request processing

#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <string>
#include <netinet/in.h>

class HTTPServer {
private:
    int server_fd; // Server socket file descriptor
    struct sockaddr_in address; // Server address structure
    int port; // Port number

    std::string getContentType(const std::string& filename); // Determines MIME type
    std::string readFile(const std::string& filepath); // Reads file content
    std::string parseRequest(const std::string& request); // Parses HTTP request
    std::string createResponse(const std::string& path); // Creates HTTP response
    void handleClient(int client_socket); // Handles a single client connection

public:
    HTTPServer(int port);
    ~HTTPServer();
    bool start(); // Starts the server (bind, listen)
    void run();   // Accepts and handles clients
};

#endif // HTTPSERVER_H
