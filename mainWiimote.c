#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <poll.h>
#include <errno.h>
#include <ncurses.h>
#include <time.h>

#include "libs/libWiimote.h"
#include "visualsWiimote.c"
#include "handleInputWiimote.c"

#define G_ERROR -1
#define true 1
#define false 0

static bool ledState[4];
static struct xwii_iface *iface;
struct winsize sizeOld, sizeNew;

static char *get_dev(int num)
{
    struct xwii_monitor *mon;
    char *ent;
    int i = 0;

    mon = xwii_monitor_new(false, false);
    if (!mon)
    {
        visualError("No pude crear el monitor\n", 20, 8);
        return NULL;
    }

    while ((ent = xwii_monitor_poll(mon)))
    {
        if (++i == num)
            break;
        free(ent);
    }

    xwii_monitor_unref(mon);

    if (!ent)
        visualError("No pude encontrar el dispositivo especifico", 50, 8);

    return ent;
}

static void batteryRefresh(void)
{
    int ret;
    uint8_t capacity;

    ret = xwii_iface_get_battery(iface, &capacity);
    if (ret)
        visualError("No pude leer datos de la bateria", 20, 8);
    else
        visualBatetery(capacity);
}

static void ledRefresh(void)
{
    int ret;
    for (int n = 0; n < 4; n++)
    {
        ret = xwii_iface_get_led(iface, XWII_LED(n + 1), &ledState[n]);
        if (ret)
            visualError("No pude leer el estado de los leds", 20, 8);
        else
            visualLed(n, ledState[n]);
    }
}

static void devRefresh(void)
{
    int ret;
    char *name;

    ret = xwii_iface_get_devtype(iface, &name);
    if (ret)
    {
        visualError("No pude obtener el tipo de dispositivo", 20, 8);
    }
    else
    {
        mvprintw(10, 18, "%s", name);
        free(name);
    }
}

static void refreshInfo()
{
    batteryRefresh();
    ledRefresh();
    devRefresh();
}

static void redraw()
{
    clear();
    wiimoteWindow();
    refreshInfo();
    refresh();
}

void handleResizeTerminal()
{
    sizeOld = sizeNew;
    if (ioctl(0, TIOCGWINSZ, (char *)&sizeNew) < 0)
        printf("TIOCGWINSZ error");

    if (sizeOld.ws_row != sizeNew.ws_row || sizeOld.ws_col != sizeNew.ws_col)
    {
        redraw();
        refreshInfo();
        visualSetSize(sizeNew);
    }
}
static void handle_watch(void)
{
    static unsigned int num;
    int ret;

    printf("Info: Watch Event #%u", ++num);

    ret = xwii_iface_open(iface, xwii_iface_available(iface) | XWII_IFACE_WRITABLE);
    if (ret)
        printf("Error: Cannot open interface: %d", ret);

    refreshInfo();
}

static int keyboard(void)
{
    int key;

    key = wgetch(stdscr);
    if (key == G_ERROR)
        return 0;
    if (key == 'q')
        return -ECANCELED;

    return 0;
}

static int checkControllerConnection()
{
    struct xwii_monitor *mon;
    char *ent;
    int num = 0;

    mon = xwii_monitor_new(false, false);
    if (!mon)
    {
        visualError("Cannot create monitor\n", 20, 8);
        return -EINVAL;
    }

    while ((ent = xwii_monitor_poll(mon)))
    {
        printf("Found device #%d: %s\n", ++num, ent);
        free(ent);
        return num;
    }
    xwii_monitor_unref(mon);
    return G_ERROR;
}

static int run(struct xwii_iface *iface)
{
    // time_t start = 0, now = 0;
    struct timespec start, now;
    double elapsed;
    int reset = 0;

    struct xwii_event event;
    int ret = 0, fds_num;
    struct pollfd fds[2];

    memset(fds, 0, sizeof(fds));
    fds[0].fd = 0;
    fds[0].events = POLLIN;
    fds[1].fd = xwii_iface_get_fd(iface);
    fds[1].events = POLLIN;
    fds_num = 2;

    ret = xwii_iface_watch(iface, true);
    if (ret)
        visualError("Error: Cannot initialize hotplug watch descriptor", 50, 8);

    while (true)
    {
        ret = poll(fds, fds_num, -1);
        if (ret < 0 && errno != EINTR)
        {
            ret = -errno;
            visualError("No se ha podido leer el control", 40, 8);
            printf("Fds: %d", ret);
            break;
        }

        ret = xwii_iface_dispatch(iface, &event, sizeof(event));
        if (ret)
        {
            if (ret != -EAGAIN)
            {
                visualError("No se ha podido leer el control", 40, 8);
                printf("Error:%d", ret);
                break;
            }
        }

        switch (event.type)
        {
        case XWII_EVENT_GONE:
            printf("Info: Adiuuu Wiimote");
            fds[1].fd = -1;
            fds[1].events = 0;
            fds_num = 1;
            break;
        case XWII_EVENT_WATCH:
            handle_watch();
            break;
        case XWII_EVENT_KEY:
            runHandleInput(&event);
            break;
        case XWII_EVENT_ACCEL:
            runHandleAccel(&event);
            printAccelInput();
            break;
        case XWII_EVENT_IR:
            break;
        case XWII_EVENT_MOTION_PLUS:
            runHandleAccel(&event);
            break;
        case XWII_EVENT_NUNCHUK_KEY:
        case XWII_EVENT_NUNCHUK_MOVE:
            break;
        }
        handleResizeTerminal();
        refresh();

        ret = keyboard();
        if (ret == -ECANCELED)
            return 0;
    }
}

int main()
{
    initVisuals();
    visualStartProgram();

    while (true)
    {
        int status = 0;
        char numController = G_ERROR, *path = NULL;

        //* 1. Saber si hay un control listo para ser usado
        status = checkControllerConnection();
        if (status == -EINVAL || status == G_ERROR)
        {
            visualError("No se ha encontrado un control", 40, 8);
            return status;
        }

        path = get_dev(status);
        if (!path)
        {
            visualError("No se ha encontrado la ruta del control", 50, 10);
            return -EINVAL;
        }

        status = xwii_iface_new(&iface, path);
        if (status)
        {
            visualError("No se ha podido crear la interfaz del control", 40, 10);
            return status;
        }

        //* 2. Abrir el control para lectura y escritura
        status = xwii_iface_open(iface, xwii_iface_available(iface) | XWII_IFACE_WRITABLE);
        if (status)
            visualError("No se ha podido abrir la interfaz del control", 40, 10);

        //* 3. Controlar el tamaño de la terminal
        wiimoteWindow();
        refreshInfo();
        //* 4. Correr el control
        status = run(iface);
        if (!status)
        {
            xwii_iface_unref(iface);
            endwin();
            return status;
        }
    }
}

// TODO: Cuando hay un error debe de terminar el programa
//*      (A excepcion del boton invalido, crea mejor otra funcion para manejarlo)
// TODO: Controlar la señal SIGWINCH para redimensionar la terminal
