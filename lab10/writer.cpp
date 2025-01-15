#include <iostream>
#include <fstream>
#include <cstring>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <unistd.h>
#include <cstdlib>

#define FILE_PATH "text.txt"
#define SEM_KEY 1013
#define SEM_FILE 0
#define SEM_READER 1
#define SEM_WRITER 2
#define SEM_INIT 3

int main(int argc, char* argv[]) {

    std::string writer_name = argv[1];
    int N = std::stoi(argv[2]);
    int time = std::stoi(argv[3]);

    // Получаем/создаем семафоры
    int semid = semget(SEM_KEY, 4, IPC_CREAT | 0666);
    if (semid == -1) {
        perror("semget");
        return 1;
    }

    // Инициализация начального значения семафора писателя
    struct sembuf sb = {SEM_INIT, 1, 0};
    semop(semid, &sb, 1);
    if (semctl(semid, SEM_INIT, GETVAL) == 1) {
        sb = {SEM_FILE, 1, 0};
        if (semop(semid, &sb, 1) != 0) {
            std::cout << "Semop: " << errno << "\n"; 
        };
        std::cout << "Writer init \n"; 
    }


    for (int i = 0; i < N; ++i) {
        // Ждем, пока нет активных читателей (sem reader работает как счётчик)
        struct sembuf sb2[3];
        sb2[0].sem_num = SEM_READER; // Номер семафора
        sb2[0].sem_op = 0;     // Ожидание значения 0
        sb2[0].sem_flg = 0;    // Блокирующий вызов

        sb2[1].sem_num = SEM_FILE; // Номер семафора
        sb2[1].sem_op = -1;    // Уменьшить на 1
        sb2[1].sem_flg = 0;    // Блокирующий вызов

        sb2[2].sem_num = SEM_WRITER; // Номер семафора
        sb2[2].sem_op = 1;    // Увеличить на 1
        sb2[2].sem_flg = 0;    // Блокирующий вызов
        
        if (semop(semid, sb2, 3) != 0) {
            std::cout << "Semop: " << errno << "\n"; 
        };

        // Запись в файл
        std::ofstream file(FILE_PATH, std::ios::app);
        if (!file) {
            std::cerr << "Error opening file for writing\n";
            return 1;
        }

        std::string data = writer_name + ": Message " + std::to_string(i + 1) + "\n";
        file << data;
        file.close();

        std::cout << "Writer " << writer_name << " wrote: " << data;

        // Разблокируем файл
        struct sembuf sb1[2];
        sb1[0].sem_num = SEM_WRITER; // Номер семафора
        sb1[0].sem_op = -1;     // Уменьшить на 1 
        sb1[0].sem_flg = 0;    // Блокирующий вызов

        sb1[1].sem_num = SEM_FILE; // Номер семафора
        sb1[1].sem_op = 1;    // Увеличить на 1
        sb1[1].sem_flg = 0;    // Блокирующий вызов
        //sb = {SEM_FILE, 1, 0};
        if (semop(semid, sb1, 2) != 0) {
            std::cout << "Semop: " << errno << "\n"; 
        };
        sleep(time); 
    }

   
    // Проверяем, последний ли процесс
    sb = {SEM_INIT, -1, 0};
    if (semop(semid, &sb, 1) != 0) {
        std::cout << "Semop: " << errno << "\n"; 
    };
    std::cout << semctl(semid, SEM_INIT, GETVAL) << " processes remain \n";
    if (semctl(semid, SEM_INIT, GETVAL) == 0) {
        if (semctl(semid, 0, IPC_RMID) == -1) {
        perror("Failed to remove semaphores (Probably already removed)");
        } else {
            std::cout << "Semaphores removed successfully\n";
        }
    }

    return 0;
}
