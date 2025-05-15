/**
 * @file maze.c
 * @author Sun Fanding
 * @brief Code for the maze game for COMP1921 Assignment 2
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
typedef struct __Coord {
    int x;
    int y;
} coord;

// Struct to represent the maze
typedef struct __Maze {
    char **map;     // 2D dynamic array storing the map
    int height;     // Maze height
    int width;      // Maze width
    coord start;    // Starting coordinates
    coord end;      // End coordinates
} maze;

/**
 * @brief Initialise a maze object - allocate memory and set attributes
 * @return 0 on success, 1 on failure
 */
int create_maze(maze *this, int height, int width) {
    this->height = height;
    this->width = width;
    this->map = (char **)malloc(height * sizeof(char *));
    if (!this->map) return 1;
    
    for (int i = 0; i < height; i++) {
        this->map[i] = (char *)malloc(width * sizeof(char));
        if (!this->map[i]) {
            for (int j = 0; j < i; j++) free(this->map[j]);
            free(this->map);
            return 1;
        }
    }
    return 0;
}

/**
 * @brief Free memory allocated for the maze
 */
void free_maze(maze *this) {
    for (int i = 0; i < this->height; i++) {
        free(this->map[i]);
    }
    free(this->map);
    this->map = NULL;
}

/**
 * @brief Validate and get maze width
 * @return Valid width or 0 (error)
 */
int get_width(FILE *file) {
    char line[MAX_DIM + 2]; // +2 for newline and null terminator
    fseek(file, 0, SEEK_SET);
    
    if (!fgets(line, sizeof(line), file)) return 0;
    int width = strlen(line) - 1; // Remove newline
    
    // Verify all lines have consistent length
    while (fgets(line, sizeof(line), file)) {
        if (line[strlen(line)-1] != '\n' || strlen(line)-1 != width) return 0;
    }
    return (width >= MIN_DIM && width <= MAX_DIM) ? width : 0;
}

/**
 * @brief Validate and get maze height
 */
int get_height(FILE *file) {
    fseek(file, 0, SEEK_SET);
    int height = 0;
    char line[MAX_DIM + 2];
    
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '\n') break; // Allow trailing empty line
        height++;
    }
    return (height >= MIN_DIM && height <= MAX_DIM) ? height : 0;
}

/**
 * @brief Read maze data from file
 * @return 0 on success, 1 on failure
 */
int read_maze(maze *this, FILE *file) {
    int s_count = 0, e_count = 0;
    fseek(file, 0, SEEK_SET);

    for (int i = 0; i < this->height; i++) {
        char line[MAX_DIM + 2];
        if (!fgets(line, sizeof(line), file)) return 1;
        
        for (int j = 0; j < this->width; j++) {
            this->map[i][j] = line[j];
            if (line[j] == 'S') {
                this->start.x = j;
                this->start.y = i;
                s_count++;
            } else if (line[j] == 'E') {
                this->end.x = j;
                this->end.y = i;
                e_count++;
            } else if (line[j] != '#' && line[j] != ' ' && line[j] != '\n') {
                return 1; // Invalid character
            }
        }
    }
    return (s_count == 1 && e_count == 1) ? 0 : 1;
}

/**
 * @brief Handle player movement
 */
void move(maze *this, coord *player, char direction) {
    coord new_pos = *player;
    switch (direction) {
        case 'W': case 'w': new_pos.y--; break;
        case 'S': case 's': new_pos.y++; break;
        case 'A': case 'a': new_pos.x--; break;
        case 'D': case 'd': new_pos.x++; break;
        default: return;
    }

    // Boundary check
    if (new_pos.x < 0 || new_pos.x >= this->width ||
        new_pos.y < 0 || new_pos.y >= this->height) {
        printf("Can't move outside the map!\n");
        return;
    }

    // Collision detection
    char target = this->map[new_pos.y][new_pos.x];
    if (target == '#' || target == '\n') {
        printf("Can't move through walls!\n");
    } else {
        *player = new_pos;
    }
}

/**
 * @brief Check if player reached the exit
 */
int has_won(maze *this, coord *player) {
    return (player->x == this->end.x && player->y == this->end.y);
}

int main(int argc, char *argv[]) {
    // Argument check
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <mazefile>\n", argv[0]);
        return EXIT_ARG_ERROR;
    }

    // Open file
    FILE *file = fopen(argv[1], "r");
    if (!file) {
        perror("Error opening file");
        return EXIT_FILE_ERROR;
    }

    // Get maze dimensions
    int width = get_width(file);
    int height = get_height(file);
    if (!width || !height) {
        fclose(file);
        return EXIT_MAZE_ERROR;
    }

    // Initialize maze
    maze this_maze;
    if (create_maze(&this_maze, height, width)) {
        fclose(file);
        return EXIT_MAZE_ERROR;
    }

    // Read maze data
    if (read_maze(&this_maze, file)) {
        free_maze(&this_maze);
        fclose(file);
        return EXIT_MAZE_ERROR;
    }
    fclose(file);

    // Initialize player position
    coord player = this_maze.start;
    char input;

    // Game loop
    while (1) {
        printf("Enter move (WASD/M/Q): ");
        scanf(" %c", &input);

        if (input == 'Q' || input == 'q') break;
        if (input == 'M' || input == 'm') {
            print_maze(&this_maze, &player);
            continue;
        }

        move(&this_maze, &player, input);
        if (has_won(&this_maze, &player)) {
            printf("\nCongratulations! You escaped!\n");
            break;
        }
    }

    // Free resources
    free_maze(&this_maze);
    return EXIT_SUCCESS;
}