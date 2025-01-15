#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>

constexpr int MAX_PROCESSES = 3;

// Структура для хранения разделяемых данных
struct SharedData
{
    bool choosing[MAX_PROCESSES]; // Флаги выбора номера
    int number[MAX_PROCESSES];    // Номера процессов
};

SharedData *create_shared_memory(int &shmid)
{
    shmid = shmget(420, sizeof(SharedData), IPC_CREAT | 0666);
    if (shmid == -1)
    {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    void *shm = shmat(shmid, nullptr, 0);
    if (shm == (void *)-1)
    {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    auto *shared_data = static_cast<SharedData *>(shm);
    return shared_data;
}

void destroy_shared_memory(int shmid, SharedData *shared_data)
{
    shmid_ds buffer;

    shmctl(shmid, IPC_STAT, &buffer);
    if (buffer.shm_nattch == 1)
    {
        shmdt(shared_data);
        shmctl(shmid, IPC_RMID, nullptr);
    }
    else
    {
        shmdt(shared_data);
    }
}
    // Функция нахождения максимального значения в массиве
    int get_max(const int *arr, int size)
    {
        int max_val = 0;
        for (int i = 0; i < size; ++i)
        {
            if (arr[i] > max_val)
            {
                max_val = arr[i];
            }
        }
        return max_val;
    }

    // Вход в критическую секцию
    void enter_critical_section(SharedData * shared_data, int process_id)
    {
        shared_data->choosing[process_id] = true;
        shared_data->number[process_id] = get_max(shared_data->number, MAX_PROCESSES) + 1;
        shared_data->choosing[process_id] = false;

        for (int j = 0; j < MAX_PROCESSES; ++j)
        {
            if (j == process_id)
                continue;

            // Ждем, пока поток j завершит выбор номера
            while (shared_data->choosing[j])
                ;

            // Ждем, пока потоки с более высоким приоритетом завершат работу
            while (shared_data->number[j] != 0 &&
                   (shared_data->number[j] < shared_data->number[process_id] ||
                    (shared_data->number[j] == shared_data->number[process_id] && j < process_id)))
            {
                std::this_thread::yield(); // Уступаем процессор
            }
        }
    }

    // Выход из критической секции
    void exit_critical_section(SharedData * shared_data, int process_id)
    {
        shared_data->number[process_id] = 0;
    }

    // Запись в файл
    void write_to_file(const std::string &file_name, int process_id, int line_number)
    {
        std::ofstream file(file_name, std::ios::app);
        if (file.is_open())
        {
            file << "Process " << process_id << " writes line " << line_number << "\n";
            file.close();
        }
        else
        {
            std::cerr << "Error opening file\n";
            file.close();
        }
    }

    // Основная функция записи с синхронизацией
    void writer(SharedData * shared_data, int process_id, const std::string &file_name, int interval, int lines)
    {
        for (int i = 0; i < lines; ++i)
        {
            enter_critical_section(shared_data, process_id);

            // Критическая секция: запись в файл
            write_to_file(file_name, process_id, i + 1);

            exit_critical_section(shared_data, process_id);

            // Задержка перед следующей записью
            std::this_thread::sleep_for(std::chrono::milliseconds(interval));
        }
    }

    int main(int argc, char *argv[])
    {
        if (argc != 5)
        {
            std::cerr << "Usage: " << argv[0] << " <process_id> <file_name> <interval_ms> <lines> \n";
            return EXIT_FAILURE;
        }

        int process_id = std::atoi(argv[1]);
        std::string file_name = argv[2];
        int interval = std::atoi(argv[3]);
        int lines = std::atoi(argv[4]);
        

        int shmid;
        SharedData *shared_data = create_shared_memory(shmid);
        if (!shared_data)
        {
            return EXIT_FAILURE;
        }
        writer(shared_data, process_id, file_name, interval, lines);

        destroy_shared_memory(shmid, shared_data);

        return EXIT_SUCCESS;
    }
