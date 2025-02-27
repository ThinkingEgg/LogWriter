#include <iostream>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <stdlib.h>
#include <chrono>
#include <string>
#include <fstream>
#include <array>
#include "LogWriter.h"


// Потокобезопасная очередь для передачи сообщений между потоками
class MessageQueue {
public:
    void push(const std::string& message) {
        std::lock_guard<std::mutex> lock(_mutex);
        _queue.push(message);
        _cond_var.notify_one(); // Уведомляем о новом элементе
    }

    std::string pop() {
        std::unique_lock<std::mutex> lock(_mutex);
        _cond_var.wait(lock, [this] { return !_queue.empty() || _stop; });

        if (_queue.empty()) {
            return ""; // Если очередь пуста и поток остановлен, возвращаем пустую строку
        }

        std::string message = _queue.front();
        _queue.pop();
        return message;
    }

    void stop() {
        {
            std::lock_guard<std::mutex> lock(_mutex);
            _stop = true;
        }
        _cond_var.notify_all(); // Уведомляем все ожидающие потоки об остановке
    }

private:
    std::queue<std::string> _queue;
    std::mutex _mutex;
    std::condition_variable _cond_var;
    bool _stop = false;
};

// Функция для записи сообщений в журнал в отдельном потоке
void logThreadFunction(MessageQueue& queue, log_message::Message& log_writer) {
    while (true) {
        std::string message = queue.pop();
        if (message.empty()) break; // Если сообщение пустое - выход из цикла

        log_writer.input_message(message); // Записываем сообщение в лог
    }
}

// Функция для очистки экрана в зависимости от системы
void clear_screen() {
#ifdef WINDOWS
    std::system("cls");
#else
    std::system ("clear");
#endif
}

// Первым параметром после программы указывается приоритет, затем имя файла-журнала
int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << argv[0] << ": введены не все параметры\n";
        return 52;
    }
    try {
        log_message::Message log_writer(std::stoul(argv[1]), argv[2]);
        MessageQueue queue;
        std::thread logThread(logThreadFunction, std::ref(queue), std::ref(log_writer));

        int choice = 0;
        std::string input;
        std::atomic<bool> running(true);

        while (running) {
            std::cout << "\nМеню:\n"
                      << "1. Ввести сообщение\n"
                      << "2. Изменить базовый приоритет сообщения\n"
                      << "3. Сменить файл журнала\n"
                      << "4. Закрыть программу\n"
                      << "Выберите действие: ";
            
            // Чистый ввод выбора меню в случае ошибки ввода
            if (!(std::cin >> choice)) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                clear_screen();
                continue;
            }

            switch (choice) {
                case 1: { // Ввести сообщение
                    std::cout << "Введите сообщение: ";
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Полная очистка буфера
                    std::getline(std::cin, input);
                    clear_screen();
                    queue.push(input);
                    std::this_thread::sleep_for(std::chrono::milliseconds(5));
                    break;
                }
                case 2: { // Изменить приоритет
                    size_t newPriority;
                    std::cout << "Новый приоритет: ";
                    std::cin >> newPriority;
                    clear_screen();
                    log_writer.set_default_priority(newPriority);
                    break;
                }
                case 3: { // Сменить файл
                    std::string newFilename;
                    std::cout << "Введите новое имя файла: ";
                    std::cin >> newFilename;
                    clear_screen();
                    log_writer.set_filename(newFilename);

                    break;
                }
                case 4: { // Выход
                    running = false;
                    break;
                }
                default:
                    clear_screen();
                    std::cerr << "Неверный выбор. Попробуйте снова.\n";
            }
        }

        queue.stop();
        if (logThread.joinable()) { logThread.join(); }

    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
    }

    return 0;
}