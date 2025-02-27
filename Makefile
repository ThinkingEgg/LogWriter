CC = g++
CFLAGS = -std=c++17 -Wall -pthread
LDFLAGS =

SOURCES = log_console_app.cpp log_message_lib.cpp
HEADERS = LogWriter.h
OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLE = log_console_app

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

%.o: %.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJECTS) $(EXECUTABLE)