#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Estructura para representar un Pokémon
typedef struct {
    SDL_Texture *texture;
    SDL_Rect position;
    int speed;
} Pokemon;

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

// Función para mover un Pokémon a lo largo de la ruta
void movePokemon(Pokemon *pokemon, int screen_width) {
    pokemon->position.x += pokemon->speed;
    if (pokemon->position.x > screen_width || pokemon->position.x < 0) {
        pokemon->speed = -pokemon->speed; // Cambio de dirección
    }
}

// Función principal
int main(int argc, char *args[]) {
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    const int screen_width = 640;
    const int screen_height = 480;
    const int num_pokemon = 5;

    srand(time(NULL));

    if (!initSDL(&window, &renderer, screen_width, screen_height)) {
        printf("Failed to initialize SDL.\n");
        return -1;
    }

    Pokemon pokemon[num_pokemon];
    const char *spritePaths[num_pokemon] = {
        "pokemon1.png",
        "pokemon1.png",
        "pokemon1.png",
        "pokemon1.png",
        "pokemon1.png"
    };

    // Inicializa cada Pokémon con una textura, posición y velocidad aleatoria
    for (int i = 0; i < num_pokemon; i++) {
        pokemon[i].texture = loadTexture(spritePaths[i], renderer);
        if (pokemon[i].texture == NULL) {
            printf("Failed to load texture %d.\n", i);
            return -1;
        }

        pokemon[i].position.x = rand() % screen_width;
        pokemon[i].position.y = (i * (screen_height / num_pokemon)) + (screen_height / (2 * num_pokemon));
        pokemon[i].position.w = 50; // Ancho del sprite
        pokemon[i].position.h = 50; // Alto del sprite
        pokemon[i].speed = (rand() % 5) + 1; // Velocidad aleatoria entre 1 y 5
    }

    int quit = 0;
    SDL_Event e;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear(renderer);

        for (int i = 0; i < num_pokemon; i++) {
            movePokemon(&pokemon[i], screen_width);
            SDL_RenderCopy(renderer, pokemon[i].texture, NULL, &pokemon[i].position);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16); // Aproximadamente 60 FPS
    }

    for (int i = 0; i < num_pokemon; i++) {
        SDL_DestroyTexture(pokemon[i].texture);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
