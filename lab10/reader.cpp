#include <iostream>
#include <fstream>
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
    std::streampos file_position = 0; // Позиция в файле
    std::string reader_name = argv[1];

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

    while (true) {

        struct sembuf sb1[2];
        sb1[0].sem_num = SEM_WRITER; // Номер семафора
        sb1[0].sem_op = 0;    // Ожидание ноля
        sb1[0].sem_flg = 0;    // Блокирующий вызов

        sb1[1].sem_num = SEM_READER; // Номер семафора
        sb1[1].sem_op = 1;    // Увеличить на 1
        sb1[1].sem_flg = 0;    // Блокирующий вызов
        if (semop(semid, sb1, 2) != 0) {
            std::cout << "Semop: " << errno << "\n"; 
        };

        // Чтение файла
        std::ifstream file(FILE_PATH);
        if (!file) {
            std::cerr << "Error opening file for reading\n";
            sb = {SEM_READER, -1, 0};
            if (semop(semid, &sb, 1) != 0) {
                std::cout << "Semop: " << errno << "\n"; 
            };
            break;
        }

         // Перемещаем указатель чтения на последнюю позицию
        file.seekg(file_position);

        std::string line;
        if (std::getline(file, line)) {
            // Читаем строку и обновляем позицию
            file_position = file.tellg(); // Сохраняем текущую позицию
            std::cout << "Reader " << reader_name << " read: " << line << '\n';
        } else {
            std::cout << "Reader " << reader_name << ": End of file or no more lines.\n";
            file.close();
             // Уменьшаем количество активных читателей
            sb = {SEM_READER, -1, 0};
            if (semop(semid, &sb, 1) != 0) {
                std::cout << "Semop: " << errno << "\n"; 
            };
            
            break;
        }
        file.close();
        
        // Уменьшаем количество активных читателей
        sb = {SEM_READER, -1, 0};
        if (semop(semid, &sb, 1) != 0) {
            std::cout << "Semop: " << errno << "\n"; 
        };
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
