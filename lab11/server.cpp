#include <iostream>
#include <vector>
#include <algorithm>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/wait.h>

#define PORT 8080

#define BUFFER_SIZE 1024


void handle_client(int client_socket)
{
    char buffer[BUFFER_SIZE] = {0};

    int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
    if (bytes_received <= 0)
    {
        close(client_socket);
        return;
    }

    std::cout << "Received sequence: " << buffer << std::endl;


    std::vector<int> numbers;
    char *token = strtok(buffer, ",");
    while (token != nullptr)
    {
        numbers.push_back(std::stoi(token));
        token = strtok(nullptr, ",");
    }
    std::sort(numbers.begin(), numbers.end());

    std::string sorted_numbers;
    for (size_t i = 0; i < numbers.size(); ++i)
    {
        sorted_numbers += std::to_string(numbers[i]);
        if (i < numbers.size() - 1)
        {
            sorted_numbers += ",";
        }
    }
    send(client_socket, sorted_numbers.c_str(), sorted_numbers.size(), 0);
    std::cout << "Sent sorted sequence: " << sorted_numbers << std::endl;

    close(client_socket);
    exit(0);
}

int main(int argc, char * argv[])
{
    int timeout = atoi(argv[1]);
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("Bind failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 5) == -1)
    {
        perror("Listen failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    std::cout << "Server listening on port " << PORT << std::endl;

    while (true)
    {
        fd_set readFds;
        FD_ZERO(&readFds);
        FD_SET(server_socket,&readFds);
        timeval timer;
        timer.tv_sec = timeout;
        timer.tv_usec = 0;

        int select_result=select(server_socket+1,&readFds,NULL,NULL , &timer);
        if (select_result==0){
            std:: cout<< "No clients\n";
            break;
        }
    
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);
        int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);

        if (client_socket < 0)
        {
            perror("Accept failed");
            break;
        }

        if (fork() == 0)
        {
            close(server_socket);
            handle_client(client_socket);
            exit(0);
        }
        else
        {
            close(client_socket);
        }
    }

    close(server_socket);
    return 0;
}