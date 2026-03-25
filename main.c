#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdlib.h>
#include <time.h>

#define WIDTH 800
#define HEIGHT 480

#define TOP_BAR 50
#define BUTTON_HEIGHT 80
#define MAX_N 100

// ---------------- STRUCT ----------------
typedef struct {
    char name[32];
    int size;
    int fps;
    void (*init)(int*);
    void (*step)(int*, int* done, int* size);
    void (*get_color)(int index, int size, Uint8*, Uint8*, Uint8*);
} Algo;

// ---------------- GLOBAL ----------------
int arr[MAX_N];

SDL_Rect action_btn = {0, HEIGHT - BUTTON_HEIGHT, WIDTH, BUTTON_HEIGHT};
SDL_Rect sleep_btn = {WIDTH - 160, 5, 150, 40};

int sleep_mode = 0;

Uint32 finish_time = 0;
int auto_restart_delay = 5000; // ms (5 secondes)

//tout les algos
int finish_anim = 0;
int finish_index = 0;

// bubble state
int i_bubble = 0;
int j_bubble = 0;

//selection state;
int i_selection = 0;
int j_selection = 0;
int min_index = 0;

//staline state
int i_stalin = 1;
int last_kept = 0;
int current_size;

//thanos state
int thanos_active = 0;
int thanos_target = 0;

//insertion state
int i_insertion = 1;
int j_insertion = 1;
int key;
int inserting = 0;

//cocktail state
int start_cocktail = 0;
int end_cocktail = 0;
int i_cocktail = 0;
int direction = 1;

//gnome state
int i_gnome = 0;


// ---------------- UTILS ----------------
int is_sorted(int size) {
    for (int i = 0; i < size - 1; i++)
        if (arr[i] > arr[i + 1]) return 0;
    return 1;
}

void shuffle(int size) {
    for (int i = size - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int tmp = arr[i];
        arr[i] = arr[j];
        arr[j] = tmp;
    }
}

int in_rect(SDL_Rect r, int x, int y) {
    return (x >= r.x && x <= r.x + r.w &&
            y >= r.y && y <= r.y + r.h);
}

// ---------------- BOGO ----------------
void bogo_init(int* size) {}

void bogo_step(int* arr, int* done, int* size) {
    if (is_sorted(*size)) {
        *done = 1;
        return;
    }
    shuffle(*size);
}

void default_color(int i, int size, Uint8* r, Uint8* g, Uint8* b) {
    *r = 255; *g = 255; *b = 255;
}

// ---------------- BUBBLE ----------------
void bubble_init(int* size) {
    i_bubble = 0;
    j_bubble = 0;
}

void bubble_step(int* arr, int* done, int* size) {
    if (i_bubble >= *size) {
        *done = 1;
        return;
    }

    if (j_bubble < *size - i_bubble - 1) {
        if (arr[j_bubble] > arr[j_bubble + 1]) {
            int tmp = arr[j_bubble];
            arr[j_bubble] = arr[j_bubble + 1];
            arr[j_bubble + 1] = tmp;
        }
        j_bubble++;
    } else {
        j_bubble = 0;
        i_bubble++;
    }
}

void bubble_color(int i, int size, Uint8* r, Uint8* g, Uint8* b){

    // éléments déjà triés (à droite)
    if (i >= size - i_bubble) {
        *r = 0; *g = 150; *b = 255; // bleu
    }

    // éléments en cours de comparaison
    else if (i == j_bubble || i == j_bubble + 1) {
        *r = 255; *g = 0; *b = 0; // rouge
    }

    // normal
    else {
        *r = 255; *g = 255; *b = 255;
    }
}

// ---------------- SELECTION SORT ----------------
void selection_init(int* size) {
    i_selection = 0;
    j_selection = 1;
    min_index = 0;
}

void selection_step(int* arr, int* done, int* size) {
    if (i_selection >= *size - 1) {
        *done = 1;
        return;
    }

    if (j_selection < *size) {
        if (arr[j_selection] < arr[min_index]) {
            min_index = j_selection;
        }
        j_selection++;
    } else {
        // swap
        int tmp = arr[i_selection];
        arr[i_selection] = arr[min_index];
        arr[min_index] = tmp;

        // passer à l’élément suivant
        i_selection++;
        min_index = i_selection;
        j_selection = i_selection + 1;
    }
}

void selection_color(int i, int size, Uint8* r, Uint8* g, Uint8* b) {

    // déjà trié
    if (i < i_selection) {
        *r = 0; *g = 150; *b = 255; // bleu
    }

    // position actuelle
    else if (i == i_selection) {
        *r = 255; *g = 0; *b = 0; // rouge
    }

    // élément en comparaison
    else if (i == j_selection) {
        *r = 255; *g = 100; *b = 0; // orange
    }

    // minimum trouvé
    else if (i == min_index) {
        *r = 0; *g = 255; *b = 0; // vert
    }

    // normal
    else {
        *r = 255; *g = 255; *b = 255;
    }
}

// ---------------- STALINE SORT ----------------
void stalin_init(int* size) {
    i_stalin = 1;
    last_kept = 0;
    current_size = *size;
}

void stalin_step(int* arr, int* done, int* size) {

    if (i_stalin >= *size) {
        *done = 1;
        return;
    }

    if (arr[i_stalin] >= arr[last_kept]) {
        last_kept = i_stalin;
        i_stalin++;
    } else {
        // suppression
        for (int j = i_stalin; j < *size - 1; j++) {
            arr[j] = arr[j + 1];
        }
        (*size)--;
    }
}

void stalin_color(int i, int size, Uint8* r, Uint8* g, Uint8* b) {

    if (i == i_stalin) {
        *r = 255; *g = 0; *b = 0; // élément analysé
    }
    else if (i <= last_kept) {
        *r = 0; *g = 150; *b = 255; // validé
    }
    else {
        *r = 100; *g = 100; *b = 100; // "condamné"
    }
}

// ---------------- THANOS SORT ----------------
void thanos_init(int* size) {
    thanos_active = 0;
}

void thanos_step(int* arr, int* done, int* size) {

    // déjà trié
    if (is_sorted(*size)) {
        *done = 1;
        return;
    }

    // lancer une phase de snap
    if (!thanos_active) {
        thanos_target = *size / 2;
        thanos_active = 1;
    }

    // suppression progressive
    if (*size > thanos_target) {

        int index = rand() % (*size);

        for (int i = index; i < *size - 1; i++) {
            arr[i] = arr[i + 1];
        }

        (*size)--;

    } else {
        // phase terminée → recommencer si pas trié
        thanos_active = 0;
    }
}

void thanos_color(int i, int size, Uint8* r, Uint8* g, Uint8* b) {

    if (i % 2 == 0) {
        *r = 180; *g = 0; *b = 255; // violet stylé
    } else {
        *r = 255; *g = 255; *b = 255;
    }
}

// ---------------- INSERTION SORT ----------------
void insertion_init(int* size) {
    i_insertion = 1;
    inserting = 0;
}

void insertion_step(int* arr, int* done, int* size) {

    if (i_insertion >= *size) {
        *done = 1;
        return;
    }

    if (!inserting) {
        key = arr[i_insertion];
        j_insertion = i_insertion - 1;
        inserting = 1;
    }

    if (j_insertion >= 0 && arr[j_insertion] > key) {
        arr[j_insertion + 1] = arr[j_insertion];
        j_insertion--;
    } else {
        arr[j_insertion + 1] = key;
        i_insertion++;
        inserting = 0;
    }
}

void insertion_color(int i, int size, Uint8* r, Uint8* g, Uint8* b) {

    if (i < i_insertion) {
        *r = 0; *g = 150; *b = 255; // trié
    }
    else if (i == i_insertion) {
        *r = 255; *g = 0; *b = 0; // élément courant
    }
    else {
        *r = 255; *g = 255; *b = 255;
    }
}

// ---------------- COCKTAIL SORT ----------------
void cocktail_init(int* size) {
    start_cocktail = 0;
    end_cocktail = *size - 1;
    i_cocktail = 0;
    direction = 1;
}

void cocktail_step(int* arr, int* done, int* size) {

    if (start_cocktail >= end_cocktail) {
        *done = 1;
        return;
    }

    if (direction == 1) {
        if (i_cocktail < end_cocktail) {
            if (arr[i_cocktail] > arr[i_cocktail + 1]) {
                int tmp = arr[i_cocktail];
                arr[i_cocktail] = arr[i_cocktail + 1];
                arr[i_cocktail + 1] = tmp;
            }
            i_cocktail++;
        } else {
            end_cocktail--;
            direction = -1;
        }
    } else {
        if (i_cocktail > start_cocktail) {
            if (arr[i_cocktail] < arr[i_cocktail - 1]) {
                int tmp = arr[i_cocktail];
                arr[i_cocktail] = arr[i_cocktail - 1];
                arr[i_cocktail - 1] = tmp;
            }
            i_cocktail--;
        } else {
            start_cocktail++;
            direction = 1;
        }
    }
}

void cocktail_color(int i, int size, Uint8* r, Uint8* g, Uint8* b) {

    if (i < start_cocktail || i > end_cocktail)
        *r = 0, *g = 150, *b = 255; // trié

    else if (i == i_cocktail)
        *r = 255, *g = 0, *b = 0;

    else
        *r = 255, *g = 255, *b = 255;
}

// ---------------- GNOME SORT ----------------
void gnome_init(int* size) {
    i_gnome = 1;
}
void gnome_step(int* arr, int* done, int* size) {

    if (i_gnome >= *size) {
        *done = 1;
        return;
    }

    if (i_gnome == 0 || arr[i_gnome] >= arr[i_gnome - 1]) {
        i_gnome++;
    } else {
        int tmp = arr[i_gnome];
        arr[i_gnome] = arr[i_gnome - 1];
        arr[i_gnome - 1] = tmp;
        i_gnome--;
    }
}
void gnome_color(int i, int size, Uint8* r, Uint8* g, Uint8* b) {

    if (i < i_gnome)
        *r = 0, *g = 150, *b = 255;

    else if (i == i_gnome)
        *r = 255, *g = 0, *b = 0;

    else
        *r = 255, *g = 255, *b = 255;
}

// ---------------- GAMBLE SORT ----------------
void gamble_init(int* size) {}

void gamble_step(int* arr, int* done, int* size) {

    if (is_sorted(*size)) {
        *done = 1;
        return;
    }

    int a = rand() % (*size);
    int b = rand() % (*size);

    if (a == b) return;

    // swap si améliore
    if ((a < b && arr[a] > arr[b]) || (a > b && arr[a] < arr[b])) {
        int tmp = arr[a];
        arr[a] = arr[b];
        arr[b] = tmp;
    }
}
void gamble_color(int i, int size, Uint8* r, Uint8* g, Uint8* b) {

    // léger effet aléatoire visuel
    if (rand() % 20 == 0)
        *r = 255, *g = 0, *b = 255; // flash

    else
        *r = 255, *g = 255, *b = 255;
}


// ---------------- TEXT ----------------
void draw_text(SDL_Renderer* renderer, TTF_Font* font, const char* text, int x, int y) {
    SDL_Color white = {255,255,255};
    SDL_Surface* surf = TTF_RenderText_Solid(font, text, white);
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);

    SDL_Rect rect = {x, y, surf->w, surf->h};
    SDL_RenderCopy(renderer, tex, NULL, &rect);

    SDL_FreeSurface(surf);
    SDL_DestroyTexture(tex);
}

// ---------------- DRAW ----------------
void draw(SDL_Renderer* renderer, TTF_Font* font, int done, Algo* algo, int size){
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // TOP BAR
    SDL_Rect top = {0, 0, WIDTH, TOP_BAR};
    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
    SDL_RenderFillRect(renderer, &top);

    draw_text(renderer, font, algo->name, 10, 10);

    // BARS
    int barWidth = WIDTH / size;

    for (int i = 0; i < size; i++) {

        SDL_Rect bar = {
            i * barWidth,
            (HEIGHT - BUTTON_HEIGHT) - arr[i],
            barWidth - 2,
            arr[i]
        };

        Uint8 r, g, b;

        // -------- ANIMATION FIN --------
        if (finish_anim) {

            if (i < finish_index) {
                // jaune
                r = 255; g = 255; b = 0;
            } else {
                // bleu
                r = 0; g = 150; b = 255;
            }

        } else {
            // couleur normale de l'algo
            algo->get_color(i, size, &r, &g, &b);
        }

        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
        SDL_RenderFillRect(renderer, &bar);
    }

    // BUTTON COLOR
    if (done)
        SDL_SetRenderDrawColor(renderer, 0, 180, 0, 255); // vert
    else
        SDL_SetRenderDrawColor(renderer, 200, 100, 0, 255); // orange

    SDL_RenderFillRect(renderer, &action_btn);

    // BUTTON TEXT
    if (done) {

        char text[64];

        if (finish_time != 0) {
            Uint32 remaining = auto_restart_delay - (SDL_GetTicks() - finish_time);
            int seconds = remaining / 1000 + 1;

            sprintf(text, "RESTART (%d)", seconds);
        } else {
            sprintf(text, "RESTART");
        }

        draw_text(renderer, font, text, WIDTH/2 - 100, HEIGHT - BUTTON_HEIGHT + 25);
    }
    else
        draw_text(renderer, font, "SKIP >", WIDTH/2 - 50, HEIGHT - BUTTON_HEIGHT + 25);

    // BUTTON VEILLE

    SDL_SetRenderDrawColor(renderer, 80, 80, 200, 255);
    SDL_RenderFillRect(renderer, &sleep_btn);

    draw_text(renderer, font, "SLEEP", WIDTH - 150, 10);

    SDL_RenderPresent(renderer);
}

// ---------------- VEILLE ----------------
void sleep_screen(SDL_Renderer* renderer) {
    // écran noir une fois
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    SDL_Event e;
    while (sleep_mode) {
        SDL_WaitEvent(&e); // CPU ~0%

        if (e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_KEYDOWN) {
            system("vcgencmd display_power 1");
            sleep_mode = 0;
        }

        if (e.type == SDL_QUIT) {
            exit(0);
        }
    }
}

// ---------------- NEW ALGO ----------------
void start_new_algo(int* current_algo, Algo* algos, int algo_count, int* size, int* done) {
    int algo;
    finish_anim = 0;
    finish_index = 0;
    do {
        algo = rand() % algo_count;
    } while (algo == *current_algo);

    *current_algo = algo;
    *size = algos[*current_algo].size;

    for (int i = 0; i < *size; i++)
        arr[i] = rand() % (HEIGHT - BUTTON_HEIGHT - TOP_BAR);

    algos[*current_algo].init(size);
    *done = 0;
}

// ---------------- MAIN ----------------
int main() {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    SDL_Window* window = SDL_CreateWindow(
        "Sort Visual",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        800, 480,
        SDL_WINDOW_FULLSCREEN_DESKTOP
    );

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

    TTF_Font* font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 24);

    srand(time(NULL) ^ SDL_GetTicks());

    Algo algos[] = {
        {"Bogo Sort", 6, 60, bogo_init, bogo_step, default_color},
        {"Bubble Sort", 50, 60, bubble_init, bubble_step, bubble_color},
        {"Selection Sort", 50, 60, selection_init, selection_step, selection_color},
        {"Staline Sort", 100, 60, stalin_init, stalin_step, stalin_color},
        {"Thanos Sort", 100, 15, thanos_init, thanos_step, thanos_color},
        {"Insertion Sort", 50, 60, insertion_init, insertion_step, insertion_color},
        {"Cocktail Sort", 50, 60, cocktail_init, cocktail_step, cocktail_color},
        {"Gnome Sort", 50, 60, gnome_init, gnome_step, gnome_color},
        {"Gamble Sort", 20, 60, gamble_init, gamble_step, gamble_color}
    };

    int algo_count = sizeof(algos)/sizeof(Algo);

    int current_algo = rand() % algo_count;
    int done = 0;
    int running = 1;
    int size = algos[current_algo].size;

    for (int i = 0; i < size; i++)
        arr[i] = rand() % (HEIGHT - BUTTON_HEIGHT - TOP_BAR);

    algos[current_algo].init(&size);

    while (running) {

        SDL_Event e;
        while (SDL_PollEvent(&e)) {

            if (e.type == SDL_QUIT)
                running = 0;

            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)
                running = 0;

            if (e.type == SDL_MOUSEBUTTONDOWN) {
                int x = e.button.x;
                int y = e.button.y;

                if (in_rect(action_btn, x, y)) {
                    start_new_algo(&current_algo, algos, algo_count, &size, &done);
                    finish_time = 0;
                }

                if (in_rect(sleep_btn, x, y)) {
                    system("vcgencmd display_power 0");
                    sleep_mode = 1;
                }
            }
        }

        if (sleep_mode) {
            sleep_screen(renderer);
        }

        if (!done)
            algos[current_algo].step(arr, &done, &size);

        if (done && finish_time == 0) {
            finish_time = SDL_GetTicks();
        }

        if (done && finish_time != 0) {

            Uint32 elapsed = SDL_GetTicks() - finish_time;

            if (elapsed >= auto_restart_delay) {
                start_new_algo(&current_algo, algos, algo_count, &size, &done);
                finish_time = 0;
            }
        }
        
        if (done && !finish_anim) {
            finish_anim = 1;
            finish_index = 0;
        }

        if (finish_anim) {
            finish_index++;
            if (finish_index > size)
                finish_index = size;
        }

        draw(renderer, font, done, &algos[current_algo], size);

        SDL_Delay(1000 / algos[current_algo].fps);
    }

    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    TTF_Quit();
    SDL_Quit();

    return 0;
}