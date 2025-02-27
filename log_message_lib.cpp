#include <iostream>
#include <fstream>
#include <string>
#include <array>
#include <condition_variable>
#include "LogWriter.h"

namespace log_message {

    //  Ошибка некорректного приоритета
    MessagePriorityException::MessagePriorityException(const std::string& message) : msg(message) {}
    const char* MessagePriorityException::what() const noexcept { return msg.c_str(); }

    // Конструктор, принимает значение базового приоритета, имя файла
    Message::Message(size_t def_priority, const std::string& fname) : _filename(fname) {
        while (1 > def_priority || def_priority > this->_array_of_priorities.size()) {
            std::cout << "Введенный базовый приоритет не входит в диапазон от 1 до " << this->_array_of_priorities.size()
            << ". \nВведите корректный приоритет: ";
            std::cin >> def_priority;
        }
        this->_default_priority = def_priority;
    };

    //  Деструктор
    Message::~Message() {};

    //  Изменение значения базового приоритета
    void Message::set_default_priority(size_t& def_priority) {
        if (1 <= def_priority && def_priority <= this->_array_of_priorities.size()) { 
            this->_default_priority = def_priority;
            std::cout << "Базовый приоритет изменен на " << this->_default_priority << " - " << this->_array_of_priorities[this->_default_priority - 1] << std::endl;
        } else {
            std::cerr << "Введенный приоритет не входит в диапазон от 1 до " << this->_array_of_priorities.size() << ", изменения не произведены\n";
        }
    }

    //  Смена файла журнала
    void Message::set_filename(const std::string& fname) {
        this->_filename = fname;
        std::cout << "Файл журнала сменен на " << this->_filename << std::endl;
    }

    //  Проверка на присутствие в непустой строке только чисел
    bool Message::only_numbers_in_str(const std::string& str) {
        for (const auto& c : str) {
            if ('0' > c || c > '9') {
                return false;
            }
        }
        return ( str.size() != 0 );  //  функция вернет false, если строка пустая
    }

    //  Запись сообщения в лог-файл
    void Message::write_in_log() {
        std::fstream logfile(this->_filename, std::ios::app);
        if (logfile.is_open()) {

            // Записываем системное время, убираем перенос строки, добавляемый std::ctime
            std::time_t current_system_time = std::time(nullptr);
            std::string str_time = std::ctime(&current_system_time);
            if (str_time.back() == '\n') str_time = str_time.substr(0, str_time.size() - 1);

            //[<приоритет>] [<системное время>] "<Сообщение>"
            logfile << "[" << this->_array_of_priorities[this->_priority - 1] << "] [" << str_time << "] " << "\"" << this->_text << "\"" << std::endl;

        } else { std::cerr << "Ошибка открытия файла " << this->_filename << std::endl; }
        logfile.close();
    }

    //  Присвоение сообщению приоритета и текста по полученной строке
    void Message::input_message(const std::string& line) {
        size_t space_pos = line.find(' ');
        if (line[0] == '~' && space_pos != std::string::npos) { // Проверяем, есть ли знак ~ в начале сообщения и есть ли пробел после него
                
            std::string priority_str = line.substr(1, space_pos - 1);

            // Проверяем, что значение приоритета корректно
            if (only_numbers_in_str(priority_str)){
                try {
                    this->_text = line.substr(space_pos + 1);
                    size_t temp_priority = std::stoi(priority_str);
                    if (1 > temp_priority || temp_priority > this->_array_of_priorities.size()) { throw MessagePriorityException("Указанный приоритет не выходит в диапазон от 1 до " + std::to_string(this->_array_of_priorities.size())); }

                    this->_priority = temp_priority;
                    // Если число вне диапазона значений приоритетов(1-<кол-во приоритетов>) или максимального значения size_t, оставляем базовый приоритет, сообщение - подстрока после флага приоритета
                } catch (const std::out_of_range& e) {
                    std::cerr << "Ошибка записи приоритета: " << e.what() << ", присвоен базовый приоритет" << std::endl;
                    this->_priority = this->_default_priority;
                } catch (const MessagePriorityException& e) {
                    std::cerr << "Ошибка записи приоритета: " << e.what() << ", присвоен стандартный приоритет" << std::endl;
                    this->_priority = this->_default_priority;
                }
            } else {
                // Если в приоритете введено не число, оставляем базовый приоритет, сообщение - вся строка
                this->_priority = this->_default_priority;
                this->_text = line;
            }
        } else {
            // Если флага нет, используем базовый приоритет и всю строку как сообщение
            this->_priority = this->_default_priority;
            this->_text = line;
        }

        //  Запись сообщения в журнал, если приоритет не ниже базового
        if (this->_priority <= this->_default_priority) { write_in_log(); }
    }
}