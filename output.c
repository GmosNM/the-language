#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>


void println(const char *format, ...) {
    va_list args;
    va_start(args, format);

    for (int i = 0; format[i] != '\0'; i++) {
        if (format[i] == '{' && format[i + 1] == '}') {
            i += 1;
            const char *arg = va_arg(args, const char *);
            printf("%s", arg);
        } else {
            putchar(format[i]);
        }
    }

    va_end(args);
    printf("\n");
}


int main() {
    int x = 10;
	if(x == 10){
			println("x is {}", "10");
}
	else{
	println("x is {}", "not 10");
}
return 0;
}
