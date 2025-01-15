#include <fstream>
#include <iostream>
#include <cstdlib>
#include <unistd.h>

int main(int argc, char* argv[]) {
    const char* processIdentifier = "CHILD2";
    const char* filePath = argv[0];
    int waitTime = std::atoi(argv[1]);
    
    sleep(waitTime);

    std::ofstream outputFile(filePath, std::ios::app);
    if (!outputFile) {
        std::cerr << "Unable to open file" << std::endl;
        return EXIT_FAILURE;
    }

    pid_t currentPid = getpid();
    outputFile << processIdentifier << " Process ID: " << currentPid << std::endl;
    outputFile << processIdentifier << " Parent ID: " << getppid() << std::endl;
    outputFile << processIdentifier << " Session ID: " << getsid(currentPid) << std::endl;
    outputFile << processIdentifier << " Group ID: " << getpgid(currentPid) << std::endl;
    outputFile << processIdentifier << " Real User ID: " << getuid() << std::endl;
    outputFile << processIdentifier << " Effective User ID: " << getegid() << std::endl;
    outputFile << processIdentifier << " Real Group ID: " << getgid() << std::endl;
    outputFile << processIdentifier << " Effective Group ID: " << getegid() << std::endl;
    outputFile << "------------------------------------------" << std::endl;

    outputFile.close();
    return 0;
}