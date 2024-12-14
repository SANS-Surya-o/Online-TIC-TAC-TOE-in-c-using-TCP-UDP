# Compiler
CC = gcc

# Compiler flags
CFLAGS = -DDEBUG -DUSE_TCP

# Source files and directories
SERVER_SRC = server.c helper/*.c
CLIENT_SRC = client.c helper/*.c

# Executable names
SERVER_EXEC = server
CLIENT_EXEC = client

# Targets
all: $(SERVER_EXEC) $(CLIENT_EXEC)

$(SERVER_EXEC): $(SERVER_SRC)
	$(CC) $(SERVER_SRC) -o $(SERVER_EXEC) $(CFLAGS)

$(CLIENT_EXEC): $(CLIENT_SRC)
	$(CC) $(CLIENT_SRC) -o $(CLIENT_EXEC) $(CFLAGS)

# Clean up
clean:
	rm -f $(SERVER_EXEC) $(CLIENT_EXEC)
