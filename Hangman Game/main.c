#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define MAX_WORDS 100
#define MAX_WORD_LEN 50
#define MAX_PLAYERS 100
#define GREEN "\x1b[32m"
#define YELLOW "\x1b[38;2;255;165;0m"
#define RED "\x1b[31m"
#define BLUE    "\x1b[34m"
#define RESET "\x1b[0m"

typedef struct {
    char name[50];
    int score;
} Player;

void clear_screen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void wait_for_enter() {
    printf("\nPress Enter to continue...");
    getchar();
    clear_screen();
}

int loadWords(char words[MAX_WORDS][MAX_WORD_LEN]) {
    FILE *file = fopen("words.txt", "r");
    if (file == NULL) {
        printf(RED "Error: could not open words.txt\n" RESET);
        return 0;
    }
    int count = 0;
    while (fgets(words[count], MAX_WORD_LEN, file) != NULL && count < MAX_WORDS) {
        words[count][strcspn(words[count], "\n")] = 0;
        count++;
    }
    fclose(file);
    return count;
}

void addWordToList() {
    char newWord[MAX_WORD_LEN];
    printf("Enter a new word to add (letters only, max %d): ", MAX_WORD_LEN - 1);
    fgets(newWord, sizeof(newWord), stdin);
    newWord[strcspn(newWord, "\n")] = 0;

    if (strlen(newWord) == 0) {
        printf(RED "No word entered. Returning to menu.\n" RESET);
        wait_for_enter();
        return;
    }

    for (int i = 0; i < strlen(newWord); i++) {
        if (!isalpha(newWord[i])) {
            printf(RED "Invalid word! Only letters allowed.\n" RESET);
            wait_for_enter();
            return;
        }
    }

    FILE *file = fopen("words.txt", "a");
    if (file == NULL) {
        printf(RED "Error opening words file.\n" RESET);
    } else {
        fseek(file, 0, SEEK_END);
        long size = ftell(file);
        if (size > 0) {
            fseek(file, -1, SEEK_END);
            int lastChar = fgetc(file);
            if (lastChar != '\n') {
                fputc('\n', file);
            }
        }
        fprintf(file, "%s\n", newWord);
        fclose(file);
        printf(GREEN "New word added successfully!\n" RESET);
    }
    wait_for_enter();
}

int findPlayerIndex(Player players[], int count, const char *name) {
    for (int i = 0; i < count; i++) {
        if (strcmp(players[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

void readLeaderboard(Player players[], int *count) {
    FILE *file = fopen("leaderboard.txt", "r");
    *count = 0;
    if (file != NULL) {
        while (fscanf(file, "%s %d", players[*count].name, &players[*count].score) == 2) {
            (*count)++;
        }
        fclose(file);
    }
}

void writeLeaderboard(Player players[], int count) {
    FILE *file = fopen("leaderboard.txt", "w");
    if (file == NULL) {
        printf(RED "Error opening leaderboard file.\n" RESET);
        return;
    }
    for (int i = 0; i < count; i++) {
        fprintf(file, "%s %d\n", players[i].name, players[i].score);
    }
    fclose(file);
    printf(GREEN "Leaderboard updated!\n" RESET);
}

void viewLeaderboard() {
    Player players[MAX_PLAYERS];
    int count;
    readLeaderboard(players, &count);
    if (count == 0) {
        printf(RED "The leaderboard is empty.\n" RESET);
    } else {
        printf("----Leaderboard----\n");
        for (int i = 0; i < count; i++) {
            printf("%s%s%s - %d wins\n", YELLOW, players[i].name, RESET, players[i].score);
        }
    }
    printf("-------------------\n");
    wait_for_enter();
}

void viewGameTips() {
    FILE *file = fopen("tips.txt", "r");
    if (file == NULL) {
        printf(RED "No game tips available.\n" RESET);
    } else {
        char line[200];
        printf("----Game Tips----\n");
        while (fgets(line, sizeof(line), file) != NULL) {
            printf("%s", line);
        }
        fclose(file);
    }
    printf("------------------\n");
    wait_for_enter();
}

void addGameTip() {
    char tip[200];
    printf("Enter a new game tip (max 200 chars): ");
    fgets(tip, sizeof(tip), stdin);
    tip[strcspn(tip, "\n")] = 0;

    if (strlen(tip) == 0) {
        printf(RED "No tip entered. Returning to menu.\n" RESET);
        wait_for_enter();
        return;
    }

    FILE *file = fopen("tips.txt", "a");
    if (file == NULL) {
        printf(RED "Error opening tips file.\n" RESET);
    } else {
        fseek(file, 0, SEEK_END);
        long size = ftell(file);
        if (size > 0) {
            fseek(file, -1, SEEK_END);
            int lastChar = fgetc(file);
            if (lastChar != '\n') {
                fputc('\n', file);
            }
        }
        fprintf(file, "-%s\n", tip);
        fclose(file);
        printf(GREEN "New tip added successfully!\n" RESET);
    }
    wait_for_enter();
}

void playGame() {
    char username[50];
    printf("Please enter your username (letters only, max 20): ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = 0;

    if (strlen(username) == 0 || strlen(username) > 20) {
        printf(RED "Invalid username! Must be 1–20 characters.\n" RESET);
        wait_for_enter();
        return;
    }

    char words[MAX_WORDS][MAX_WORD_LEN];
    int numWords = loadWords(words);
    if (numWords == 0) {
        wait_for_enter();
        return;
    }

    const char *word = words[rand() % numWords];
    int wordLen = strlen(word);
    char guessed[wordLen + 1];
    int tries = 6, won = 0;

    for (int i = 0; i < wordLen; i++) guessed[i] = '_';
    guessed[wordLen] = '\0';

    while (tries > 0) {
        printf("\nWord: ");
        for (int i = 0; i < wordLen; i++) printf("%c ", guessed[i]);
        printf("\nTries left: %d\n", tries);

        char input[100];
        printf("Enter one letter or the full word (letters only): ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = 0;

        int valid = 1;
        for (int i = 0; i < strlen(input); i++) {
            if (!isalpha(input[i])) {
                valid = 0;
                break;
            }
        }
        if (!valid) {
            printf(RED "Invalid input! Only letters allowed.\n" RESET);
            continue;
        }

        if (strlen(input) == 1) {
            char letter = input[0];
            int found = 0;
            for (int i = 0; i < wordLen; i++) {
                if (word[i] == letter && guessed[i] == '_') {
                    guessed[i] = letter;
                    found = 1;
                }
            }
            if (!found) {
                printf(RED "Wrong guess!\n" RESET);
                tries--;
            }
        } else {
            if (strcmp(input, word) == 0) {
                strcpy(guessed, word);
                won = 1;
                break;
            } else {
                printf(RED "Wrong word!\n" RESET);
                tries--;
            }
        }

        if (strcmp(guessed, word) == 0) {
            won = 1;
            break;
        }
    }

    Player players[MAX_PLAYERS];
    int playerCount;
    readLeaderboard(players, &playerCount);

    if (won) {
        printf(GREEN "\nCongratulations, %s! You guessed the word: %s\n" RESET, username, word);
        int idx = findPlayerIndex(players, playerCount, username);
        if (idx >= 0) {
            players[idx].score++;
        } else {
            strcpy(players[playerCount].name, username);
            players[playerCount].score = 1;
            playerCount++;
        }
        writeLeaderboard(players, playerCount);
    } else {
        printf(RED "\nSorry, %s! You lost. The word was: %s\n" RESET, username, word);
    }

    wait_for_enter();
}

void header() {
    clear_screen();
    printf("=====================================\n");
    printf("|         %sHANGMAN GAME%s             |\n", YELLOW, RESET);
    printf("=====================================\n");
    printf("| %s1%s - Play Game                    |\n", BLUE, RESET);
    printf("| %s2%s - View Leaderboard             |\n", BLUE, RESET);
    printf("| %s3%s - View Game Tips               |\n", BLUE, RESET);
    printf("| %s4%s - Add Game Tip                 |\n", BLUE, RESET);
    printf("| %s5%s - Add Word to List             |\n", BLUE, RESET);
    printf("| %s0%s - %sExit%s                         |\n", BLUE, RESET, RED, RESET);
    printf("=====================================\n");
}
void menu(int op) {
    switch(op) {
        case 1: playGame(); break;
        case 2: viewLeaderboard(); break;
        case 3: viewGameTips(); break;
        case 4: addGameTip(); break;
        case 5: addWordToList(); break;
        default: printf(GREEN "Thank you for playing! Goodbye!\n" RESET); break;
    }
    clear_screen();
}

int main() {
    srand(time(NULL));
    int option;

    do {
        header();
        printf("Enter option (0-5): ");
        if (scanf("%d", &option) != 1) {
            while(getchar() != '\n');  // clear invalid input
            printf(RED "Invalid input! Enter a number.\n" RESET);
            continue;
        }
        while(getchar() != '\n');  // clear buffer
        if (option >= 0 && option <= 5) {
            menu(option);
        } else {
            printf(RED "Invalid option! Choose between 0 and 5.\n" RESET);
        }
    } while(option != 0);

    return 0;
}
