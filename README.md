### Proyecto #1 Paralela

#### <img src="https://art.ngfiles.com/images/386000/386577_stardoge_8-bit-pokeball.png?f1446737358" alt="Pokeball" width="50" height="50" style="vertical-align: middle;"> Descripción del Proyecto:  

Este proyecto simula el movimiento de sprites (Pokémones y Pokebola) rebotando dentro de una ventana utilizando la biblioteca SDL2. Se implementa tanto una versión serial como una versión paralela usando OpenMP para comparar el rendimiento de ambas versiones. El movimiento de los sprites se paraleliza para aprovechar múltiples núcleos y mejorar la eficiencia en la ejecución.


#### Requisitos:
SDL2
- SDL2_image
- SDL2_ttf
- OpenMP (para la versión paralela)

#### Comandos para Compilar y Ejecutar:

*Versión Serial:*
```bash
gcc -o screensaver screensaver.c -lSDL2 -lSDL2_image -lSDL2_ttf -lm
./screensaver [número_de_sprites]
```

*Versión Paralela (con OpenMP)*
```bash
gcc -o screensaverOM screensaver.c -lSDL2 -lSDL2_image -lSDL2_ttf -lm -fopenmp
./screensaverOM [número_de_sprites]
```

*Ejemplos de ejecución*

```bash
./screensaver 250
```

```bash
./screensaverOM 250
```
