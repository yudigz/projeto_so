CC=gcc
CFLAGS=-Wall -Iinclude

# Pega todos os arquivos .c dentro da pasta src/
SRC=$(wildcard src/*.c)
# Define que os .o terão o mesmo nome e caminho dos .c (ficarão em src/)
OBJ=$(SRC:.c=.o)
BIN=simulador

# Descobre todos os .c em escalonadores/ e define o .so correspondente
# Ex: escalonadores/rr.c → escalonadores/librr.so
ESC_SRC=$(wildcard escalonadores/*.c)
ESC_SO=$(ESC_SRC:escalonadores/%.c=escalonadores/lib%.so)

all: $(BIN) $(ESC_SO)

# Passo final: linka os .o para criar o executável e apaga os .o em seguida
$(BIN): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(BIN) -ldl
	@rm -f $(OBJ)

# Passo intermediário: compila cada .c em um .o
src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Compila cada plugin em escalonadores/ como biblioteca compartilhada
# -shared: produz .so em vez de executável
# -fPIC: Position-Independent Code, obrigatório para código carregado com dlopen
escalonadores/lib%.so: escalonadores/%.c
	$(CC) $(CFLAGS) -shared -fPIC $< -o $@

clean:
	rm -f $(BIN) $(ESC_SO)
