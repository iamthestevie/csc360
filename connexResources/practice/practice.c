#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>

int main(void)
{
    char *input = NULL;
    char *prompt = "your command:";

    input = readline(prompt);

    printf("%s\n", input);

    return 0;
}