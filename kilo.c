#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

struct termios orig_termios; // to store the original terminal attributes

void disableRawMode(void) { // disable raw mode when the program exits
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enableRawMode(void) { // enable raw mode for the terminal
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disableRawMode);

    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}


int main(void) {
    enableRawMode();
    char c;

    while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q') { // read input until 'q' is pressed
        if (iscntrl(c)) { // if the character is a control character, print its ASCII value
            printf("%d\n", c);
        } else {
            printf("%d ('%c')\n", c, c); // print the ASCII value and the character itself
        }
    }
    return 0;
}
