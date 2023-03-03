#include <iostream>
#include <fstream>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <algorithm>
#include <thread>
#include <csignal>
#include <arpa/inet.h>

#define UDP_BUF_SIZE 1357

std::thread threads[100];                   // at max 100 threads
int threadno = 0;
int socket_fd;

void* file_routine (void*);

int main(int argc, char const *argv[]) {
    sockaddr_in server_addr{};
    sockaddr_in client_addr{};
    int server_port = 1721;

    socklen_t client_addr_len = sizeof(client_addr);
    ssize_t n;
    char buf[UDP_BUF_SIZE];
    memset(buf, 0,UDP_BUF_SIZE);

    if ((socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        std::cout << "[ERROR] failed to create socket" << '\n';
        return 1;
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl (INADDR_ANY);
    server_addr.sin_port = htons (server_port);

    if (bind(socket_fd, (sockaddr*) &server_addr, sizeof(server_addr)) == -1) {
        std::cout << "[ERROR] failed to bind socket"  << '\n';
        return 0;
    }

    std::cout << "[INFO] Server is listening on " << server_port << " port \n";

    std::ofstream file("server6.jpg");
    n = recvfrom(socket_fd, buf, UDP_BUF_SIZE, 0, (struct sockaddr *) &client_addr, &client_addr_len);
    while (true) {
        if (n < 0) {
            break;
        }
        if (strcmp(buf, "end") == 0) {
            break;
        }
        file.write(buf, n);
        memset(buf, 0, UDP_BUF_SIZE);
        n = recvfrom(socket_fd, buf, UDP_BUF_SIZE, 0, (struct sockaddr *) &client_addr, &client_addr_len);
    }
    file.close();
    std::cout << "[INFO] Server closed" << '\n';
}
