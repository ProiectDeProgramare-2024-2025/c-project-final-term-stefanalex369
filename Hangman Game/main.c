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
#define BLUE "\x1b[34m"
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
    printf("\nPress Enter to return to menu...");
    getchar();
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
        if (strlen(words[count]) >= 3) {
            count++;
        }
    }
    fclose(file);
    return count;
}

void addWordToList() {
    char newWord[MAX_WORD_LEN];
    printf("Enter a new word to add (letters only, min 3, max %d): ", MAX_WORD_LEN - 1);
    fgets(newWord, sizeof(newWord), stdin);
    newWord[strcspn(newWord, "\n")] = 0;

    if (strlen(newWord) < 3) {
        printf(RED "Word too short! Must be at least 3 letters.\n" RESET);
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
}

void saveGameHistory(const char *username, int score) {
    FILE *file = fopen("history.txt", "a");
    if (file != NULL) {
        fprintf(file, "%s %d\n", username, score);
        fclose(file);
    }
}

void sortPlayers(Player players[], int count) {
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (players[j].score > players[i].score) {
                Player temp = players[i];
                players[i] = players[j];
                players[j] = temp;
            }
        }
    }
}

void viewLeaderboard() {
    Player players[MAX_PLAYERS];
    int count;
    readLeaderboard(players, &count);

    if (count == 0) {
        printf(RED "The leaderboard is empty.\n" RESET);
    } else {
        sortPlayers(players, count);  // sortare descrescătoare
        printf("----Leaderboard----\n");
        for (int i = 0; i < count; i++) {
            printf("%d. %s%s%s - %d points\n", i + 1, YELLOW, players[i].name, RESET, players[i].score);
        }
    }
    printf("-------------------\n");
    wait_for_enter();
}

void viewPlayerHistory() {
    char username[50];
    printf("Enter username to view history: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = 0;

    FILE *file = fopen("history.txt", "r");
    if (file == NULL) {
        printf(RED "No history available.\n" RESET);
        wait_for_enter();
        return;
    }

    int found = 0, gameNumber = 1;
    char name[50];
    int score;
    printf("----Game History for %s----\n", username);
    while (fscanf(file, "%s %d", name, &score) == 2) {
        if (strcmp(name, username) == 0) {
            printf("Game %d: %d points\n", gameNumber, score);
            gameNumber++;
            found = 1;
        }
    }
    fclose(file);
    if (!found) {
        printf("No games found for this player.\n");
    }
    printf("-----------------------------\n");
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
void playGame(Player players[], int *playerCount) {
    char username[50];
    printf("Please enter your username (letters only, max 20): ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = 0;

    if (*playerCount >= MAX_PLAYERS) {
        printf(RED "Maximum number of players reached!\n" RESET);
        wait_for_enter();
        return;
    }

    int idx = -1;
    for (int i = 0; i < *playerCount; i++) {
        if (strcmp(players[i].name, username) == 0) {
            idx = i;
            break;
        }
    }
    if (idx == -1) {
        strcpy(players[*playerCount].name, username);
        players[*playerCount].score = 0;
        idx = (*playerCount)++;
    }

    char words[MAX_WORDS][MAX_WORD_LEN];
    int numWords = loadWords(words);
    if (numWords == 0) {
        wait_for_enter();
        return;
    }

    int totalScore = 0;
    int userQuit = 0;

    while (1) {
        const char *word = words[rand() % numWords];
        int wordLen = strlen(word);
        char display[wordLen + 1];
        int wrongGuesses = 0;

        for (int i = 0; i < wordLen; i++) {
            display[i] = '_';
        }
        display[wordLen] = '\0';

        while (wrongGuesses < 5 && strcmp(display, word) != 0) {
            printf("\nWord: ");
            for (int i = 0; i < wordLen; i++) printf("%c ", display[i]);
            printf("\nWrong guesses: %d/5\n", wrongGuesses);

            char input[100];
            printf("Enter one letter or the full word: ");
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
                    if (word[i] == letter && display[i] == '_') {
                        display[i] = letter;
                        found = 1;
                    }
                }
                if (!found) {
                    printf(RED "Wrong guess!\n" RESET);
                    wrongGuesses++;
                }
            } else {
                if (strcmp(input, word) == 0) {
                    strcpy(display, word);
                    break;
                } else {
                    printf(RED "Wrong word!\n" RESET);
                    wrongGuesses++;
                }
            }
        }

        if (strcmp(display, word) == 0) {
            printf(GREEN "\nCongratulations! You completed the word: %s\n" RESET, word);
            totalScore += wordLen;

            char choice[10];
            printf("Do you want to continue? (y/n): ");
            fgets(choice, sizeof(choice), stdin);
            if (tolower(choice[0]) != 'y') {
                userQuit = 1;
                break;
            }
        } else {
            printf(RED "\nGame over! The word was: %s\n" RESET, word);
            break;
        }
    }

    // update only if the new score is better than the old best
    if (totalScore > players[idx].score) {
        players[idx].score = totalScore;
    }

    saveGameHistory(username, totalScore);
    writeLeaderboard(players, *playerCount);

    if (!userQuit) {
        wait_for_enter();
    }
}

void header() {
    printf("=====================================\n");
    printf("|         %sHANGMAN GAME%s             |\n", YELLOW, RESET);
    printf("=====================================\n");
    printf("| %s1%s - Play Game                    |\n", BLUE, RESET);
    printf("| %s2%s - View Leaderboard             |\n", BLUE, RESET);
    printf("| %s3%s - View Player History          |\n", BLUE, RESET);
    printf("| %s4%s - View Game Tips               |\n", BLUE, RESET);
    printf("| %s5%s - Add Game Tip                 |\n", BLUE, RESET);
    printf("| %s6%s - Add Word to List             |\n", BLUE, RESET);
    printf("| %s0%s - %sExit%s                         |\n", BLUE, RESET, RED, RESET);
    printf("=====================================\n");
}

int main() {
    srand(time(NULL));
    Player players[MAX_PLAYERS];
    int playerCount;
    readLeaderboard(players, &playerCount);

    int option;
    while (1) {
        clear_screen();
        header();
        printf("Enter option (0-6): ");
        if (scanf("%d", &option) != 1) {
            while (getchar() != '\n');
            printf(RED "Invalid input! Enter a number.\n" RESET);
            wait_for_enter();
            continue;
        }
        while (getchar() != '\n');

        if (option == 0) {
            printf(GREEN "Thank you for playing! Goodbye!\n" RESET);
            break;
        }

        clear_screen();  // clear only once here, before showing each section

        switch (option) {
            case 1:
                playGame(players, &playerCount);
                break;
            case 2:
                viewLeaderboard();
                break;
            case 3:
                viewPlayerHistory();
                break;
            case 4:
                viewGameTips();
                break;
            case 5:
                addGameTip();
                break;
            case 6:
                addWordToList();
                break;
            default:
                printf(RED "Invalid option! Choose between 0 and 6.\n" RESET);
                wait_for_enter();
        }
    }

    return 0;
}
