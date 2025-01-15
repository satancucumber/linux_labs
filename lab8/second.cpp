#include <iostream>
#include <sys/msg.h>
#include <unistd.h>
#include <cstring>

struct my_msg{
    long mtype;
    int mtext;
};

int main (int argc, char * argv[]){

    int queu = msgget(51,IPC_CREAT|IPC_EXCL);
    if (queu != -1){
        std:: cout << "First programm is not running\n";
        msgctl(queu,IPC_RMID,NULL);
        return 1;
    }   
    queu = msgget(51,IPC_CREAT);
    struct  my_msg message ;
    message.mtype=1;
    message.mtext=atoi(argv[1]);
    auto code = msgsnd(queu,&message,sizeof(my_msg)+1,0);
    
}