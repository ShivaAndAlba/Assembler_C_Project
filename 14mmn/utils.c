#include "assembler.h"
/********************************************//**
 * \brief a checker to determine dirctive type
 *
 * \return a macro of a type of action
 ***********************************************/
int dirc_type(line_node *curr_line_node)
{
    int j;
    char *token;

    token = curr_line_node->tokenz[0];

    for (j=0 ; j<curr_line_node->num_tokenz ; j++)
    {
        token=curr_line_node->tokenz[j];

        if ((strstr(token, ".data") || strstr(token, ".string")))
        {
            return DATA_STRING_DIRC;
        }
        else  if (strstr(token, ".entry"))
        {
            return ENTRY_DIRC;
        }
        else  if (strstr(token, ".extern"))
        {
            return EXTERN_DIRC;
        }
    }
    return COMMAND_LINE;
}
/********************************************//**
 * \brief used to search for label in assembly code line
 *
 * \return pointer to the label else NULL
 ***********************************************/
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

/********************************************//**
 * \brief check to see if line has a label deceleration and if written correctly
 *
 * \return True if it does else False
 ***********************************************/
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
            curr_line_node->error_flag = TRUE;
        }

        for(i=0 ; (strlen(token)) > i ; i++)
        {
            if(!isalnum(token[i]) && !(token[i] == ':'))
            {
                print_error(curr_line_node->line_num, "valid label contains only A-Z,a-z,0-9 characters.");
                curr_line_node->error_flag = TRUE;
            }
        }
        return TRUE;
    }
    return FALSE;
}
/********************************************//**
 * \brief handle file opening
 *
 * \return pointer to opened file else NULL
 ***********************************************/
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
/********************************************//**
 * \brief print error formated message
 *
 * \return None
 ***********************************************/
void print_error(int line_num, const char *format, ...)
{
	va_list args;
	va_start(args, format);
	printf("[ERROR] In line %d: ", line_num);
	vprintf(format, args);
	printf("\n");
	va_end(args);
}








