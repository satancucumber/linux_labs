#include <fstream>
#include <iostream>
#include <cstdlib>
#include <unistd.h>
std:: ofstream oddfile;
std:: ofstream evenfile;

void* threadFunctionOdd(void *arg){
    std::string str = *(static_cast<std::string*>(arg));
    oddfile << str<<"\n";
    pthread_exit(NULL);
}

void* threadFunctionEven(void *arg){
    std::string str = *(static_cast<std::string*>(arg));
    evenfile << str<<"\n";
     pthread_exit(NULL);
}


int main(){
    std:: ifstream text("text.txt",std::ios_base::in);
    oddfile.open("odd.txt",std::ios_base::trunc|std::ios_base::out );
    evenfile.open("even.txt",std::ios_base::trunc|std::ios_base::out );

    while(!text.eof()){
        std::string line1,line2;
        pthread_t thread1,thread2;

        if(getline(text,line1)) pthread_create(&thread1,NULL,&threadFunctionOdd,&line1);
        else break;
        if(getline(text,line2)) pthread_create(&thread2,NULL,&threadFunctionEven,&line2);
        else {pthread_join(thread1,NULL); break;}
        pthread_join(thread1,NULL);
        pthread_join(thread2,NULL);
    }
    oddfile.close();
    evenfile.close();
}