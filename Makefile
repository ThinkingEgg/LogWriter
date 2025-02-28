CC = gcc
CFLAGS = -std=c++17 -Wall -pthread -fPIC
LDFLAGS = -L. -llog_message -Wl,-rpath,.

APP = log_console_app    
LIBRARY = liblog_message.so     

SOURCES = log_console_app.cpp log_message_lib.cpp       
HEADERS = LogWriter.h

all: $(APP)

$(LIBRARY): log_message_lib.o
	$(CC) $(CFLAGS) -shared -o $@ $<

%.o: %.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

$(APP): log_console_app.o $(LIBRARY)
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

clean:
	rm -f *.o $(APP) $(LIBRARY)