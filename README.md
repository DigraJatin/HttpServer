# Build instructions for the C++ HTTP Server

## How to build

1. Make sure you have g++ (C++20 or newer) installed.
2. In the project directory, run:

```
g++ -std=c++20 -pthread main.cpp HTTPServer.cpp -o server
```

## How to run

```
./server
```

The server will start on port 8080. Open your browser and go to http://localhost:8080/

## Project structure
- main.cpp         // Entry point
- HTTPServer.h     // HTTPServer class declaration
- HTTPServer.cpp   // HTTPServer class implementation
- index.html       // Default web page
