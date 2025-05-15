/**
 * @file maze.c
 * @author SunFanding
 * @brief Code for the maze game for COMP1921 Assignment 2
 * NOTE - You can remove or edit this file however you like - this is just a provided skeleton code
 * which may be useful to anyone who did not complete assignment 1.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Defines for max and min permitted dimensions
#define MAX_DIM 100
#define MIN_DIM 5

// Defines for the required autograder exit codes
#define EXIT_SUCCESS 0
#define EXIT_ARG_ERROR 1
#define EXIT_FILE_ERROR 2
#define EXIT_MAZE_ERROR 3

// Struct to represent coordinates
typedef struct {
    int x;
    int y;
} coord;

// Struct to represent the maze
typedef struct {
    char **map;    // 2D array to store the maze
    int height;    // Height of the maze
    int width;     // Width of the maze
    coord start;   // Starting position
    coord end;     // Ending position
} maze;

/**
 * @brief Initialise a maze object - allocate memory and set attributes
 *
 * @param this Pointer to the maze to be initialised
 * @param height Height to allocate
 * @param width Width to allocate
 * @return int 0 on success, 1 on failure
 */
int create_maze(maze *this, int height, int width) {
    if (height < MIN_DIM || height > MAX_DIM || width < MIN_DIM || width > MAX_DIM) {
        fprintf(stderr, "Error: Maze dimensions out of bounds (%d, %d).\n", height, width);
        return 1;
    }

    // Allocate memory for the map
    this->map = malloc(height * sizeof(char *));
    if (this->map == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for maze map.\n");
        return 1;
    }

    for (int i = 0; i < height; i++) {
        this->map[i] = malloc(width * sizeof(char));
        if (this->map[i] == NULL) {
            // Free previously allocated memory
            for (int j = 0; j < i; j++) {
                free(this->map[j]);
            }
            free(this->map);
            fprintf(stderr, "Error: Memory allocation failed for maze row.\n");
            return 1;
        }
    }

    this->height = height;
    this->width = width;
    return 0;
}

/**
 * @brief Free the memory allocated to the maze struct
 *
 * @param this The pointer to the struct to free
 */
void free_maze(maze *this) {
    for (int i = 0; i < this->height; i++) {
        free(this->map[i]);
    }
    free(this->map);
    free(this);
}

/**
 * @brief Validate and return the width of the mazefile
 *
 * @param file The file pointer to check
 * @return int 0 for error, or a valid width (5-100)
 */
int get_width(FILE *file) {
    int width = 0;
    char buffer[MAX_DIM + 1];
    while (fgets(buffer, sizeof(buffer), file)) {
        int len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            len--;
        }
        if (width == 0) {
            width = len;
        } else if (len != width) {
            fprintf(stderr, "Error: Inconsistent row length.\n");
            return 0;
        }
        if (width < MIN_DIM || width > MAX_DIM) {
            fprintf(stderr, "Error: Row length out of bounds (%d).\n", width);
            return 0;
        }
    }
    return width;
}

/**
 * @brief Validate and return the height of the mazefile
 *
 * @param file The file pointer to check
 * @return int 0 for error, or a valid height (5-100)
 */
int get_height(FILE *file) {
    int height = 0;
    char buffer[MAX_DIM + 1];
    while (fgets(buffer, sizeof(buffer), file)) {
        height++;
        if (height > MAX_DIM) {
            fprintf(stderr, "Error: Maze height exceeds maximum (%d).\n", MAX_DIM);
            return 0;
        }
    }
    if (height < MIN_DIM) {
        fprintf(stderr, "Error: Maze height below minimum (%d).\n", MIN_DIM);
        return 0;
    }
    return height;
}

/**
 * @brief Read in a maze file into a struct
 *
 * @param this Maze struct to be used
 * @param file Maze file pointer
 * @return int 0 on success, 1 on failure
 */
int read_maze(maze *this, FILE *file) {
    // Get width and height
    rewind(file);
    this->width = get_width(file);
    if (this->width == 0) {
        return 1;
    }

    this->height = get_height(file);
    if (this->height == 0) {
        return 1;
    }

    // Allocate memory for the map
    if (create_maze(this, this->height, this->width) != 0) {
        return 1;
    }

    // Read the maze into the map
    rewind(file);
    char buffer[MAX_DIM + 1];
    int i = 0;
    while (fgets(buffer, sizeof(buffer), file)) {
        int len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }
        if (strlen(buffer) != (size_t)this->width) {  // 修改这里
            fprintf(stderr, "Error: Inconsistent row length in maze file.\n");
            return 1;
        }
        strcpy(this->map[i], buffer);
        i++;
    }

    // Find start and end points
    int start_count = 0, end_count = 0;
    for (int i = 0; i < this->height; i++) {
        for (int j = 0; j < this->width; j++) {
            if (this->map[i][j] == 'S') {
                this->start.x = j;
                this->start.y = i;
                start_count++;
            } else if (this->map[i][j] == 'E') {
                this->end.x = j;
                this->end.y = i;
                end_count++;
            }
        }
    }

    if (start_count != 1 || end_count != 1) {
        fprintf(stderr, "Error: Maze must have exactly one start and one end point.\n");
        return 1;
    }

    return 0;
}

/**
 * @brief Prints the maze out - code provided to ensure correct formatting
 *
 * @param this Pointer to maze to print
 * @param player The current player location
 */
void print_maze(maze *this, coord *player) {
    // Make sure we have a leading newline
    printf("\n");
    for (int i = 0; i < this->height; i++) {
        for (int j = 0; j < this->width; j++) {
            // Decide whether player is on this spot or not
            if (player->x == j && player->y == i) {
                printf("X");
            } else {
                printf("%c", this->map[i][j]);
            }
        }
        // End each row with a newline
        printf("\n");
    }
}

/**
 * @brief Validates and performs a movement in a given direction
 *
 * @param this Maze struct
 * @param player The player's current position
 * @param direction The desired direction to move in
 */
void move(maze *this, coord *player, char direction) {
    int new_x = player->x;
    int new_y = player->y;

    switch (direction) {
        case 'W':
        case 'w':
            new_y -= 1;
            break;
        case 'A':
        case 'a':
            new_x -= 1;
            break;
        case 'S':
        case 's':
            new_y += 1;
            break;
        case 'D':
        case 'd':
            new_x += 1;
            break;
        case 'Q':
        case 'q':
            printf("Game quit.\n");
            exit(EXIT_SUCCESS);
        default:
            printf("Invalid direction. Please use W/A/S/D or Q to quit.\n");
            return;
    }

    // Check boundaries
    if (new_x < 0 || new_x >= this->width || new_y < 0 || new_y >= this->height) {
        printf("Cannot move outside the maze.\n");
        return;
    }

    // Check for walls
    if (this->map[new_y][new_x] == '#') {
        printf("Cannot move through walls.\n");
        return;
    }

    // Move the player
    player->x = new_x;
    player->y = new_y;
}

/**
 * @brief Check whether the player has won and return a pseudo-boolean
 *
 * @param this Current maze
 * @param player Player position
 * @return int 0 for false, 1 for true
 */
int has_won(const maze *this, const coord *player) {
    return (player->x == this->end.x && player->y == this->end.y);
}

int main(int argc, char *argv[]) {  // 修改这里
    // Check args
    if (argc != 2) {
        fprintf(stderr, "Usage: ./maze <mazefile>\n");
        return EXIT_ARG_ERROR;
    }

    // Open the maze file
    FILE *f = fopen(argv[1], "r");
    if (f == NULL) {
        fprintf(stderr, "Error: Cannot open file %s.\n", argv[1]);
        return EXIT_FILE_ERROR;
    }

    // Read in mazefile to struct
    maze *this_maze = malloc(sizeof(maze));
    if (this_maze == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for maze struct.\n");
        fclose(f);
        return EXIT_FILE_ERROR;
    }

    if (read_maze(this_maze, f) != 0) {
        free_maze(this_maze);
        fclose(f);
        return EXIT_MAZE_ERROR;
    }

    fclose(f);

    // Initialize player position
    coord *player = malloc(sizeof(coord));
    if (player == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for player.\n");
        free_maze(this_maze);
        return EXIT_FILE_ERROR;
    }
    player->x = this_maze->start.x;
    player->y = this_maze->start.y;

    // Maze game loop
    char move;
    while (1) {
        print_maze(this_maze, player);
        printf("Enter move (W/A/S/D/M/Q): ");
        scanf(" %c", &move);

        if (move == 'M' || move == 'm') {
            print_maze(this_maze, player);
            continue;
        }

        if (move == 'Q' || move == 'q') {
            printf("Game quit.\n");
            break;
        }

        // Perform move
        move(this_maze, player, move);
        if (has_won(this_maze, player)) {
            print_maze(this_maze, player);
            printf("Congratulations! You have won the game.\n");
            break;
        }
    }

    // Clean up
    free(player);
    free_maze(this_maze);
    return EXIT_SUCCESS;
}