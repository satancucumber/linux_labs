#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>

#define PORT 8080

#define BUFFER_SIZE 1024

#define CONNECTION_TIMEOUT 10

std::string generate_random_sequence(int count)
{
    std::srand(std::time(nullptr));
    std::string sequence;
    for (int i = 0; i < count; ++i)
    {
        sequence += std::to_string(std::rand() % 100);
        if (i < count - 1)
        {
            sequence += ",";
        }
    }
    return sequence;
}

int main(int argc, char *argv[])
{
    int timeout = std::stoi(argv[1]);
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    server_addr.sin_addr.s_addr= htonl(INADDR_ANY);

    auto beginTime = time(NULL); 
    int isConnected;
    while (((time(NULL) - beginTime )< timeout) 
    &&  (isConnected =  connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr))) < 0);
   
    

    timeval tv;
   
    if (isConnected<0 ) 
        {
                std::cout << "Connection attempt timed out after " << timeout << " seconds." << std::endl;
                close(client_socket);
                return 1;
        }


    std::string random_sequence = generate_random_sequence(10);
    std::cout << "Generated sequence: " << random_sequence << std::endl;

    
   
    send(client_socket, random_sequence.c_str(), random_sequence.size(), 0);

    fd_set write_fds;
    FD_ZERO(&write_fds);
    FD_SET(client_socket, &write_fds);

   
    tv.tv_sec = timeout;
    tv.tv_usec = 0;
   int select_result = select(client_socket + 1, nullptr, &write_fds, nullptr, &tv);
    if (select_result == 0 ){
         std::cout << "Timeout recieved" << std::endl;
    }else{
        char buffer[BUFFER_SIZE] = {0};
        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (bytes_received > 0)
        {
            std::cout << "Sorted sequence from server: " << buffer << std::endl;
        }
        else
        {
            std::cout << "Failed to receive sorted sequence" << std::endl;
        }
    }
    close(client_socket);
    return 0;
}
