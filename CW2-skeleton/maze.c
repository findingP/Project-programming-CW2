/**
 * @file maze.c
 * @author SunFanding
 * @brief Maze game implementation for COMP1921 Assignment 2
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_DIM 100
#define MIN_DIM 5

#define EXIT_SUCCESS 0
#define EXIT_ARG_ERROR 1
#define EXIT_FILE_ERROR 2
#define EXIT_MAZE_ERROR 3

typedef struct __Coord {
    int x;
    int y;
} coord;

typedef struct __Maze {
    char **map;
    int height;
    int width;
    coord start;
    coord end;
} maze;

/**
 * @brief Initialize maze structure with dynamic memory allocation
 * @param this Pointer to maze structure
 * @param height Maze height dimension
 * @param width Maze width dimension
 * @return 0 on success, 1 on allocation failure
 */
int create_maze(maze *this, int height, int width) {
    this->height = height;
    this->width = width;
    this->map = malloc(height * sizeof(char *));
    if (!this->map) return 1;

    for (int i = 0; i < height; i++) {
        this->map[i] = malloc(width * sizeof(char));
        if (!this->map[i]) {
            for (int j = 0; j < i; j++) free(this->map[j]);
            free(this->map);
            return 1;
        }
    }
    return 0;
}

/**
 * @brief Release allocated memory for maze structure
 * @param this Pointer to maze structure to free
 */
void free_maze(maze *this) {
    if (this->map) {
        for (int i = 0; i < this->height; i++) free(this->map[i]);
        free(this->map);
    }
}

/**
 * @brief Validate and calculate maze width from file
 * @param file Pointer to maze file
 * @return Valid width (5-100) or 0 for invalid format
 */
int get_width(FILE *file) {
    rewind(file);
    int width = -1;
    char line[MAX_DIM + 2];
    
    while (fgets(line, sizeof(line), file)) {
        size_t len = strlen(line);
        if (len > 0 && line[len-1] == '\n') line[--len] = '\0';
        
        // Handle empty line (allowed only at EOF)
        if (len == 0) {
            if (fgetc(file) != EOF) return 0;
            break;
        }
        
        // Validate characters
        for (size_t i = 0; i < len; i++) {
            if (!strchr("# SE", line[i])) return 0;
        }
        
        // Check consistent width
        if (width == -1) width = (int)len;
        else if ((int)len != width) return 0;
    }
    return (width >= MIN_DIM && width <= MAX_DIM) ? width : 0;
}

/**
 * @brief Validate and calculate maze height from file
 * @param file Pointer to maze file
 * @return Valid height (5-100) or 0 for invalid format
 */
int get_height(FILE *file) {
    rewind(file);
    int height = 0;
    char line[MAX_DIM + 2];
    
    while (fgets(line, sizeof(line), file)) {
        size_t len = strlen(line);
        if (len > 0 && line[len-1] == '\n') line[--len] = '\0';
        
        if (len == 0) {
            if (fgetc(file) != EOF) return 0;
            break;
        }
        height++;
    }
    return (height >= MIN_DIM && height <= MAX_DIM) ? height : 0;
}

/**
 * @brief Load maze data from file into structure
 * @param this Pointer to initialized maze structure
 * @param file Pointer to validated maze file
 * @return 0 on success, 1 for invalid maze configuration
 */
int read_maze(maze *this, FILE *file) {
    rewind(file);
    int s_count = 0, e_count = 0;
    char line[MAX_DIM + 2];
    
    for (int y = 0; y < this->height; y++) {
        if (!fgets(line, sizeof(line), file)) return 1;
        
        size_t len = strlen(line);
        if (len > 0 && line[len-1] == '\n') line[--len] = '\0';
        
        // Validate row length
        if ((int)len != this->width) return 1;
        
        // Process each character
        for (int x = 0; x < this->width; x++) {
            char c = line[x];
            this->map[y][x] = c;
            
            if (c == 'S') {
                this->start = (coord){x, y};
                s_count++;
            } else if (c == 'E') {
                this->end = (coord){x, y};
                e_count++;
            }
        }
    }
    return (s_count == 1 && e_count == 1) ? 0 : 1;
}

/**
 * @brief Display maze state with player position
 * @param this Pointer to maze structure
 * @param player Pointer to player coordinates
 */
void print_maze(maze *this, coord *player) {
    printf("\n");
    for (int y = 0; y < this->height; y++) {
        for (int x = 0; x < this->width; x++) {
            printf((x == player->x && y == player->y) ? "X" : "%c", this->map[y][x]);
        }
        printf("\n");
    }
}

/**
 * @brief Validate and execute player movement
 * @param this Pointer to maze structure
 * @param player Pointer to player coordinates
 * @param direction Movement input character
 */
void move(maze *this, coord *player, char direction) {
    coord new = *player;
    switch (tolower(direction)) {
        case 'w': new.y--; break;
        case 'a': new.x--; break;
        case 's': new.y++; break;
        case 'd': new.x++; break;
        default: return;
    }
    
    // Boundary checking
    if (new.x < 0 || new.x >= this->width || 
        new.y < 0 || new.y >= this->height) {
        printf("Movement out of bounds!\n");
    } 
    // Wall collision check
    else if (this->map[new.y][new.x] == '#') {
        printf("Cannot move through walls!\n");
    } 
    // Valid movement
    else {
        *player = new;
    }
}

/**
 * @brief Check victory condition
 * @param this Pointer to maze structure
 * @param player Pointer to player coordinates
 * @return 1 if player reached exit, 0 otherwise
 */
int has_won(maze *this, coord *player) {
    return player->x == this->end.x && player->y == this->end.y;
}

int main(int argc, char *argv[]) {
    // Argument validation
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <mazefile>\n", argv[0]);
        return EXIT_ARG_ERROR;
    }
    
    // File handling
    FILE *file = fopen(argv[1], "r");
    if (!file) {
        perror("File opening error");
        return EXIT_FILE_ERROR;
    }
    
    // Maze dimension validation
    int width = get_width(file);
    int height = get_height(file);
    if (!width || !height) {
        fclose(file);
        return EXIT_MAZE_ERROR;
    }
    
    // Maze initialization
    maze game_maze;
    if (create_maze(&game_maze, height, width) || 
        read_maze(&game_maze, file)) {
        fclose(file);
        free_maze(&game_maze);
        return EXIT_MAZE_ERROR;
    }
    fclose(file);
    
    // Game loop
    coord player = game_maze.start;
    char input;
    while (!has_won(&game_maze, &player)) {
        printf("Enter move (WASD/M/Q): ");
        scanf(" %c", &input);
        
        if (tolower(input) == 'm') {
            print_maze(&game_maze, &player);
        } else if (tolower(input) == 'q') {
            break;
        } else {
            move(&game_maze, &player, input);
        }
    }
    
    // End game handling
    if (has_won(&game_maze, &player)) {
        printf("Congratulations! You escaped the maze!\n");
    }
    
    // Cleanup
    free_maze(&game_maze);
    return EXIT_SUCCESS;
}