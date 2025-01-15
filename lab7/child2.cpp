#include <fstream>
#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <wait.h>

bool isClose=false;
void sigHandler(int sig){
    isClose=true;
}
int main(int argc, char* argv[]){
        sigset_t mask,b_mask;
         struct sigaction sigact;
        sigemptyset(&mask);
        sigaddset(&mask,SIGUSR2);
        sigaddset(&b_mask,SIGQUIT);
        sigprocmask(SIG_UNBLOCK,&b_mask,NULL);
        sigact.sa_handler=sigHandler;
        sigaction(SIGQUIT,&sigact,NULL);
        int readCnt=-1;
         int fd = atoi(argv[0]) ;
        std:: ofstream evenfile("even.txt",std::ios_base::trunc|std::ios_base::out );
        char c;
        int sig;
        sigwait(&mask,&sig);
        while(!isClose || (readCnt=read(fd,&c,1))>0){
            if (readCnt>0){
                evenfile<<c<<"\n";
                killpg(0,SIGUSR1);
                sigwait(&mask,&sig);
            }
        }
        killpg(0,SIGUSR1);
        evenfile.close();
}