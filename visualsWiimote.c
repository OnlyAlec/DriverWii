#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ncurses.h>
#include <string.h>
#include <time.h>
#include <sys/ioctl.h>

struct winsize size;

/*
 * Ncurses colors
 *     COLOR_BLACK   0
 *     COLOR_RED     1
 *     COLOR_GREEN   2
 *     COLOR_YELLOW  3
 *     COLOR_BLUE    4
 *     COLOR_MAGENTA 5
 *     COLOR_CYAN    6
 *     COLOR_WHITE   7
 */

static void wiimoteWindow()
{
    size_t i = 0;

    clear();
    attron(COLOR_PAIR(2));
    mvprintw(i++, 0, "+===================================================================+ ");
    mvprintw(i++, 0, "|       +-----+                    +---+                            | ");
    mvprintw(i++, 0, "|       |     |         (   )      |   |                            | ");
    mvprintw(i++, 0, "|  +----+     +----+    +---+      +---+        +---+   +---+       | ");
    mvprintw(i++, 0, "|  |               |    |   |                   |   |   |   |       | ");
    mvprintw(i++, 0, "|  +----+     +----+    +---+      +---+        +---+   +---+       | ");
    mvprintw(i++, 0, "|       |     |                    |   |                            | ");
    mvprintw(i++, 0, "|       +-----+                    +---+                            | ");
    mvprintw(i++, 0, "+===================================================================+ ");
    mvprintw(i++, 0, "|  Bateria:              Acelerometro: x:        y:        z:       | ");
    mvprintw(i++, 0, "|  Dispositivo:                     LED:  [ ]  [ ]  [ ]  [ ]        | ");
    mvprintw(i++, 0, "+===================================================================+ ");
    attroff(COLOR_PAIR(2));
    move(100, 0);
}

static int *getCenter(int strLen)
{
    int rows = size.ws_row, cols = size.ws_col;
    int *center = (int *)malloc(sizeof(int) * 2);
    center[0] = (cols - strLen) / 2;
    center[1] = rows / 2;
    return center;
}

static void visualError(char *msg, int boxWidth, int boxHeight)
{
    clear();
    attron(COLOR_PAIR(1));
    int rows = size.ws_row, cols = size.ws_col;
    int startX = (cols - boxWidth) / 2;
    int startY = (rows - boxHeight) / 2;

    for (int i = startY; i < startY + boxHeight; i++)
    {
        mvwprintw(stdscr, i, startX, "*");
        mvwprintw(stdscr, i, startX + boxWidth - 1, "*");
    }
    for (int i = startX; i < startX + boxWidth; i++)
    {
        mvwprintw(stdscr, startY, i, "*");
        mvwprintw(stdscr, startY + boxHeight - 1, i, "*");
    }

    mvprintw(startY + boxHeight / 2, startX + boxWidth / 2 - strlen(msg) / 2, "%s", msg);
    attroff(COLOR_PAIR(1));
    move(0, 0);
    refresh();
}

static void updateWiimoteWindow(char *battery, int *accelerometer, char *mac, char *led)
{
    mvprintw(10, 11, "%s", battery);
    mvprintw(11, 11, "%s", mac);
    mvprintw(11, 33, "%s", led);
}

static void visualKey(char key, int x, int y)
{
    attron(COLOR_PAIR(3));
    mvprintw(x, y, "%c", key);
    attroff(COLOR_PAIR(3));
}

static void visualNotValidKey()
{
    attron(COLOR_PAIR(4));
    char *str = "Boton no valido";
    int *centerText = getCenter(strlen(str));
    mvwprintw(stdscr, 25, centerText[0], str);
    attroff(COLOR_PAIR(4S ||));
}

static void visualAccel(int x, int y, int z)
{
    mvprintw(9, 42, "%4d", x);
    mvprintw(9, 51, "%4d", y);
    mvprintw(9, 61, "%4d", z);
}

static void visualBatetery(int capacity)
{
    int i;

    mvprintw(9, 10, "%3u%%", capacity);

    for (i = 0; i * 10 < capacity; ++i)
        mvprintw(9, 15 + i, "#");
}

static void visualLed(int n, bool state)
{
    mvprintw(10, 43 + (n * 5), "%c", state ? 'x' : ' ');
}

static void visualSetSize(const struct winsize sizeNew)
{
    size = sizeNew;
}

static void initVisuals()
{
    initscr();
    curs_set(0);
    cbreak();
    raw();
    noecho();
    timeout(0);

    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(3, COLOR_CYAN, COLOR_BLACK);
    init_pair(4, COLOR_GREEN, COLOR_BLACK);
    init_pair(5, COLOR_YELLOW, COLOR_BLACK);
    init_pair(6, COLOR_BLUE, COLOR_BLACK);
    init_pair(7, COLOR_WHITE, COLOR_BLACK);

    if (ioctl(0, TIOCGWINSZ, (char *)&size) < 0)
        printf("TIOCGWINSZ error");
}

static void visualStartProgram()
{
    initVisuals();

    char *str = "Bienvenido al programa de control de Wiimote";
    int *centerText = getCenter(strlen(str));

    time_t startTime = time(NULL);
    while (difftime(time(NULL), startTime) < 5)
    {
        clear();
        mvwprintw(stdscr, centerText[1], centerText[0], str);
        attron(COLOR_PAIR(rand() % 7 + 1));
        for (int i = 0; i < size.ws_row; i++)
        {
            mvprintw(i, 0, "*");
            mvprintw(i, size.ws_col - 1, "*");
            usleep(1000);
        }
        for (int i = 0; i < size.ws_col; i++)
        {
            mvprintw(0, i, "*");
            mvprintw(size.ws_row - 1, i, "*");
            usleep(1000);
        }
        attroff(COLOR_PAIR(rand() % 7 + 1));
        refresh();
    }
}