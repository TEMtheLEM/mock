WARNS=-Wextra -Werror -Wno-unused-parameter
FSANS=-fsanitize=address,undefined
OPTS=-O3 -Os

debug:
	clang mock.c -o mock $(WARNS) $(FSANS) -O0

release:
	clang mock.c -o mock $(WARNS) $(OPTS)
