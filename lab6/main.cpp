#include <iostream>
#include <signal.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <unistd.h>

void sigHandler (int sig){
    time_t seconds_papent_start = time(NULL);
	tm* parent_start_time = localtime(&seconds_papent_start);
    struct timeval parent_start;
    gettimeofday(&parent_start, NULL);

	pid_t pid = fork();
	if (pid == 0){
        struct timeval child_start;
        gettimeofday(&child_start, NULL);
        time_t seconds_child_start = time(NULL);
        tm* child_start_time = localtime(&seconds_child_start);
        std:: cout << "Время старта потомка: " << asctime(child_start_time) << 
           "PID потомка: " << getpid() <<  std:: endl;
        struct timeval child_stop;
        gettimeofday(&child_stop, NULL);
        std:: cout << "Время потомка: " << (child_stop.tv_sec * 1000LL) + (child_stop.tv_usec / 1000) - (child_start.tv_sec * 1000LL) + (child_start.tv_usec / 1000) << " мс\n" << std:: endl;
        exit(EXIT_SUCCESS);
	}else{
        std:: cout << "Время старта родителя: " << asctime(parent_start_time) <<
            "PID родителя: " << getpid() << std:: endl;
        struct timeval parent_stop;
        gettimeofday(&parent_stop, NULL);
        std:: cout << "Время родителя: " << (parent_stop.tv_sec * 1000LL) + (parent_stop.tv_usec / 1000) - (parent_start.tv_sec * 1000LL) + (parent_start.tv_usec / 1000) << " мс\n" << std:: endl;
    }
}

int main(int argc, char *argv[]){
    struct itimerval timer, oldTimer;
    sigset_t mask;
    struct  sigaction sig;
 
    sigemptyset(&sig.sa_mask);
    sigaddset(&sig.sa_mask,SIGTSTP);
    sigprocmask(SIG_SETMASK,&sig.sa_mask,NULL);
    
    sig.sa_handler=sigHandler;
    sigaction(SIGALRM,&sig,NULL);
    
    int num = atoi(argv[2]);
    timer.it_interval.tv_sec= atoi(argv[1]);
    timer.it_interval.tv_usec = 0;
    timer.it_value.tv_sec=atoi(argv[1]);
    
    if (setitimer(ITIMER_REAL,&timer,NULL)==-1){
            std:: cout << "error\n";
            exit(1);
        }

    for (int i=0;i<num;i++){
        pause();
    }
}