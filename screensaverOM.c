#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <SDL2/SDL_ttf.h>
#include <omp.h>

#define MIN_SPEED 4
#define MAX_SPEED 8
#define POKEBALL_SPEED 6
#define ANGLE_ADJUSTMENT 0.2

// Estructura para representar un Pokémon
typedef struct {
    SDL_Texture *texture;
    SDL_Rect position;
    float speed;
    float angle;
    int isCaught;
} Pokemon;

// Función para generar un color aleatorio
void generateRandomBackgroundColor(Uint8 *r, Uint8 *g, Uint8 *b) {
    *r = rand() % 256;
    *g = rand() % 256;
    *b = rand() % 256;
}

// Función para inicializar SDL
int initSDL(SDL_Window **window, SDL_Renderer **renderer, int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 0;
    }

    *window = SDL_CreateWindow("Pokemon Screensaver", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
    if (*window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return 0;
    }

    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
    if (*renderer == NULL) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return 0;
    }

    return 1;
}

// Función para cargar una imagen como textura
SDL_Texture* loadTexture(const char *path, SDL_Renderer *renderer) {
    SDL_Texture *newTexture = NULL;
    SDL_Surface *loadedSurface = IMG_Load(path);
    if (loadedSurface == NULL) {
        printf("Unable to load image %s! SDL_image Error: %s\n", path, IMG_GetError());
    } else {
        newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
        SDL_FreeSurface(loadedSurface);
    }

    return newTexture;
}

// Función para mover un Pokémon en un ángulo
void movePokemon(Pokemon *pokemon, int screen_width, int screen_height) {
    // Calcula el nuevo desplazamiento
    pokemon->position.x += (int)(pokemon->speed * cos(pokemon->angle));
    pokemon->position.y += (int)(pokemon->speed * sin(pokemon->angle));

    // Detecta colisiones con los bordes
    if (pokemon->position.x <= 0) {
        pokemon->position.x = 1; // Mover ligeramente hacia dentro de la pantalla
        pokemon->angle = M_PI - pokemon->angle; // Rebote en el eje X
    } else if (pokemon->position.x + pokemon->position.w >= screen_width) {
        pokemon->position.x = screen_width - pokemon->position.w - 1;
        pokemon->angle = M_PI - pokemon->angle;
    }

    if (pokemon->position.y <= 0) {
        pokemon->position.y = 1;
        pokemon->angle = -pokemon->angle; // Rebote en el eje Y
    } else if (pokemon->position.y + pokemon->position.h >= screen_height) {
        pokemon->position.y = screen_height - pokemon->position.h - 1;
        pokemon->angle = -pokemon->angle;
    }

    // Solo ajustar si el ángulo está cerca de los ejes (0, 90, 180, 270 grados)
    if (fabs(fmod(pokemon->angle, M_PI / 2)) < 0.1) {
        float random_adjustment = ((float)rand() / RAND_MAX) * ANGLE_ADJUSTMENT - (ANGLE_ADJUSTMENT / 2);
        pokemon->angle += random_adjustment;
    }

    // Asegurar que el ángulo esté en el rango [0, 2π]
    if (pokemon->angle < 0) {
        pokemon->angle += 2 * M_PI;
    } else if (pokemon->angle >= 2 * M_PI) {
        pokemon->angle -= 2 * M_PI;
    }

    // Si la velocidad baja de MIN_SPEED, asignar una nueva velocidad aleatoria entre MIN_SPEED y MAX_SPEED
    if (pokemon->speed < MIN_SPEED) {
        pokemon->speed = (rand() % (MAX_SPEED - MIN_SPEED + 1)) + MIN_SPEED;
    }
}

void movePokeball(Pokemon *pokeball, int screen_width, int screen_height) {
    // Calcula el nuevo desplazamiento
    pokeball->position.x += (int)(pokeball->speed * cos(pokeball->angle));
    pokeball->position.y += (int)(pokeball->speed * sin(pokeball->angle));

    // Detecta colisiones con los bordes
    if (pokeball->position.x <= 0) {
        pokeball->position.x = 1; // Mover ligeramente hacia dentro de la pantalla
        pokeball->angle = M_PI - pokeball->angle; // Rebote en el eje X
    } else if (pokeball->position.x + pokeball->position.w >= screen_width) {
        pokeball->position.x = screen_width - pokeball->position.w - 1;
        pokeball->angle = M_PI - pokeball->angle;
    }

    if (pokeball->position.y <= 0) {
        pokeball->position.y = 1;
        pokeball->angle = -pokeball->angle; // Rebote en el eje Y
    } else if (pokeball->position.y + pokeball->position.h >= screen_height) {
        pokeball->position.y = screen_height - pokeball->position.h - 1;
        pokeball->angle = -pokeball->angle;
    }

    // Solo ajustar si el ángulo está cerca de los ejes (0, 90, 180, 270 grados)
    if (fabs(fmod(pokeball->angle, M_PI / 2)) < 0.1) {
        float random_adjustment = ((float)rand() / RAND_MAX) * ANGLE_ADJUSTMENT - (ANGLE_ADJUSTMENT / 2);
        pokeball->angle += random_adjustment;
    }

    // Asegurar que el ángulo esté en el rango [0, 2π]
    if (pokeball->angle < 0) {
        pokeball->angle += 2 * M_PI;
    } else if (pokeball->angle >= 2 * M_PI) {
        pokeball->angle -= 2 * M_PI;
    }
}

float generateRandomAngle() {
    float angle;
    do {
        angle = (float)(rand() % 360) * (M_PI / 180.0); // Ángulo aleatorio en radianes
    } while (fabs(fmod(angle, M_PI / 2)) < 0.1); // Evitar ángulos cercanos a 0, 90, 180, 270 grados
    return angle;
}

// Función para detectar colisiones entre la pokebola y un Pokémon
int checkCollision(Pokemon *pokeball, Pokemon *pokemon) {
    return !(
        pokeball->position.x + pokeball->position.w <= pokemon->position.x ||
        pokeball->position.x >= pokemon->position.x + pokemon->position.w ||
        pokeball->position.y + pokeball->position.h <= pokemon->position.y ||
        pokeball->position.y >= pokemon->position.y + pokemon->position.h
    );
}

// Función principal
int main(int argc, char *args[]) {
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    const int screen_width = 980;
    const int screen_height = 720;
    
    // Verificar que se haya pasado un argumento para el número de pokemons
    if (argc < 2) {
        printf("Usage: %s <number_of_pokemons>\n", args[0]);
        return -1;
    }

    // Convertir el argumento a un entero
    int num_pokemon = atoi(args[1]);

    if (num_pokemon <= 0) {
        printf("Invalid number of pokemons. Must be a positive integer.\n");
        return -1;
    }

    srand(time(NULL));

    if (!initSDL(&window, &renderer, screen_width, screen_height)) {
        printf("Failed to initialize SDL.\n");
        return -1;
    }

    if (TTF_Init() == -1) {
        printf("SDL_ttf could not initialize! TTF_Error: %s\n", TTF_GetError());
        return -1;
    }

    TTF_Font *font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 24); 
    if (font == NULL) {
        printf("Failed to load font! TTF_Error: %s\n", TTF_GetError());
        return -1;
    }

    Uint8 bg_r = 0, bg_g = 0, bg_b = 0; 
    Pokemon pokemon[num_pokemon];
    char spritePath[31]; // cantidad de sprites de pokemons
    
    // Inicializa cada Pokémon con una textura, posición, velocidad y ángulo aleatorio
    for (int i = 0; i < num_pokemon; i++) {
        int random_number = (rand() % 30) + 1; // Número aleatorio entre 1 y 30
        snprintf(spritePath, sizeof(spritePath), "pokemons/pokemon%d.png", random_number);
        pokemon[i].texture = loadTexture(spritePath, renderer);
        if (pokemon[i].texture == NULL) {
            printf("Failed to load texture %d.\n", i);
            return -1;
        }

        pokemon[i].position.x = rand() % screen_width;
        pokemon[i].position.y = rand() % screen_height;
        pokemon[i].position.w = 60; // Ancho del sprite
        pokemon[i].position.h = 60; // Alto del sprite
        pokemon[i].speed = (rand() % (MAX_SPEED - MIN_SPEED + 1)) + MIN_SPEED; // Velocidad aleatoria entre el rango
        pokemon[i].angle = generateRandomAngle(); // Ángulo aleatorio en radianes
        pokemon[i].isCaught = 0;
    }

    // Inicializar la pokebola
    Pokemon pokeball;
    pokeball.texture = loadTexture("pokeball.png", renderer);
    if (pokeball.texture == NULL) {
        printf("Failed to load pokeball texture.\n");
        return -1;
    }

    pokeball.position.x = rand() % screen_width;
    pokeball.position.y = rand() % screen_height;
    pokeball.position.w = 50; // Ancho del sprite de la pokebola
    pokeball.position.h = 50; // Alto del sprite de la pokebola
    pokeball.speed = POKEBALL_SPEED; // Velocidad fija de la pokebola
    pokeball.angle = generateRandomAngle();

    int quit = 0;
    SDL_Event e;

    Uint64 start_time = SDL_GetPerformanceCounter();
    Uint64 end_time;
    double elapsed_ms;

    int frame_count = 0;
    double fps = 0.0;

    Uint64 total_time = 0;
    int iterations_to_measure = 100;  // Puedes ajustar esto según sea necesario

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            }
        }

        SDL_SetRenderDrawColor(renderer, bg_r, bg_g, bg_b, 0xFF);
        SDL_RenderClear(renderer);

        Uint64 frame_start_time = SDL_GetPerformanceCounter();

        // Mover y dibujar la pokebola
        movePokeball(&pokeball, screen_width, screen_height);

        // Paralelizar el bucle de movimiento de los Pokémon
        #pragma omp parallel for
        for (int i = 0; i < num_pokemon; i++) {
            if (!pokemon[i].isCaught && checkCollision(&pokeball, &pokemon[i])) {
                #pragma omp critical
                {
                    pokemon[i].isCaught = 1; // Atrapado
                    generateRandomBackgroundColor(&bg_r, &bg_g, &bg_b);
                }
            }
            movePokemon(&pokemon[i], screen_width, screen_height);
            // Aquí no necesitamos renderizar en paralelo, lo haremos en el hilo principal
        }

        // Renderizado en el hilo principal
        for (int i = 0; i < num_pokemon; i++) {
            if (!pokemon[i].isCaught) {
                SDL_RenderCopy(renderer, pokemon[i].texture, NULL, &pokemon[i].position);
            }
        }

        SDL_RenderCopy(renderer, pokeball.texture, NULL, &pokeball.position);

        // Cálculo de FPS (ahora se realiza en cada frame)
        end_time = SDL_GetPerformanceCounter();
        fps = (double)SDL_GetPerformanceFrequency() / (end_time - start_time);
        start_time = end_time;

        char fps_text[20];
        sprintf(fps_text, "FPS: %.2f", fps);
        SDL_Color textColor = {255, 255, 255, 0}; 
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, fps_text, textColor);
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_Rect textRect = {10, 10, textSurface->w, textSurface->h};
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);

        SDL_RenderPresent(renderer);
        SDL_Delay(16); // Aproximadamente 60 FPS

        Uint64 frame_end_time = SDL_GetPerformanceCounter();
        elapsed_ms = (double)((frame_end_time - frame_start_time) * 1000) / SDL_GetPerformanceFrequency();
        total_time += (frame_end_time - frame_start_time);

        frame_count++;
        if (frame_count >= iterations_to_measure) {
            double average_time = (double)(total_time * 1000) / (iterations_to_measure * SDL_GetPerformanceFrequency());
            printf("Tiempo promedio de ejecución para %d iteraciones: %.2f ms\n", iterations_to_measure, average_time);
            total_time = 0; // Reiniciar el tiempo total
            frame_count = 0; // Reiniciar el contador de frames
        }
    }

    for (int i = 0; i < num_pokemon; i++) {
        SDL_DestroyTexture(pokemon[i].texture);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
