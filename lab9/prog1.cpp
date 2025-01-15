#include <sys/shm.h>
#include <iostream>
#include <unistd.h>
#include <cstdlib>

#define GLOBAL_KEY 300
#define PROCESS_NUM 0

struct SharedData {
    int init_proc[3];
    int queue_num = 1;
    int queue[3];
    pid_t pids[3];
};

void enter_critical_section(SharedData *data, pid_t pid) {

    // Устанавливаем номер запроса как max(queue) + 1
    data->queue[PROCESS_NUM] = data->queue_num;
    data->queue_num = data->queue_num + 1;

    // Ждём своей очереди
    bool in_queue = true;
    while (in_queue) {
        in_queue = false;
        for (int i = 0; i < 3; ++i) {
            if (i != PROCESS_NUM && data->init_proc[i] == 1) {
                if (data->queue[i] < data->queue[PROCESS_NUM] || 
                    (data->queue[i] == data->queue[PROCESS_NUM] && data->pids[i] < pid)) {
                    in_queue = true;
                    break;
                }
            }
        }
    }
}

void leave_critical_section(SharedData *data, pid_t pid) {
    // Освобождаем свой номер и место
    data->queue[PROCESS_NUM] = 99;
}


int main(int argc, char* argv[]) {
    
    std::cout << "Количество: " << argv[1] << " Время: " << argv[2] << "\n";
    
    int count = atoi(argv[1]);
    int timeout = atoi(argv[2]);

    int shmid; 

    // Подключение к (создание) разделяемой памяти
    shmid = shmget(GLOBAL_KEY, sizeof(SharedData), 0666 | IPC_CREAT);
    SharedData *data = (SharedData*)shmat(shmid, NULL, 0);

    pid_t pid = getpid();
    data->pids[PROCESS_NUM] = pid;
    data->init_proc[PROCESS_NUM] = 1;

    int i = 0;
    while (i < count) {
        // Вход в критическую секцию
        enter_critical_section(data, pid);
        i++;
        std::cout << "Запись: " << PROCESS_NUM << " : " << i << "\n";
        FILE *fp = fopen("output.txt", "a");
        fprintf(fp, "%d PID: %d \n", PROCESS_NUM, pid);
        fclose(fp);
        sleep(timeout);
        // Выход из критической секции
        leave_critical_section(data, pid);
    }

    shmid_ds buffer;

    shmdt(data);
    std::cout << "Откреппление от разделяемого сегмента\n";
    
    shmctl(shmid, IPC_STAT, &buffer);
    std::cout << "Кол-во процессов, разделяющих память: " << buffer.shm_nattch << "\n";
    if (buffer.shm_nattch == 1) {
        shmctl(shmid, IPC_RMID, nullptr);
        std::cout << "Уничтожение разделяемого сегмента\n";
    }
   
    return 0;
}