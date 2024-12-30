#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>

// Colores para la salida
#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define YELLOW "\033[1;33m"
#define BLUE "\033[0;34m"
#define RESET "\033[0m"

// Estructura para estadísticas
typedef struct {
    int total_tests;
    int passed_tests;
    int failed_tests;
    double total_time;
    char failed_tests_names[100][256];
    int failed_count;
} TestStats;

TestStats stats = {0};

// Estructura para el mapa
typedef struct {
    char **map;
    int width;
    int height;
    int player_count;
    int exit_count;
    int collectibles;
} Map;

// Función para liberar el mapa
void free_map(Map *map) {
    if (!map) return;
    
    for (int i = 0; i < map->height; i++)
        free(map->map[i]);
    free(map->map);
    free(map);
}

// Función para validar las paredes del mapa
int validate_walls(Map *map) {
    // Verificar primera y última fila
    for (int x = 0; x < map->width; x++) {
        if (map->map[0][x] != '1' || map->map[map->height - 1][x] != '1')
            return 0;
    }
    
    // Verificar primera y última columna
    for (int y = 0; y < map->height; y++) {
        if (map->map[y][0] != '1' || map->map[y][map->width - 1] != '1')
            return 0;
    }
    
    return 1;
}

// Función para cargar y validar el mapa
Map *load_map(const char *map_path) {
    FILE *fp = fopen(map_path, "r");
    if (!fp) {
        printf("%sError: No se puede abrir el archivo %s%s\n", RED, map_path, RESET);
        return NULL;
    }

    Map *map = calloc(1, sizeof(Map));
    if (!map) {
        fclose(fp);
        return NULL;
    }

    // Leer el mapa línea por línea
    char line[1024];
    char **temp;
    int y = 0;

    while (fgets(line, sizeof(line), fp)) {
        // Eliminar el salto de línea
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n')
            line[--len] = '\0';

        // Ignorar líneas vacías y EOF
        if (len == 0 || strcmp(line, "EOF") == 0)
            continue;

        // Actualizar el ancho del mapa
        if (map->width == 0)
            map->width = len;
        else if (len != (size_t)map->width) {
            printf("%sError: El mapa no es rectangular%s\n", RED, RESET);
            free_map(map);
            fclose(fp);
            return NULL;
        }

        // Redimensionar el array de líneas
        temp = realloc(map->map, (y + 1) * sizeof(char *));
        if (!temp) {
            free_map(map);
            fclose(fp);
            return NULL;
        }
        map->map = temp;

        // Copiar la línea
        map->map[y] = strdup(line);
        if (!map->map[y]) {
            free_map(map);
            fclose(fp);
            return NULL;
        }

        // Contar elementos en la línea
        for (size_t x = 0; x < len; x++) {
            switch (map->map[y][x]) {
                case 'P': map->player_count++; break;
                case 'E': map->exit_count++; break;
                case 'C': map->collectibles++; break;
            }
        }

        y++;
    }

    map->height = y;
    fclose(fp);

    // Validaciones básicas
    if (map->player_count != 1 || map->exit_count != 1 || map->collectibles < 1) {
        printf("%sError: Mapa inválido (P:%d, E:%d, C:%d)%s\n", 
               RED, map->player_count, map->exit_count, map->collectibles, RESET);
        free_map(map);
        return NULL;
    }

    // Validar paredes
    if (!validate_walls(map)) {
        printf("%sError: Las paredes del mapa están incompletas%s\n", RED, RESET);
        free_map(map);
        return NULL;
    }

    return map;
}

// Función para validar los caracteres del mapa
int validate_chars(const char *map_path, int is_bonus) {
    FILE *fp = fopen(map_path, "r");
    if (!fp) return 0;

    char line[1024];
    int line_num = 0;
    const char *valid_chars = is_bonus ? "01PECNM" : "01PEC";

    while (fgets(line, sizeof(line), fp)) {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n')
            line[--len] = '\0';

        if (len == 0 || strcmp(line, "EOF") == 0)
            continue;

        for (size_t i = 0; i < len; i++) {
            if (!strchr(valid_chars, line[i])) {
                printf("%sError: Caracter inválido '%c' en línea %d, posición %zu%s\n",
                       RED, line[i], line_num + 1, i + 1, RESET);
                printf("%sCaracteres válidos (%s): %s%s\n",
                       RED, is_bonus ? "bonus" : "mandatory", valid_chars, RESET);
                fclose(fp);
                return 0;
            }
        }
        line_num++;
    }

    fclose(fp);
    return 1;
}

// Función para mostrar el contenido del mapa
void print_map_content(const char *map_path) {
    FILE *fp = fopen(map_path, "r");
    if (!fp) return;

    printf("\nContenido del mapa:\n");
    printf("------------------------\n");

    char line[1024];
    int line_num = 0;

    while (fgets(line, sizeof(line), fp)) {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n')
            line[--len] = '\0';

        if (len == 0 || strcmp(line, "EOF") == 0)
            continue;

        printf("[%d]: %s\n", line_num++, line);
    }

    printf("------------------------\n");
    fclose(fp);
}

// Función para ejecutar un test
void run_test(const char *executable, const char *map_path, int expected_result, const char *test_name) {
    struct timeval start, end;
    gettimeofday(&start, NULL);

    stats.total_tests++;
    printf("\nProbando %s...\n", test_name);

    // Mostrar contenido del mapa
    FILE *check_file = fopen(map_path, "r");
    if (check_file) {
        fclose(check_file);
        print_map_content(map_path);
    }

    // Validar el mapa
    Map *map = load_map(map_path);
    int map_valid = (map != NULL);
    free_map(map);

    // Validar caracteres
    int chars_valid = check_file ? validate_chars(map_path, strstr(executable, "bonus") != NULL) : 0;

    // Ejecutar el programa
    int program_result = 0;
    if (map_valid && chars_valid) {
        char command[512];
        sprintf(command, 
            "cd .. && ("
            "./%s %s > /dev/null 2>&1 & "
            "pid=$!; "
            "sleep 0.1; "
            "if ps -p $pid > /dev/null; then "
            "    kill -9 $pid 2>/dev/null; "
            "    pkill -f %s; "
            "fi)", 
            executable, map_path, executable);
        program_result = system(command);
    }

    // Evaluar resultados
    int test_passed = 0;
    if (expected_result == 0)
        test_passed = map_valid && chars_valid && (program_result == 0);
    else
        test_passed = !map_valid || !chars_valid || (program_result != 0);

    if (test_passed) {
        stats.passed_tests++;
        printf("%s[✓] Test %s pasó correctamente%s\n", GREEN, test_name, RESET);
    } else {
        stats.failed_tests++;
        strcpy(stats.failed_tests_names[stats.failed_count++], test_name);
        printf("%s[✗] Test %s falló%s\n", RED, test_name, RESET);
    }

    // Calcular tiempo
    gettimeofday(&end, NULL);
    double time_taken = (end.tv_sec - start.tv_sec) * 1000.0;
    time_taken += (end.tv_usec - start.tv_usec) / 1000.0;
    stats.total_time += time_taken / 1000.0;
}

void run_mandatory_tests(void) {
    printf("\n%s=== INICIANDO TESTS MANDATORY ===%s\n", BLUE, RESET);
    printf("Probando la parte obligatoria (sin enemigos)\n");

    // Tests de validación de archivo
    run_test("so_long", "maps/empty_file.ber", 1, "Archivo vacío");
    run_test("so_long", "maps/empty_with_ber.ber", 1, "Archivo vacío con extensión .ber");
    run_test("so_long", "maps/no_extension", 1, "Sin extensión .ber");
    run_test("so_long", "maps/wrong_extension.txt", 1, "Extensión incorrecta");

    // Tests con mapas válidos
    run_test("so_long", "maps/mandatory/valid_map.ber", 0, "Mapa válido básico");
    run_test("so_long", "maps/mandatory/valid_large.ber", 0, "Mapa válido grande");

    // Tests con mapas inválidos
    run_test("so_long", "maps/invalid_chars.ber", 1, "Caracteres inválidos");
    run_test("so_long", "maps/multiple_players.ber", 1, "Múltiples jugadores");
    run_test("so_long", "maps/multiple_exits.ber", 1, "Múltiples salidas");
    run_test("so_long", "maps/no_exit.ber", 1, "Sin salida");
    run_test("so_long", "maps/no_collectibles.ber", 1, "Sin coleccionables");
    run_test("so_long", "maps/not_rectangular.ber", 1, "No rectangular");
    run_test("so_long", "maps/walls_incomplete.ber", 1, "Paredes incompletas");
    run_test("so_long", "maps/no_walls.ber", 1, "Sin paredes");
    run_test("so_long", "maps/map_with_spaces.ber", 1, "Mapa con espacios");

    // Tests de tamaño
    run_test("so_long", "maps/too_small_1x1.ber", 1, "Mapa 1x1");
    run_test("so_long", "maps/minimal_valid.ber", 0, "Mapa mínimo válido");
}

void run_bonus_tests(void) {
    printf("\n%s=== INICIANDO TESTS BONUS ===%s\n", BLUE, RESET);
    printf("Probando la parte bonus (con enemigos y animaciones)\n");

    // Tests básicos con enemigos
    run_test("so_long_bonus", "maps/bonus/valid_map.ber", 0, "Mapa válido con enemigos");
    run_test("so_long_bonus", "maps/bonus/valid_large.ber", 0, "Mapa grande con enemigos");
    
    // Tests específicos de bonus
    run_test("so_long_bonus", "maps/bonus/enemy_near_player.ber", 0, "Enemigo cerca del jugador");
    run_test("so_long_bonus", "maps/bonus/enemy_near_exit.ber", 0, "Enemigo cerca de salida");
    run_test("so_long_bonus", "maps/bonus/multiple_enemies.ber", 0, "Múltiples enemigos");
    run_test("so_long_bonus", "maps/bonus/blocked_path.ber", 0, "Camino bloqueado por enemigos");
    run_test("so_long_bonus", "maps/bonus/surrounded_player.ber", 0, "Jugador rodeado por enemigos");
    run_test("so_long_bonus", "maps/bonus/surrounded_exit.ber", 0, "Salida rodeada por enemigos");
    run_test("so_long_bonus", "maps/bonus/surrounded_collectibles.ber", 0, "Coleccionables rodeados");
    
    // Tests de validación específicos de bonus
    run_test("so_long_bonus", "maps/bonus/invalid_enemy_char.ber", 1, "Caracter de enemigo inválido");
}

int main(int argc, char **argv) {
    if (argc > 1 && strcmp(argv[1], "bonus") == 0)
        run_bonus_tests();
    else if (argc > 1 && strcmp(argv[1], "all") == 0) {
        run_mandatory_tests();
        run_bonus_tests();
    }
    else
        run_mandatory_tests();

    // Imprimir resumen
    printf("\n%s=== RESUMEN ===%s\n", BLUE, RESET);
    printf("Tests totales: %d\n", stats.total_tests);
    printf("Tests pasados: %d\n", stats.passed_tests);
    printf("Tests fallidos: %d\n", stats.failed_tests);
    printf("Tiempo total: %.2f segundos\n", stats.total_time);

    if (stats.failed_tests > 0) {
        printf("\nTests fallidos:\n");
        for (int i = 0; i < stats.failed_count; i++)
            printf("- %s\n", stats.failed_tests_names[i]);
    }

    return stats.failed_tests > 0 ? 1 : 0;
} 