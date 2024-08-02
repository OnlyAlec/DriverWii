# Proyecto - Wiimote en linux

Proyecto universitario para la materia de Sistemas Operativos II, este proyecto permite la interacción con un control Wiimote utilizando una interfaz gráfica en la terminal usando la conexión bluetooth.
Una de las utilidades de este proyecto es para la prueba de un driver desarrollado para Linux, usando la menor cantidad de código para poder desplegar y mostrar el input del control.

## Uso

Recordemos de que se ocupo lo minimo de código para la ejecución del código, con los archivos que se encuentran en el repositorio es suficiente para su compilación y ejecución del proyecto. Lo unico a considerar es que todo se probo en un entorno de Linux (Arch Linux).

1. Clona el repositorio en tu equipo,

2. Compila el proyecto, si esta usando VS Code puede compilar con el siguiente `task.json`:
    ```json
    {
        "type": "cppbuild",
        "label": "Wiimote Build",
        "command": "/usr/bin/gcc",
        "args": [
            "-fdiagnostics-color=always",
            "-I${fileDirname}/libs/",
            "${fileDirname}/libs/*.c",
            "-D'XWII__EXPORT=__attribute__((visibility(\"default\")))'",
            "-g",
            "${fileDirname}/mainWiimote.c",
            "-o",
            "/tmp/mainWiimote",
            "-lncurses",
            "-ludev",
        ],
        "options": {
            "cwd": "${fileDirname}"
        },
    }
   ```
3. Conecta el control antes de ejecutar el binario, asegurate que la conexión sea correcta.
4. Ejecuta el binario generado:
    ```bash
    ./mainWiimote
    ```
5. Una terminal se abrira y se mostrara un wiimote dibujado en la terminal si identificación del control fue correcta.

## Estructura del Proyecto

El proyecto tiene la siguiente estructura:
- `mainWiimote.c`: Contiene la función principal y la lógica de inicialización del programa.
- `handleInputWiimote.c`: Maneja la entrada de datos del Wiimote, usando la libreria correspondiente.
- `visualsWiimote.c`: Contiene las funciones para la interfaz gráfica en la terminal.
- `/libs`: Contiene archivos de biblioteca y encabezados necesarios para la interacción con el Wiimote.

## Contribución.

Si deseas contribuir al proyecto, sigue estos pasos:

1. Haz un fork del repositorio.
2. Crea una nueva rama (`git checkout -b feature/nueva-funcionalidad`).
3. Realiza tus cambios y haz commit (`git commit -am 'Añadir nueva funcionalidad'`).
4. Sube tus cambios (`git push origin feature/nueva-funcionalidad`).
5. Abre un Pull Request.

## Licencia

Este proyecto está licenciado bajo la Licencia MIT. Consulta el archivo LICENSE para más detalles.