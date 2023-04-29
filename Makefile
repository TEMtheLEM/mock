WARNS=-Wextra -Werror -Wno-unused-parameter
FSANS=-fsanitize=address,undefined
OPTS=-O3 -Os
CC=ccache clang

debug:
	$(CC) mock.c -o mock $(WARNS) $(FSANS) -O0

release:
	$(CC) mock.c -o mock $(WARNS) $(OPTS)
