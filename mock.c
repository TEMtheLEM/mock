#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>


#define STD_BUFF_SIZE 4096


#ifdef __unix__
// These functions are better but only available on *nix.
#define RAND() random()
#define SRAND(s) srandom(s)

int32_t urandomInt32() {
        FILE *fp = fopen("/dev/urandom", "r");
        int32_t n;
        fread(&n, 1, sizeof n, fp);
        fclose(fp);
        return n;
}
#else
#ifdef __WIN32__
// Windows support.
#define RAND() rand()
#define SRAND(s) srand(s)

#include <windows.h>

int32_t urandomInt32() {
        HCRYPTPROV h_crypt_prov;
        int32_t n;

        if (CryptAcquireContext(&h_crypt_prov, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
                if (CryptGenRandom(h_crypt_prov, sizeof n, (BYTE*) &n)) {
                        CryptReleaseContext(h_crypt_prov, 0);
                        return n;
                }
                CryptReleaseContext(h_crypt_prov, 0);
        }

        return -1;
}
#else
#error "Unsupported operating system."
#endif
#endif


char upper(char c) {
        if ('a' <= c && c <= 'z') {
                c -= 'a';
                c += 'A';
        }

        return c;
}


char lower(char c) {
        if ('A' <= c && c <= 'Z') {
                c -= 'A';
                c += 'a';
        }

        return c;
}


// WARN: free() the returned string!
char* mockString(const char *s) {
        size_t len = strlen(s);
        char *m = (char*) malloc(len + 1);

        for (size_t i = 0; i < len; i++) {
                char c = s[i];
                m[i] = RAND() % 2 ? upper(c) : lower(c);
        }

        m[len] = 0;
        return m;
}


void mockStream(void *stream) {
        char *str = NULL,
              buffer[STD_BUFF_SIZE];
        size_t str_size = 0,
               buff_len = 0;

        while (fgets(buffer, STD_BUFF_SIZE, stream)) {
                buff_len = strlen(buffer);
                str_size += buff_len;

                str = (char*) realloc(str, str_size + 1);

                str[str_size - buff_len] = 0;
                strcat(str, buffer);
        }

        char *mocked = mockString(str);
        free(str);
        printf("%s", mocked);
        free(mocked);
}


int32_t mockFile(const char *filename) {
        if (!filename) {
                printf("Provide a file.");
                return 2;
        }

        FILE *fp = fopen(filename, "r");
        if (fp == NULL) {
                perror("Error opening file");
                return 3;
        }

        mockStream(fp);
        fclose(fp);

        return 0;
}


int32_t mockAllArgs(const int32_t argc,
                    const char **argv) {
        for (int32_t i = 0; i < argc; i++) {
                char *mocked = mockString(argv[i]);
                printf("%s ", mocked);
                free(mocked);
        }
        printf("\n");

        return 0;
}


int32_t interactiveMode() {
        mockStream(stdin);

        return 0;
}


int32_t displayHelp() {
        printf("Options are;\n\
        --file | -f . . . . . Mock a file.\n\
        --parameters | -p . . The following parameters.\n\
        --interactive | -i  . Start in \"Interactive\" mode.\n");
        return 1;
}


int32_t main(const int32_t argc,
             const char **argv) {
        SRAND(urandomInt32());

        if (argc < 2)
                goto help;

        const char *choice = argv[1];
        if (strcmp(choice, "--file") == 0 ||
            strcmp(choice, "-f") == 0)
                return mockFile(argc > 2 ? argv[2] : NULL);
        if (strcmp(choice, "--parameters") == 0 ||
            strcmp(choice, "-p") == 0)
                return mockAllArgs(argc - 2, argv + 2);
        if (strcmp(choice, "--interactive") == 0 ||
            strcmp(choice, "-i") == 0)
                return interactiveMode();

        printf("Unrecognized option '%s'.\n", argv[1]);

        help:
        return displayHelp();
}
