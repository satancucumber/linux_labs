#include <fstream>
#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <wait.h>


int pipe_fd[2];


int main (int argc, char* argv[]){
    sigset_t mask;
 
    sigemptyset(&mask);
    sigaddset(&mask,SIGUSR1);
    sigaddset(&mask,SIGUSR2);
    sigaddset(&mask,SIGQUIT);
    sigprocmask(SIG_BLOCK,&mask,NULL);
    std:: ifstream text(argv[1],std::ios_base::in);
    if (!text.is_open()) exit(1);
    if(pipe(pipe_fd)==-1){
        std:: cout << "Cant create pipe\n";
        return 1;
    }
    fcntl(pipe_fd[0],F_SETFL,(fcntl(pipe_fd[0],F_GETFL))|O_NONBLOCK);
    char targ[10];
    sprintf(targ,"%d",pipe_fd[0]);
    std::cout<<1;
    auto child1= fork();
    if (child1==0){
        close(pipe_fd[1]);
        execl("child", targ, NULL);
    } else {
        auto child2= fork();
        if (child2 ==0){
            close(pipe_fd[1]);
            execl("child2", targ, NULL);
        }else {
                                                                                                                                 usleep(100000);
			std::string  s;
            close(pipe_fd[0]);
			while(!text.eof()){
               getline(text,s);
				write(pipe_fd[1], &s[0],strlen(s.c_str()));
			}
			text.close();
            close(pipe_fd[1]);
            kill(child1,SIGQUIT);
            kill(child2,SIGQUIT);
			int st1, st2;
            waitpid(child1,&st1,0);
            waitpid(child2,&st2,0);
        
        }
    }


}