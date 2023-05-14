WARNS=-Wextra -Werror -Wno-unused-parameter
FSANS=-fsanitize=address,undefined
OPTS=-O3 -Os
CC=ccache clang

debug:
	$(CC) mock.c -o mock $(WARNS) $(FSANS) -O0

release:
	$(CC) mock.c -o mock $(WARNS) $(OPTS)

# Uses MinGW to compile a windows binary on other platforms.
windows:
	make release CC=x86_64-w64-mingw32-gcc
