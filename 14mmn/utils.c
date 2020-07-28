#include "assembler.h"
/*
void strcat_dyn(char *dest, char *source)
{
    char string_size;
    char *new_dest;

    string_size = strlen(dest) + strlen(source);
    new_dest = realloc(dest, (string_size * sizeof(char)));

    strcat(new_dest, source);
}
*/

bool is_data_or_string(char *token)
{
    int i;
    char *dirc_type[3] = {".data", ".string", NULL};

    for(i=0 ; dirc_type[i] != NULL ; i++)
    {
        if (strstr(token, dirc_type[i]))
        {
            return TRUE;
        }
    }
    return FALSE;
}

char *get_label(line_node *curr_label_node)
{
    int i;
    char *token;

    for (i=0 ; curr_label_node->num_tokenz > i ; i++)
    {
        token = curr_label_node->tokenz[i];
        if(strchr(token, ':'))
        {
            return (strtok(token, ":"));
        }
    }
    return NULL;
}


bool is_label_decleration(line_node *curr_line_node, int tok_idx)
{
    int i, first_char = 0;
    char *token = curr_line_node->tokenz[tok_idx];

    if(strchr(token,':'))
    {
        if(!isupper(token[first_char]))
        {
            print_error(curr_line_node->line_num, "first letter of label is not upper case.");
        }

        for(i=0 ; (strlen(token)) > i ; i++)
        {
            if(!isalnum(token[i]) && !(token[i] == ':'))
            {
                print_error(curr_line_node->line_num, "valid label contains only A-Z,a-z,0-9 characters.");
            }
        }
        return TRUE;
    }
    return FALSE;
}

FILE *file_open(char *file_name, char *file_type, char *mode)
{
    if (strstr((const char*)file_name, (const char*)file_type) != NULL)
    {
        FILE *file =fopen(file_name, mode);
        if (file == NULL)
        {
            printf("[ERROR] File:%s failed to open\n.",file_name);
            return  NULL;
        }
        else
        {
            printf("[INFO] Successfully opened file:%s.as\n",file_name);
            return file;
        }
    }
    printf("[ERROR] Argument:%s is not \"%s\" type. Skipping...\n", file_name, file_type);
    return NULL;
}

void print_error(int line_num, const char *format, ...)
{
	va_list args;
	va_start(args, format);
	printf("[ERROR] In line %d: ", line_num);
	vprintf(format, args);
	printf("\n");
	va_end(args);
}








