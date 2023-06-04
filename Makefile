WARNS=-Wextra -Werror -Wno-unused-parameter -Wpedantic
FSANS=-fsanitize=address,undefined
OPTS=-O3 -Os
CC=ccache clang

help:
	@echo "Makefile Usage; make [option]"
	@echo "Available [option]s;"
	@echo "    help . . . Display this text."
	@echo "    debug  . . Compile a binary made for debugging."
	@echo "    release  . Compile a binary optimized for release."
	@echo "    windows  . Use MinGW cross-platform compiler to"
	@echo "        compile a windows binary (will be release)."

debug:
	$(CC) mock.c -o mock $(WARNS) $(FSANS) -O0

release:
	$(CC) mock.c -o mock $(WARNS) $(OPTS)

# Uses MinGW to compile a windows binary on other platforms.
windows:
	make release CC=x86_64-w64-mingw32-gcc
