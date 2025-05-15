/**
 * @file maze.c
 * @author Sun Fanding
 * @brief Code for the maze game for COMP1921 Assignment 2
 * NOTE - You can remove or edit this file however you like - this is just a provided skeleton code
 * which may be useful to anyone who did not complete assignment 1.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// defines for max and min permitted dimensions
#define MAX_DIM 100
#define MIN_DIM 5

// defines for the required autograder exit codes
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
        return 1;
    }

    this->map = malloc(height * sizeof(char *));
    if (this->map == NULL) {
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
    // Implementation goes here
    return 0;
}

/**
 * @brief Validate and return the height of the mazefile
 *
 * @param file The file pointer to check
 * @return int 0 for error, or a valid height (5-100)
 */
int get_height(FILE *file) {
    // Implementation goes here
    return 0;
}

/**
 * @brief Read in a maze file into a struct
 *
 * @param this Maze struct to be used
 * @param file Maze file pointer
 * @return int 0 on success, 1 on failure
 */
int read_maze(maze *this, FILE *file) {
    // Implementation goes here
    return 0;
}

/**
 * @brief Prints the maze out - code provided to ensure correct formatting
 *
 * @param this Pointer to maze to print
 * @param player The current player location
 */
void print_maze(maze *this, coord *player) {
    // make sure we have a leading newline..
    printf("\n");
    for (int i = 0; i < this->height; i++) {
        for (int j = 0; j < this->width; j++) {
            // decide whether player is on this spot or not
            if (player->x == j && player->y == i) {
                printf("X");
            } else {
                printf("%c", this->map[i][j]);
            }
        }
        // end each row with a newline.
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
    // Implementation goes here
}

/**
 * @brief Check whether the player has won and return a pseudo-boolean
 *
 * @param this Current maze
 * @param player Player position
 * @return int 0 for false, 1 for true
 */
int has_won(maze *this, coord *player) {
    // Implementation goes here
    return 0;
}

int main() {
    // Check args
    // Example:
    // if (argc != 2) {
    //     fprintf(stderr, "Usage: ./maze <mazefile>\n");
    //     return EXIT_ARG_ERROR;
    // }

    // Set up some useful variables
    coord *player = malloc(sizeof(coord));
    if (player == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return EXIT_FILE_ERROR;
    }

    maze *this_maze = malloc(sizeof(maze));
    if (this_maze == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        free(player);
        return EXIT_FILE_ERROR;
    }

    FILE *f = fopen("mazefile", "r");
    if (f == NULL) {
        fprintf(stderr, "Error opening file\n");
        free(player);
        free(this_maze);
        return EXIT_FILE_ERROR;
    }

    // Read in mazefile to struct
    if (read_maze(this_maze, f) != 0) {
        fprintf(stderr, "Error reading maze\n");
        fclose(f);
        free(player);
        free(this_maze);
        return EXIT_MAZE_ERROR;
    }

    // Initialize player position
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
    free_maze(this_maze);
    free(player);
    fclose(f);
    return EXIT_SUCCESS;
}