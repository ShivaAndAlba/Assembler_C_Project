#include "assembler.h"

bool is_data_or_string(line_node *curr_line_node)
{
    int i,j;
    char *dirc_type[3] = {".data", ".string", NULL};
    char *token;

    token = curr_line_node->tokenz[0];

    for (j=0 ; j<curr_line_node->num_tokenz ; j++)
    {
        token=curr_line_node->tokenz[j];
        for(i=0 ; dirc_type[i] != NULL ; i++)
        {
            if (strstr(token, dirc_type[i]))
            {
                return TRUE;
            }
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


bool is_label_decleration(line_node *curr_line_node)
{
    int i, first_char = 0;
    int tok_idx = curr_line_node->tok_idx;
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
/*
bool clear_q_marks(char **p_token)
{
    char *p_runer;

    *p_token = strtok(*p_token, "\"");
    p_runer = *p_token;

    while (*p_runer!= '\0')
    {
        if (*p_runer == '"')
        {
            *p_runer = '\0';
            return TRUE;
        }
        if (isalpha(*p_runer))
        {
            p_runer++;
        }
        else
        {
            return FALSE;
        }
    }
    return TRUE;
}
*/

void print_error(int line_num, const char *format, ...)
{
	va_list args;
	va_start(args, format);
	printf("[ERROR] In line %d: ", line_num);
	vprintf(format, args);
	printf("\n");
	va_end(args);
}








