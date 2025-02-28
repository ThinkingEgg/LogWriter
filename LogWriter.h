#ifndef LOG_WRITER_H
#define LOG_WRITER_H

#include <exception>
#include <string>
#include <array>

#ifdef __cplusplus
extern "C" {
#endif

namespace log_message {

    //  Ошибка некорректного приоритета
    class MessagePriorityException : public std::exception {
        public:
            MessagePriorityException(const std::string& message);
            const char* what() const noexcept override;
        private:
            std::string msg; //  Текст ошибки
    };

    class Message {
        public:
            Message(size_t def_priority, const std::string& fname);  // Конструктор, принимает значение базового приоритета, имя файла
            ~Message();  //  Деструктор

            void set_default_priority(size_t& def_priority);  //  Изменение значения базового приоритета
            void set_filename(const std::string& fname);  //  Смена файла журнала
            bool only_numbers_in_str(const std::string& str);  //  Проверка на присутствие в непустой строке только чисел
            void write_in_log();  //  Запись сообщения в файл-журнал
            void input_message(const std::string& line);  //  Присвоение сообщению приоритета и текста по полученной строке

        private:
            std::string _text;  //  текст сообщения
            std::string _filename;  // имя файла
            size_t _priority;  //  присвоенный приоритет
            size_t _default_priority;  //  базовый приоритет
            std::array<std::string, 3> _array_of_priorities { "HIGH", "MEDIUM", "LOW" };  //  словестные обозначения приоритетов
    };
}

#ifdef __cplusplus
}
#endif

#endif  //  LOG_WRITER_H