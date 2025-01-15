#include <iostream>
#include <sys/msg.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>

struct my_msg{
    long mtype;
    int mtext;
};
int queu ;
void sigHandler (int sig)
{ 
    std:: cout << "No message to recieve \n";
    
    msgctl(queu,IPC_RMID,NULL);
    }

int main (int argc, char * argv[]){
    int waitTime = atoi(argv[1]);
    struct itimerval timer;
    queu = msgget(51,IPC_CREAT|402);
    if (queu == -1){
        std:: cout << "Error of create queu of message";
        return 1;
    }
    
    std :: cout << "Delay time:"<< waitTime << "\n";
    struct my_msg message ;
    sigset_t mask;
    struct  sigaction sig;
    sigemptyset(&sig.sa_mask);
    sigprocmask(SIG_SETMASK,&sig.sa_mask,NULL);
    sig.sa_handler=sigHandler;
    sigaction(SIGALRM,&sig,NULL);
    timer.it_interval.tv_sec= atoi(argv[1]);
    timer.it_interval.tv_usec = 0;
    timer.it_value.tv_sec=atoi(argv[1]);
    setitimer(ITIMER_REAL,&timer,NULL);
    while(true){
        auto reply = msgrcv(queu, &message,sizeof(my_msg)+1,0,0);
        if (reply>0){
            std:: cout << "New time:"<< message.mtext<<"\n";
            timer.it_interval.tv_sec= message.mtext;
            timer.it_interval.tv_usec = 0;
            timer.it_value.tv_sec = message.mtext;
            setitimer(ITIMER_REAL,&timer,NULL);
           
        }else{
            break;
        }
        if (message.mtype == -1){ std:: cout << "Error of recieve msg"; break;}
        
    }
}