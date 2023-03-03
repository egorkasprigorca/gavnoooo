#include <iostream>
#include <sys/socket.h>
#include <cstring>
#include <arpa/inet.h>

#define UDP_BUF_SIZE 1357

int main(int argc, char const *argv[]) {
    int socket_fd;
    sockaddr_in server_addr{}, client_addr{};
    auto slen = (socklen_t)sizeof(client_addr);
    char buf[2048];
    int recvlen;
    char server_host[] = "127.0.0.1";
    int server_port = 1721;

    if ((socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        std::cout << "[ERROR] failed to create socket" << '\n';
        return 1;
    }

    client_addr.sin_family = AF_INET;
    client_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(socket_fd, (sockaddr*) &client_addr, sizeof(client_addr)) < 0) {
        std::cout << "[ERROR] failed to bind socket" << '\n';
        return 2;
    }

    std::cout << "[INFO] Client started!" << '\n';

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    if (inet_aton(server_host, &server_addr.sin_addr) == 0) {
        std::cout << "[ERROR] inet_aton() failed" << '\n';
        return 3;
    }

    int i = 0;

    while (true) {
        std::cout << "1 - Send message\n2 - Send file\n3 - Exit\nChoice option: ";
        std::cin.getline (buf, 2048);
        switch (buf[0]) {
            case '1': {
                memset(buf, 0, 2048);
                std::cout << "Print message: ";
                std::cin.getline(buf, 2048);
                std::cout << "[INFO] Sending the message to " << server_host << ":" << ntohs(server_addr.sin_port) << '\n';
                if (sendto (socket_fd, buf, 2048, 0, (sockaddr*) &server_addr, slen) == -1) {
                    std::cout << "[ERROR] Failed to send the message" << '\n';
                    return -1;
                }
                continue;
            }
            case '2': {
                FILE *fp;
                size_t bufsz;
                fp = fopen("test.jpg", "r");
                fseek(fp, 0, SEEK_END);
                bufsz = ftell(fp);
                fseek(fp, 0, SEEK_SET);
                printf("File size: %li bytes\n", bufsz);
                char *buff = (char*)malloc(sizeof(char) * bufsz);
                fread(buff, sizeof(char), bufsz, fp);
                fclose(fp);

                char *from = &buff[0];
                size_t remainingToSend = bufsz;
                while (remainingToSend > 0) {
                    size_t sendSize =
                            remainingToSend > UDP_BUF_SIZE ? UDP_BUF_SIZE : remainingToSend;
                    while (sendto(socket_fd, (const char *)from, sendSize, 0,
                                  (const struct sockaddr *)&server_addr,
                                  sizeof(server_addr)) < 0) { // Sending packet per packet
                        printf("Error during send, retrying...\n");
                    }
                    remainingToSend -= sendSize;
                    from += sendSize;
                }
                char buffer[] = "end";
                sendto(socket_fd, buffer, sizeof(buffer), 0, (const struct sockaddr *)&server_addr,
                       sizeof(server_addr));
                free(buff);
                continue;
            }
            case '3': {
                return 0;
            }
        }
    }

}