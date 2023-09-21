#ifndef COMPLETIONS_H
#define COMPLETIONS_H

void initialize_completions();
char** my_completion(const char* text, int start, int end);
char* file_generator(const char* text, int state);

#endif
