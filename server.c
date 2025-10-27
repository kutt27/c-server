#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT     8080
#define BACKLOG  10

int main(void)
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);

    // 1. Create socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Set up the address structure
    address.sin_family      = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;   // Listen on all interfaces
    address.sin_port        = htons(PORT);

    // 2. Bind the socket to the port
    if (bind(server_fd, (struct sockaddr *)&address, addrlen) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // 3. Listen for connections
    if (listen(server_fd, BACKLOG) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    while (1) {
        // 4. Accept an incoming connection (blocking)
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen)) < 0) {
            perror("Accept failed");
            continue;
        }

        char buffer[1024] = {0};

        // 5. Read the request from the client
        read(new_socket, buffer, 1023);
        printf("Request received\n%s\n", buffer);

        // 6. Build and send the HTTP response
        const char *body = "<h1>Hello from the C Server!</h1>"
                           "<p>The C backend is operational.</p>";
        char response[2048];
        snprintf(response, sizeof(response),
                 "HTTP/1.1 200 OK\r\n"
                 "Content-Type: text/html\r\n"
                 "Content-Length: %zu\r\n"
                 "\r\n"
                 "%s", strlen(body), body);

        write(new_socket, response, strlen(response));

        // 7. Close the client socket
        close(new_socket);
    }
    return 0;
}
