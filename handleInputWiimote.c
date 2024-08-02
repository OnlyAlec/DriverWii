//! Se quiere llenar un buffer de 5 espacios con pulsaciones del control
//! De igual manera, en otro buffer llenar con los datos del acelerometro
//! El buffer debe de esperar como maximo 1 segundo
//! Este lleno el buffer o no se debe de imprimir el buffer en pantalla

//^ 1. Esperar a que exista Input del control
//^ 2. Leer el Input y definir que boton se presiono
//^ 3. Llenar un buffer con el Input reconocido
//^ 4. Esperar si existe mas Input repitiendo desde el paso 2.
//^ 5. Enviar el buffer a la pantalla
//^ 6. Recolectar datos del acelerometro
//^ 7. Llenar un buffer con los datos del acelerometro
//^ 8. Enviar el buffer a la pantalla

#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 5
#define DEBOUNCE_THRESHOLD 0.5

typedef struct btnState
{
    char key;
    time_t lastPress;
} btnState;

char controlBuffer[BUFFER_SIZE];
int32_t accelerometerBuffer[BUFFER_SIZE];
int controlIndex = 0;
int accelerometerIndex = 0;

btnState *btnsStates = NULL;
int lastState = 0;

static void clearBuffer(char *buffer)
{
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        buffer[i] = '\0';
    }
    controlIndex = 0;
}

static char keyIdentified(const struct xwii_event *event)
{
    unsigned int code = event->v.key.code;

    if (code == XWII_KEY_LEFT)
        return 'L';
    else if (code == XWII_KEY_RIGHT)
        return 'R';
    else if (code == XWII_KEY_UP)
        return 'U';
    else if (code == XWII_KEY_DOWN)
        return 'D';
    else if (code == XWII_KEY_A)
        return 'A';
    else if (code == XWII_KEY_B)
        return 'B';
    else if (code == XWII_KEY_HOME)
        return 'H';
    else if (code == XWII_KEY_MINUS)
        return '-';
    else if (code == XWII_KEY_PLUS)
        return '+';
    else if (code == XWII_KEY_ONE)
        return '1';
    else if (code == XWII_KEY_TWO)
        return '2';
}

void printInput(const struct xwii_event *event, char key)
{
    char str = ' ';
    if (event->v.key.state == 1)
        str = key;

    switch (key)
    {
    case 'A':
        visualKey(str, 4, 26);
        break;
    case 'B':
        visualKey(str, 2, 26);
        break;
    case '+':
        visualKey(str, 2, 37);
        break;
    case '-':
        visualKey(str, 6, 37);
        break;
    case '1':
        visualKey(str, 4, 50);
        break;
    case '2':
        visualKey(str, 4, 58);
        break;
    case 'U':
        visualKey(str, 4, 5);
        break;
    case 'D':
        visualKey(str, 4, 17);
        break;
    case 'L':
        visualKey(str, 6, 11);
        break;
    case 'R':
        visualKey(str, 2, 11);
        break;
    default:
        visualNotValidKey("Botón no reconocido", 15, 8);
        break;
    }
}

static int canProcessButtonPress(char buttonKey)
{
    time_t now = time(NULL);
    for (int i = 0; i < lastState; i++)
    {
        if (btnsStates[i].key == buttonKey)
        {
            double t = difftime(now, btnsStates[i].lastPress);
            if (t >= DEBOUNCE_THRESHOLD)
            {
                btnsStates[i].lastPress = now;
                return 1;
            }
            else
            {
                return 0;
            }
        }
    }
    //* Registrar el botón en el buffer de estados
    btnState *temp = realloc(btnsStates, (lastState + 1) * sizeof(btnState));
    if (temp == NULL)
        printf("Error: No se pudo reservar memoria para el buffer de estados\n");
    else
    {
        btnsStates = temp;
        btnsStates[lastState].key = buttonKey;
        btnsStates[lastState].lastPress = now;
        lastState++;
    }
    return 1;
}

int runHandleInput(const struct xwii_event *event)
{
    // if (canProcessButtonPress(keyIdentified(event)))
    // {
    char key = keyIdentified(event);
    printInput(event, key);
    return 1;
    // }
    // return 0;
}

int runHandleAccel(const struct xwii_event *event)
{
    accelerometerBuffer[0] = event->v.abs[0].x;
    accelerometerBuffer[1] = event->v.abs[0].y;
    accelerometerBuffer[2] = event->v.abs[0].z;
}

void printAccelInput()
{
    visualAccel(accelerometerBuffer[0], accelerometerBuffer[1], accelerometerBuffer[2]);
}