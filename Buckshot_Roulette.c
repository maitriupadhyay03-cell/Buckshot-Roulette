#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <windows.h> 

// Global game state variables
int bullets;
int blanks;
bool playerturn = true;
int player_life;
int comlife;
int total;

// Variables for Graphics/Animation (Declared as const char* to prevent warnings)
const char *gun_direction_icon = "---"; 
const char *shot_feedback_line = "READY..."; 

// --- Function Prototypes ---
void loadgame(int level); 
void Shoot(bool target_is_self, bool shooter_is_player);
void player_turn();
void comturn();
void DrawGameScene();
void delay(int milliseconds);

// --- ANIMATION/DELAY FUNCTIONS ---

void delay(int milliseconds) 
{
    Sleep(milliseconds);
}

void animate_shot(const char *target_name, const char *blast_direction) 
{
    gun_direction_icon = blast_direction;
    shot_feedback_line = "AIMING...";
    DrawGameScene();
    delay(400); 

    shot_feedback_line = "PULLING...";
    DrawGameScene();
    delay(400);

    shot_feedback_line = "BANG!";
    DrawGameScene();
    delay(300);
}

void clear_screen() 
{
    system("cls"); 
}

// --- GRAPHICS DRAWING FUNCTION ---

void DrawGameScene() 
{
    clear_screen();
    
    // Icons are 5 characters wide, surrounded by 1 space on the player side: ' /|\ ' (7 total width)
    const char *P_Icon[] = { "  o  ", " /|\\ ", " / \\ " };
    const char *D_Icon[] = { "          O  ", "  /|\\ ", " / \\ " };
    
    // Gun Animation Based on Shot Direction
    const char *gun_format;
    if (strcmp(gun_direction_icon, "LEFT") == 0) 
    {
        gun_format = "<= SHOT!"; 
    } else if (strcmp(gun_direction_icon, "RIGHT") == 0) 
    {
        gun_format = "SHOT! =>"; 
    } else 
    {
        gun_format = "---------"; 
    }
    
    // Helper strings for formatting the center content
    char center_gun_line[50];
    char center_feedback_line[50];

    // Total center width required is 37 characters (to place the 5-char Dealer icon correctly)
    // Gun line: Gun is 9 chars. We need 37 total. 37 - 9 = 28 spaces. 
    // Feedback line: Feedback is 20 chars. We need 37 total. 37 - 20 = 17 spaces.
    
    // Center Gun Line (9 chars)
    sprintf(center_gun_line, "              %-9s              ", gun_format); // 14 spaces + 9 chars + 14 spaces = 37 chars

    // Center Feedback Line (20 chars)
    sprintf(center_feedback_line, "         %-20s         ", shot_feedback_line); // 9 spaces + 20 chars + 8 spaces = 37 chars (using 8 spaces on the right to accommodate the 50 char width better)
    
    
    printf("====================================================\n");
    printf("|          C-ROULETTE: THE TERMINAL GAME           |\n");
    printf("====================================================\n");

    // --- LIFE & STATUS LINE ---
    printf("| LIFE: %d      ", player_life);
    if (playerturn) 
    {
        printf("<<< YOUR TURN >>>");
    } else 
    {
        printf("<<< DEALER TURN >>>");
    }
    printf("      LIFE: %d |\n", comlife);
    printf("====================================================\n");
    
    // --- VISUAL SCENE ---
    printf("  PLAYER           CHAMBER STATUS           DEALER  \n");
    
    // Line 1 (Heads / Chamber Status) - Alignment looks stable here.
    printf(" %s         [ B:%d | K:%d | T:%d ]  %s \n", 
           P_Icon[0], bullets, blanks, total, D_Icon[0]);

    // Line 2 (Torsos / Gun) - Using the pre-calculated center string
    printf(" %s%s%s \n", P_Icon[1], center_gun_line, D_Icon[1]); 

    // Line 3 (Legs / Feedback) - Using the pre-calculated center string
    printf(" %s%s%s \n", P_Icon[2], center_feedback_line, D_Icon[2]); 
    
    printf("----------------------------------------------------\n");
}


// --- GAME LOGIC FUNCTIONS (Unchanged) ---

void loadgame(int level)
{
    if (player_life == 0 || comlife == 0) 
    {
        player_life = 3;
        comlife = 3;
        playerturn = true; 
    }

    switch(level) 
    {
        case 1: bullets = 2; blanks = 2; break;
        case 2: bullets = (rand() % 3) + 1; blanks = 5 - bullets; break;
        case 3: bullets = (rand() % 5) + 1; blanks = 8 - bullets; break;
        default: bullets = 2; blanks = 2; break;
    }
    
    total = blanks + bullets;
    
    gun_direction_icon = "---";
    shot_feedback_line = "CHAMBER RELOADED";
    DrawGameScene();
    delay(1000);
}

void Shoot(bool target_is_self, bool shooter_is_player)
{
    if (total <= 0) return;

    const char *target_name = target_is_self ? (shooter_is_player ? "Yourself" : "Dealer") : 
                                               (shooter_is_player ? "Dealer" : "You");
    const char *blast_direction = (shooter_is_player && target_is_self) || (!shooter_is_player && !target_is_self) ? "LEFT" : "RIGHT";

    animate_shot(target_name, blast_direction); 

    int shot = rand() % total;
    bool is_bullet = (shot < bullets); 

    if (is_bullet)
    {
        bullets--;
        if (target_is_self)
        {
            if (shooter_is_player) 
            {
                shot_feedback_line = "BULLET! You were HIT!";
                player_life--;
                playerturn = false;
            } else 
            {
                shot_feedback_line = "BULLET! Dealer HIT!";
                comlife--;
                playerturn = true;
            }
        }
        else
        {
            if (shooter_is_player) 
            {
                shot_feedback_line = "BULLET! Dealer HIT!";
                comlife--;
                playerturn = true;
            } else 
            {
                shot_feedback_line = "BULLET! You were HIT!";
                player_life--;
                playerturn = false;
            }
        }
    }
    else
    {
        blanks--;
        if (target_is_self)
        {
            shot_feedback_line = "BLANK! Gun STAYS.";
        }
        else
        {
            shot_feedback_line = "BLANK! Gun PASSES.";
            playerturn = !shooter_is_player;
        }
    }
    
    total--; 
    
    gun_direction_icon = "---";
    DrawGameScene();
    delay(1500); 
}


void player_turn()
{
    int choice;
    
    while (true) 
    {
        DrawGameScene();
        printf("\n*** YOUR ACTION ***\n 1 : SHOOT YOURSELF | 2 : SHOOT DEALER \n: ");

        if (scanf("%d",&choice) == 1) 
        {
            while (getchar() != '\n'); 
            
            switch(choice)
            {
                case 1:
                    Shoot(true, true);
                    return; 
                case 2:
                    Shoot(false, true);
                    return; 
                default:
                    shot_feedback_line = "INVALID CHOICE. Try 1 or 2.";
            }
        } 
        else 
        {
            while (getchar() != '\n'); 
            shot_feedback_line = "INVALID INPUT. Enter a number.";
        }
    }
}

void comturn()
{
    float blank_prob = (total > 0) ? (float)blanks / total : 0.0;
    
    bool com_shoots_self = false;
    
    if (blank_prob > 0.5) 
    { 
        com_shoots_self = true;
        shot_feedback_line = "Dealer shoots SELF...";
    } 
    else 
    {
        shot_feedback_line = "Dealer shoots YOU...";
    }
    
    DrawGameScene();
    delay(1000);
    
    Shoot(com_shoots_self, false);
}


int main()
{
    srand(time(0)); 
    int level;
    
    clear_screen();
    printf("Welcome to C-Roulette!\n");

    while (true) 
    {
        printf("Enter level (1, 2, or 3): ");
        if (scanf("%d",&level) == 1) 
        {
            while (getchar() != '\n'); 
            if (level >= 1 && level <= 3) 
            {
                break; 
            }
        } 
        else 
        {
            while (getchar() != '\n'); 
        }
        printf("Invalid input. Please enter 1, 2, or 3.\n");
    }
    
    player_life = 3; 
    comlife = 3;
    playerturn = true; 

    loadgame(level);

    while(player_life > 0 && comlife > 0)
    {
        if(total == 0)
        {
            shot_feedback_line = "CHAMBER EMPTY... RELOADING";
            DrawGameScene();
            delay(1000);
            loadgame(level);
        }

        if(playerturn)
        {
            player_turn();
        }
        else
        {
            comturn();
        }
    }

    clear_screen();
    printf("\n===================================\n");
    if (player_life <= 0)
    {
        printf("\t****** YOU LOST ******\n");
    }
    else
    {
        printf("\t****** YOU WIN ******\n");
    }
    printf("===================================\n");
    delay(3000);

    return 0;
}