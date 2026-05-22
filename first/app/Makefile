CC = riscv64-linux-gnu-gcc
CFLAGS = -Wall -Wextra -O2

all: get_misa

get_misa: main.c
	$(CC) $(CFLAGS) -o get_misa main.c

clean:
	rm -f get_misa
