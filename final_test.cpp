#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>

// Структура, представляющая студента
struct Student {
    int id;
    std::string name;
    int age;
};

class StudentDatabase {
private:
    std::vector<std::shared_ptr<Student>> students;

public:
    // Метод для добавления нового студента в базу данных
    void addStudent(int id, const std::string& name, int age) {
        auto student = std::make_shared<Student>();
        student->id = id;
        student->name = name;
        student->age = age;
        students.push_back(student);
    }

    // Метод для удаления студента по идентификатору
    void removeStudent(int id) {
        students.erase(std::remove_if(students.begin(), students.end(), 
            [id](const std::shared_ptr<Student>& student) { return student->id == id; }), students.end());
    }

    // Метод для получения информации о студенте по идентификатору
    std::shared_ptr<Student> getStudentById(int id) {
        auto it = std::find_if(students.begin(), students.end(),
            [id](const std::shared_ptr<Student>& student) { return student->id == id; });
        if (it != students.end()) {
            return *it;
        } else {
            return nullptr;
        }
    }
};

int main() {
    StudentDatabase database;

    // Добавление студентов
    database.addStudent(1, "Alice", 20);
    database.addStudent(2, "Bob", 21);
    database.addStudent(3, "Charlie", 22);

    // Получение информации о студенте по идентификатору и вывод на экран
    std::shared_ptr<Student> student = database.getStudentById(2);
    if (student) {
        std::cout << "Student found: " << student->name << ", Age: " << student->age << std::endl;
    } else {
        std::cout << "Student not found!" << std::endl;
    }

    // Удаление студента
    database.removeStudent(2);

    // Получение информации о студенте по удаленному идентификатору
    student = database.getStudentById(2);
    if (student) {
        std::cout << "Student found: " << student->name << ", Age: " << student->age << std::endl;
    } else {
        std::cout << "Student not found!" << std::endl;
    }

    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------

#include <iostream>
#include <thread>
#include <mutex>

std::mutex mtx; // Создаем мьютекс

int sharedData = 0; // Общие данные для записи и чтения

// Функция, выполняемая в потоке для записи данных
void writerThread() {
    for (int i = 0; i < 5; ++i) {
        // Блокировка мьютекса перед записью
        mtx.lock();
        // Записываем данные
        sharedData = i;
        std::cout << "Writer wrote: " << sharedData << std::endl;
        // Разблокировка мьютекса после записи
        mtx.unlock();
        // Имитируем небольшую задержку
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

// Функция, выполняемая в потоке для чтения данных
void readerThread() {
    for (int i = 0; i < 5; ++i) {
        // Блокировка мьютекса перед чтением
        mtx.lock();
        // Читаем данные
        int data = sharedData;
        std::cout << "Reader read: " << data << std::endl;
        // Разблокировка мьютекса после чтения
        mtx.unlock();
        // Имитируем небольшую задержку
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

int main() {
    // Создаем потоки для записи и чтения
    std::thread writer(writerThread);
    std::thread reader(readerThread);

    // Ждем, пока потоки завершат выполнение
    writer.join();
    reader.join();

    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

const int BUFFER_SIZE = 1024;
const int PORT = 8080;

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE] = {0};

    // Создание TCP сокета
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        std::cerr << "Ошибка при создании сокета" << std::endl;
        return EXIT_FAILURE;
    }

    // Настройка серверного адреса
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Привязка сокета к адресу и порту
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        std::cerr << "Ошибка при привязке сокета" << std::endl;
        return EXIT_FAILURE;
    }

    // Прослушивание сокета
    if (listen(server_socket, 5) == -1) {
        std::cerr << "Ошибка при прослушивании сокета" << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Сервер запущен. Ожидание подключений..." << std::endl;

    socklen_t client_addr_size = sizeof(client_addr);

    // Принятие подключения от клиента
    if ((client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_size)) == -1) {
        std::cerr << "Ошибка при принятии подключения" << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Подключение принято. Ожидание сообщений от клиента..." << std::endl;

    while (true) {
        // Получение сообщения от клиента
        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (bytes_received == -1) {
            std::cerr << "Ошибка при получении данных от клиента" << std::endl;
            break;
        } else if (bytes_received == 0) {
            std::cout << "Клиент отключился" << std::endl;
            break;
        }

        // Вывод полученного сообщения
        std::cout << "Получено от клиента: " << buffer << std::endl;

        // Отправка подтверждения клиенту
        const char *ack = "Сообщение получено";
        send(client_socket, ack, strlen(ack), 0);

        // Очистка буфера
        memset(buffer, 0, BUFFER_SIZE);
    }

    // Закрытие сокетов
    close(client_socket);
    close(server_socket);

    return EXIT_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------

#include <iostream>

int main() {
    int arr[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    
    // Получение указателя на первый элемент массива
    int *ptr = &arr[0]; // или int *ptr = arr;

    // Перемещение указателя на четвертый элемент массива с помощью арифметики указателей
    ptr += 3; // Эквивалентно ptr = ptr + 3;

    // Вывод четвертого элемента массива
    std::cout << "arr[3]: " << *ptr << std::endl;

    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------
