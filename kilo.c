#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

struct termios orig_termios; // to store the original terminal attributes

void disableRawMode(void) { // disable raw mode when the program exits
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios); // restore the original terminal attributes
}

void enableRawMode(void) { // enable raw mode for the terminal
    tcgetattr(STDIN_FILENO, &orig_termios); // get the current terminal attributes and store them in orig_termios
    atexit(disableRawMode); // register the disableRawMode function to be called when the program exits

    struct termios raw = orig_termios; // create a copy of the original terminal attributes to modify
    raw.c_iflag &= ~(BRKINT| ICRNL | INPCK | ISTRIP | IXON); // disable break condition, carriage return to newline translation, parity checking, stripping of the eighth bit, and software flow control
    raw.c_oflag &= ~(OPOST); // disable output processing (e.g., converting newline to carriage return + newline)   
    raw.c_oflag &= ~(CS8); // set character size to 8 bits per byte
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG); // disable echoing, canonical mode, and signal generation 
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw); // set the modified terminal attributes to enable raw mode
}


int main(void) {
    enableRawMode();
    char c;

    while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q') { // read input until 'q' is pressed
        if (iscntrl(c)) { // if the character is a control character, print its ASCII value
            printf("%d\r\n", c);
        } else {
            printf("%d ('%c')\r\n", c, c); // print the ASCII value and the character itself
        }
    }
    return 0;
}
