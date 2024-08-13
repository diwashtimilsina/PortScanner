#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include <regex>
#include <netdb.h>
#include <fstream>
#include <boost/thread.hpp>

using namespace std;

// Function to validate a URL using regular expressions
bool isValidURL(const string &url) {
    const regex urlRegex("([a-zA-Z0-9.-]+)\\.([a-zA-Z]{2,6})(:[0-9]+)?(/[a-zA-Z0-9_.-]*)*$");
    return regex_match(url, urlRegex);
}

// Function to resolve a hostname to an IP address
bool resolveHostname(const string& hostname, string& ip_address) {
    struct hostent *he;
    struct in_addr **addr_list;

    he = gethostbyname(hostname.c_str());
    if (he == NULL) {
        return false;
    }

    addr_list = (struct in_addr **) he->h_addr_list;
    ip_address = inet_ntoa(*addr_list[0]);

    return true;
}

// Function to scan ports concurrently
void scanPort(const string& ipAddress, int port, vector<pair<int, string>>& results, boost::mutex& mutex) {
    struct timeval timeout;
    timeout.tv_sec = 1; // Timeout set to 1 second
    timeout.tv_usec = 0;

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(ipAddress.c_str());
    server.sin_port = htons(port);

    int nSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (nSocket == -1) {
        boost::mutex::scoped_lock lock(mutex);
        results.push_back({port, "Socket not created"});
        return;
    }

    if (setsockopt(nSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0) {
        boost::mutex::scoped_lock lock(mutex);
        results.push_back({port, "Failed to set socket timeout"});
        close(nSocket);
        return;
    }

    cout << "Scanning port: " << port << endl;

    int n = connect(nSocket, (struct sockaddr *)&server, sizeof(server));
    if (n == 0) {
        boost::mutex::scoped_lock lock(mutex);
        results.push_back({port, "Open"});
    } else {
        boost::mutex::scoped_lock lock(mutex);
        results.push_back({port, "Closed"});
    }

    close(nSocket);
}

// Function to handle HTTP requests
string handleHttpRequest(const string& request) {
    string url = "/";
    size_t start = request.find("GET /scan?url=");
    if (start != string::npos) {
        start += 14;
        size_t end = request.find(" HTTP/1.1", start);
        if (end != string::npos) {
            url = request.substr(start, end - start);
        }
    }

    if (!isValidURL(url)) {
        return "HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\n\r\nInvalid URL";
    }

    string ipAddress;
    if (!resolveHostname(url, ipAddress)) {
        return "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\nFailed to resolve hostname";
    }

    cout << "Resolved " << url << " to IP address: " << ipAddress << endl;
    
    vector<int> ports = {21, 22, 23, 25, 53, 80, 110, 143, 443, 587, 993, 995};
    vector<pair<int, string>> results;
    boost::mutex mutex;
    vector<boost::thread> threads;

    for (int port : ports) {
        threads.push_back(boost::thread(scanPort, ipAddress, port, boost::ref(results), boost::ref(mutex)));
    }

    for (auto& thread : threads) {
        thread.join();
    }

    // Format the response in JSON
    string response = "[";
    for (size_t i = 0; i < results.size(); ++i) {
        response += "{\"port\":" + to_string(results[i].first) + ",\"status\":\"" + results[i].second + "\"}";
        if (i < results.size() - 1) {
            response += ",";
        }
    }
    response += "]";

    return "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n" + response;
}

// Function to handle HTTP connections
void handleHttpConnection(int clientSocket) {
    char buffer[1024] = {0};
    int valread = read(clientSocket, buffer, 1024);

    string httpRequest(buffer);
    string httpResponse;

    if (httpRequest.find("GET / ") == 0 || httpRequest.find("GET /index.html ") == 0) {
        ifstream htmlFile("index.html");
        if (htmlFile.is_open()) {
            ostringstream oss;
            oss << htmlFile.rdbuf();
            htmlFile.close();
            httpResponse = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" + oss.str();
        } else {
            httpResponse = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\nFailed to open index.html";
        }
    } else if (httpRequest.find("GET /scan?url=") == 0) {
        httpResponse = handleHttpRequest(httpRequest);
    } else {
        httpResponse = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\n\r\nInvalid request";
    }

    send(clientSocket, httpResponse.c_str(), httpResponse.length(), 0);
    close(clientSocket);
}

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr;
    struct sockaddr_storage serverStorage;
    socklen_t addr_size;

    serverSocket = socket(PF_INET, SOCK_STREAM, 0);

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8000);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

    bind(serverSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
    if (listen(serverSocket, 50) == 0) {
        cout << "Server listening on port 8000" << endl;
    } else {
        cerr << "Error in starting server" << endl;
        return 1;
    }

    while (true) {
        addr_size = sizeof serverStorage;
        clientSocket = accept(serverSocket, (struct sockaddr *) &serverStorage, &addr_size);
        handleHttpConnection(clientSocket);
    }

    return 0;
}

