#include <fstream>
#include <iostream>
#include <cstdlib>
#include <unistd.h>

void logProcessInfo(const std::string& fileName, const std::string& processName);

int main(int argc, char* argv[]) {
    if (argc != 5) {
        std::cerr << "Invalid number of parameters\n";
        return EXIT_FAILURE;
    }

    std::string fileName = argv[1];
    int parentDelay = std::atoi(argv[2]);
    int child1Delay = std::atoi(argv[3]);
    int child2Delay = std::atoi(argv[4]);

    std::ofstream outputFile(fileName, std::ios::trunc);
    if (!outputFile) {
        std::cerr << "Unable to open file " << fileName << "\n";
        return EXIT_FAILURE;
    }

    outputFile << "===== PARENT DELAY - " << argv[2] << ", CHILD1 DELAY - " << argv[3] 
               << ", CHILD2 DELAY - " << argv[4] << " =====\n";
    outputFile.close();

    pid_t child1 = fork();
    if (child1 == 0) {
        sleep(child1Delay);
        logProcessInfo(fileName, "CHILD1");
    } else {
        pid_t child2 = vfork();
        if (child2 == 0) {
            execl("./child2", argv[1], argv[4], nullptr);
        } else {
            sleep(parentDelay);
            logProcessInfo(fileName, "PARENT");
        }
    }

    return EXIT_SUCCESS;
}

void logProcessInfo(const std::string& fileName, const std::string& processName) {
    std::ofstream outputFile(fileName, std::ios::app);
    if (!outputFile) {
        std::cerr << "Unable to open file\n";
        return;
    }

    pid_t currentPid = getpid();
    outputFile << processName << " Process ID - " << currentPid << "\n";
    outputFile << processName << " Parent ID - " << getppid() << "\n";
    outputFile << processName << " Session ID - " << getsid(currentPid) << "\n";
    outputFile << processName << " Group ID - " << getpgid(currentPid) << "\n";
    outputFile << processName << " Real User ID - " << getuid() << "\n";
    outputFile << processName << " Effective User ID - " << getegid() << "\n";
    outputFile << processName << " Real Group ID - " << getgid() << "\n";
    outputFile << processName << " Effective Group ID - " << getegid() << "\n";
    outputFile << "------------------------------------------\n";
    outputFile.close();
}