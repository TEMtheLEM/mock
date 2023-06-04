/**
 *  Mock - A fun, small, hack program that 'mocks' a file, user input,
 *         etc. See usage (USAGE.md or run this program without args).
 * 
 *  Copyright (C) 2023  Connor Inch (TEMtheLEM)  <temthelem@duck.com>
 * 
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 * 
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 * 
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include "appinfo.h"


#define STD_BUFF_SIZE 4096


enum EXITS {
        EXIT_OK,
        EXIT_HELP,
        EXIT_USER_FILE_NOT_GIVEN,
        EXIT_USER_FILE_NOT_OPENED,
        // OS specific ones.
        EXIT_UNIX_URANDOM_NOT_OPENED,
        EXIT_UNIX_URANDOM_BAD_READ,
        EXIT_WIN32_BAD_GENERATION,
};


#ifdef __unix__
// These functions are better but only available on *nix.
#define RAND() random()
#define SRAND(s) srandom(s)

int32_t urandomInt32(void) {
        FILE *fp = fopen("/dev/urandom", "r");
        if (!fp) {
                perror("Unable to open /dev/urandom for reading");
                exit(EXIT_UNIX_URANDOM_NOT_OPENED);
        }
        int32_t n;
        if (fread(&n, 1, sizeof n, fp) != sizeof n) {
                perror("Error while reading from /dev/urandom");
                exit(EXIT_UNIX_URANDOM_BAD_READ);
        }
        fclose(fp);
        return n;
}
#else
#ifdef __WIN32__
// Windows support.
#define RAND() rand()
#define SRAND(s) srand(s)

#include <windows.h>

int32_t urandomInt32(void) {
        HCRYPTPROV h_crypt_prov;
        int32_t n;

        if (CryptAcquireContext(&h_crypt_prov, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
                WINBOOL success = CryptGenRandom(h_crypt_prov, sizeof n, (BYTE*) &n);
                CryptReleaseContext(h_crypt_prov, 0);
                if (success)
                        return n;
        }

        printf("Error generating secure cryptographic number on __WIN32__ platform.\n");
        exit(EXIT_WIN32_BAD_GENERATION);
}
#else
#error "Unsupported operating system."
#endif
#endif


// Makes a character uppercase.
char upper(char c) {
        if ('a' <= c && c <= 'z') {
                c -= 'a';
                c += 'A';
        }

        return c;
}


// Makes a character lowercase.
char lower(char c) {
        if ('A' <= c && c <= 'Z') {
                c -= 'A';
                c += 'a';
        }

        return c;
}


// WARN: free() the returned string!
char* mockString(const char *s) {
        if (!s)
                return NULL;
        size_t len = strlen(s);
        char *mocked = (char*) malloc(len + 1);

        for (size_t i = 0; i < len; i++)
                mocked[i] = RAND() % 2 ? upper(s[i]) : lower(s[i]);

        mocked[len] = 0;
        return mocked;
}


// File mode. Used with --file or -f.
int32_t mockFile(const char *filename) {
        if (!filename) {
                printf("Provide a file.\n");
                return EXIT_USER_FILE_NOT_GIVEN;
        }

        FILE *fp = fopen(filename, "r");
        if (!fp) {
                perror("Error opening file");
                return EXIT_USER_FILE_NOT_OPENED;
        }

        char buffer[STD_BUFF_SIZE];
        while (fgets(buffer, STD_BUFF_SIZE, fp)) {
                char *mocked = mockString(buffer);
                printf("%s", mocked);
                free(mocked);
        }

        fclose(fp);

        return EXIT_OK;
}


// Parameter mode. Used with --parameters or -p.
int32_t mockAllArgs(const int32_t argc,
                    const char **argv) {
        for (int32_t i = 0; i < argc; i++) {
                char *mocked = mockString(argv[i]);
                printf("%s ", mocked);
                free(mocked);
        }

        if (argc)
                printf("\n");

        return EXIT_OK;
}


// Interactive mode. Used with --interactive or -i.
int32_t interactiveMode(void) {
        char *str = NULL,
              buffer[STD_BUFF_SIZE];
        size_t str_size = 0,
               buff_len = 0;

        while (fgets(buffer, STD_BUFF_SIZE, stdin)) {
                buff_len = strlen(buffer);
                str_size += buff_len;

                str = (char*) realloc(str, str_size + 1);

                str[str_size - buff_len] = 0;
                strcat(str, buffer);
        }

        char *mocked = mockString(str);
        free(str);
        if (!mocked)
                goto end;
        printf("%s", mocked);
        free(mocked);

        end:
        return EXIT_OK;
}


// Version & Copyright information. Shown with --version or -v.
int32_t displayVersion(void) {
        printf(

"mock v%d.%d.%d\n\n\
Copyright (C) %d  Connor Inch  (TEMtheLEM)\n\
This is free software; see the source for copying conditions.\n\
There is absolutely NO warranty; not even for\n\
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n",

MAJOR_VER, MINOR_VER, BUGFIX_VER, COPYRIGHT_YEAR);

        return EXIT_OK;
}


// Display the usage of the program.
int32_t displayHelp(void) {
        printf(

"Options are;\n\
    --file | -f . . . . . Mock a file.\n\
    --parameters | -p . . Mock all additional parameters.\n\
    --interactive | -i  . Start in \"Interactive\" mode.\n\
    --version | -v  . . . Display Copyright(s) & version.\n"

);

        return EXIT_HELP;
}


int32_t main(const int32_t argc,
             const char **argv) {
        SRAND(urandomInt32());

        if (argc < 2)
                goto help;

        const char *choice = argv[1];
        if (!strcmp(choice, "--file") || !strcmp(choice, "-f"))
                return mockFile(argc > 2 ? argv[2] : NULL);
        if (!strcmp(choice, "--parameters") || !strcmp(choice, "-p"))
                return mockAllArgs(argc - 2, argv + 2);
        if (!strcmp(choice, "--interactive") || !strcmp(choice, "-i"))
                return interactiveMode();
        if (!strcmp(choice, "--version") || !strcmp(choice, "-v"))
                return displayVersion();

        printf("Unrecognized option '%s'.\n", argv[1]);

        help:
        return displayHelp();
}
