#ifndef _H_STR_UTILS_
#define _H_STR_UTILS_

char *seperate_string_by_token(char **string, char *delimeters);
void remove_all_spaces(char *str);
void remove_leading_and_trailing_spaces(char **str);
int contains_space(char *str);
int check_for_empty_line(char *command_str);
ErrorType readline(FILE *file, char *buf);
ErrorType openfile(char *path, ErrorType *err);

#endif
