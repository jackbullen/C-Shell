#include <readline/readline.h>
#include "completions.h"
#include <string.h>
#include <stdlib.h>
#include <glob.h>

char *file_generator(const char *text, int state);

// static const char* commands[] = {"cd", "ls", "echo", "exit", NULL};

void initialize_completions() {
    rl_attempted_completion_function = my_completion;
}

char** my_completion(const char* text, int start, int end) {
    char **matches = NULL;
    rl_attempted_completion_over = 0;

    if (start == 0) {
        matches = rl_completion_matches(text, file_generator);
    } else if (rl_line_buffer && rl_line_buffer[end-1] == '*') {
        // printf("\nwildcard %s_\n", rl_line_buffer);
        matches = rl_completion_matches(text, file_generator);

        if (matches) {
            char *all_matches = calloc(1,1);
            for (int i = 0; matches[i] != NULL; i++){
                all_matches = realloc(all_matches, strlen(all_matches) + strlen(matches[i]) + 2);
                strcat(all_matches, matches[i]);
                strcat(all_matches, " ");
            }
            // rl_insert_text(all_matches);
            // rl_line_buffer[rl_point-1] = '\0'; // null terminate at cursor position
            // strcat(rl_line_buffer, all_matches); // append all_matches to the buffer
            // rl_end += strlen(all_matches);
            // rl_redisplay();
            // printf("\n%s\n", rl_line_buffer);
            // rl_point = rl_end;
            // rl_redisplay();
            printf("\n%s\n", all_matches);
            free(all_matches);
        }

    } else {
        // printf("\nnormal cmd %s_ %d\n", rl_line_buffer, end);
        char *command_str = strndup(rl_line_buffer, start);
        char *command = strtok(command_str, " ");
        
        if (start > 0 && command && ( (strcmp(command, "cd")) == 0 || (strcmp(command, "ls") == 0) )) {
            printf("adding slash, command=%s\n", command);
            rl_completion_append_character = '/';
        }
        
        free(command_str);
    }

    return matches;
}


char *file_generator(const char *text, int state) {
    static glob_t globbuf;      
    static int glob_index;

    if (!state) {
        glob_index = 0;
        char pattern[] = "*";
        glob(pattern, 0, NULL, &globbuf);
    }

    if (glob_index < globbuf.gl_pathc) {
        // printf("\nMatch: %s\n", globbuf.gl_pathv[glob_index]);
        return strdup(globbuf.gl_pathv[glob_index++]);
    } else {
        globfree(&globbuf);
        return NULL;
    }
}