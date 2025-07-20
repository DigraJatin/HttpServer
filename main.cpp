// main.cpp
// Entry point for the HTTP server application
// Creates and runs the HTTPServer instance

#include "HTTPServer.h"
#include <iostream>

int main() {
    HTTPServer server(8080); // Create server on port 8080
    if (!server.start()) {   // Start the server (bind, listen)
        return 1;
    }
    std::cout << "Press Ctrl+C to stop the server" << std::endl;
    server.run();           // Run the server loop
    return 0;
}
