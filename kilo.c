/*** Includes ***/
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <errno.h>

/*** defines ***/
#define CTRL_KEY(k) ((k) & 0x1f) // macro to convert a character to its corresponding control key by performing a bitwise AND operation with 0x1f (which is 31 in decimal, representing the lower 5 bits of the character's ASCII value)

/*** Data ***/
struct EditorConfig {
    struct termios orig_termios; // structure to store the original terminal attributes, which will be used to restore the terminal settings when the program exits
};

struct EditorConfig E;

/*** terminal funcs ***/
void die(const char *s) {
    write(STDIN_FILENO, "\x1b[2J", 4);
    write(STDIN_FILENO, "\x1b[H", 3);

    perror(s);
    exit(1);
}

void disableRawMode(void) { // disable raw mode when the program exits
    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.orig_termios) == -1) die("tcsetattr"); // restore the original terminal attributes, if tcsetattr() returns -1, call die() to handle the error
}

void enableRawMode(void) { // enable raw mode for the terminal
    if (tcgetattr(STDIN_FILENO, &E.orig_termios) == -1) die("tcgetattr"); // get the current terminal attributes and store them in orig_termios
    atexit(disableRawMode); // register the disableRawMode function to be called when the program exits

    struct termios raw = E.orig_termios; // create a copy of the original terminal attributes to modify
    raw.c_iflag &= ~(BRKINT| ICRNL | INPCK | ISTRIP | IXON); // disable break condition, carriage return to newline translation, parity checking, stripping of the eighth bit, and software flow control
    raw.c_oflag &= ~(OPOST); // disable output processing (e.g., converting newline to carriage return + newline)   
    raw.c_oflag &= ~(CS8); // set character size to 8 bits per byte
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG); // disable echoing, canonical mode, and signal generation 
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr"); // apply the modified terminal attributes
    raw.c_cc[VMIN] = 0; // set the minimum number of bytes of input needed before read() returns
    raw.c_cc[VTIME] = 1 ; // set the timeout for read() to 100 milliseconds (1 decisecond)
}

char editorReadKey(void) { // read a keypress and return it, handling escape sequences
    int nread;
    char c = '\0';
    while((nread = read(STDIN_FILENO, &c, 1))  != 1) {
        if (nread == -1 && errno != EAGAIN) die("read");
    }
    return c;
}

/*** output ***/
void editorDrawRows(void) { // draw the rows of the editor
    int y;
    for (y = 0; y < 24; y++) {
        write(STDIN_FILENO, "~\r\n", 3); // write a tilde followed by a carriage return and newline to the terminal for each row, which is 3 bytes long
    }
}

void editorRefreashScreen(void) {
    write(STDIN_FILENO, "\x1b[2J", 4); // clear the screen by sending the escape sequence "\x1b[2J" to the terminal, which is 4 bytes long
    write(STDIN_FILENO, "\x1b[H", 3); // move the cursor to the top-left corner of the screen by sending the escape sequence "\x1b[H" to the terminal, which is 3 bytes long

    editorDrawRows();
    write(STDIN_FILENO, "\x1b[H", 3); // move the cursor to the top-left corner of the screen by sending the escape sequence "\x1b[H" to the terminal, which is 3 bytes long
}

/*** input process ***/
void editorProcessKeypress(void) { // read a keypress and process it
    char c = editorReadKey();
    //screen clear poc
    // printf("%d ('%c')\r\n", c, c);
    switch(c) {
        case CTRL_KEY('e'): 
            write(STDIN_FILENO, "\x1b[2J", 4);
            write(STDIN_FILENO, "\x1b[H", 3);
            exit(0); 
            break;
    }
}

/*** init ***/
int main(void) {
    enableRawMode();
    while (1) {
        editorRefreashScreen(); // comment this out for screen clear poc
        editorProcessKeypress();
    }
    return 0;
}
