CC=gcc
CFLAGS=-Wall -Iinclude

# Pega todos os arquivos .c dentro da pasta src/
SRC=$(wildcard src/*.c)
# Define que os .o terão o mesmo nome e caminho dos .c (ficarão em src/)
OBJ=$(SRC:.c=.o)
BIN=simulador

all: $(BIN)

# Passo final: linka os .o para criar o executável e apaga os .o em seguida
$(BIN): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(BIN)
	@rm -f $(OBJ)

# Passo intermediário: compila cada .c em um .o
src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(BIN)