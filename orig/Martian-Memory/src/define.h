SDL_Surface *menu_font_a, *menu_font_i, *menuback;
SDL_Surface *scorefont, *scorefont2, *bonusfont;
SDL_Surface *Schips;
SDL_Surface *screenbak;

Mix_Music *music;
Mix_Chunk *option_sound;
Mix_Chunk *touch, *match, *nomatch, *combo_sound;


#define chips_x 8
#define chips_y 4
#define chips chips_x*chips_y
#define chips_start_x 36
#define chips_start_y 8
#define chips_spacing_x 8
#define chips_spacing_y 8
#define chip_w 64
#define chip_h 96

// --- definimos la matriz que contiene las chips
struct Cchip
{
    int type; // numero de chip que es
    int side; // que lado de la chip se muestra (0= dada vuelta, 1= se ve el grafico)
    int state; // estado en el que esta la chip (0= sin descubrir, 1=descubierta la pareja)
};
Cchip mchip[chips_x][chips_y];

int turn; // define que turno estamos (turno=0 no se ha levantado niguna chip, turno=1 hay una chip leventada y hay que levantar otra)
int match_x, match_y; // indican cual es la posicion de la chip recogida en el primer turno

double gametime;
double showtime; // tiempo que se espera para mostrar las chips levantadas
int score;
int fullscreen=0;
int combo;
int hiscores;


// --- Scores related
int namechar;
//int gameover=0;

char playername[]="          ";
char scorename[10][10];
int scoretime[10];
int scorescore[10];
int scorepos;

int codechar;
char code[]="....";
int inputloop;

