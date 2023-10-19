#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>


void println(const char *format, ...) {
    va_list args;
    va_start(args, format);

    for (int i = 0; format[i] != '\0'; i++) {
        if (format[i] == '{' && format[i + 2] == ':' && format[i + 3] == '}') {
            char spec = format[i + 1];

            if (spec == 'd' || spec == 'i') {
                int arg = va_arg(args, int);
                printf("%d", arg);
            } else if (spec == 'f') {
                double arg = va_arg(args, double);
                printf("%f", arg);
            } else if (spec == 'c') {
                int arg = va_arg(args, int);
                putchar(arg);
            } else if (spec == 's') {
                const char *arg = va_arg(args, const char *);
                printf("%s", arg);
            } else if (spec == 'b') {
                bool arg = va_arg(args, int);
                printf("%s", arg ? "true" : "false");
            }
            i += 3;
        } else {
            putchar(format[i]);
        }
    }

    va_end(args);
    printf("\n");
}


int main() {
    char* m = "max";
    char* x = "john";
    char* a = "ali";
	println("hello {s:} and {s:} and {s:}", "max", "john", "ali");
}
