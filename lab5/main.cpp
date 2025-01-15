#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <csignal>

void zeroDivisionHandler(int sig);

void segmentationHandler(int sig);

int main(int argc, char* argv[]){
    if (argc !=2){
        std:: cout<< "Wrong count of parameters\n";
        return 1;
    }
    if(signal(SIGFPE,zeroDivisionHandler)==SIG_ERR or signal(SIGSEGV,segmentationHandler)==SIG_ERR){
        std:: cout << "error of set signals\n";
    }
    int choice = atoi(argv[1]);
    char *buffer=nullptr;
    int a =0 ;
    switch(choice){
        case 1:
            a =5/a;
            break;
        case 2:
            buffer[0]=0;
            break;
        default:
            std:: cout <<"unknown parameter \n";
            break;
    }
}    

void zeroDivisionHandler(int sig){
    std:: cout <<sig<< "- Arithmetic error \n";
    exit(1);
}

void segmentationHandler(int sig){
    std:: cout <<sig<< "- Memory protection error error \n";
    exit(1);
}