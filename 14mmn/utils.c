#include "assembler.h"

void cpy_num2data_list(int num, img_node *new_img_node)
{
    if (num < 0)
    {
        num = (~num) << DATA_BITS;
        new_img_node->data |= num;
        new_img_node->data = (~new_img_node->data);
        new_img_node->data &= ~(TRUE << A_BIT);
        new_img_node->data &= ~(TRUE << R_BIT);
        new_img_node->data &= ~(TRUE << E_BIT);
    }
    else
    {
        new_img_node->data |= (num << DATA_BITS);
    }
}

bool is_external_label(label_node *label_list_head, char *token)
{
    label_node *curr_label_node;

    curr_label_node = label_list_head;

    while (curr_label_node != NULL)
    {
        if (!strcmp(curr_label_node->label, token) && !strcmp(curr_label_node->dirc_type, "extern"))
        {
            return TRUE;
        }
        curr_label_node = curr_label_node->next_node;
    }
    return FALSE;
}

/********************************************//**
 * \brief takes a set of tokens and creates, initializes line node with tokens set
 *
 * \return the newly created line node
 ***********************************************/
line_node *insert_set2line_list(line_node **line_node_head, int *line_count, char ***token_set, int *token_count)
{
    line_node *new_line_node;

    new_line_node = create_line_node(line_node_head);

    new_line_node->num_tokenz   = (*token_count);
    new_line_node->line_num     = (*line_count);
    new_line_node->tokenz       = (*token_set);
    new_line_node->tok_idx      = 0;

    return new_line_node;
}

/********************************************//**
 * \brief check if a line of assembly code is a comment or an empty line
 *
 * \return TRUE if it is else FALSE
 ***********************************************/

bool is_comment_empty(char *tmp_line, int *line_count)
{
    char *str_ptr;

    str_ptr = tmp_line;

    if (*str_ptr == ';')
    {
        return TRUE;
    }

    while(*str_ptr != '\n')
    {
        if (*str_ptr == ';')
        {
            print_error(*line_count, "Comment line should start with \';\'.");
            return TRUE;
        }

        if (*str_ptr != '\t' && *str_ptr != ' ')
        {
            return FALSE;
        }
        str_ptr++;
    }
    return TRUE;
}
/********************************************//**
 * \brief check if register addressing and if legal register
 *
 * \return TRUE if register addressing else FALSE
 ***********************************************/
bool is_register_addressing(line_node *curr_line_node, char *token)
{
    if(*token == 'r')
    {
        int reg_num;

        token++;
        reg_num = atoi(token);
        if (reg_num >= 0 && reg_num <= 7)
        {
            return TRUE;
        }
        else
        {
            print_error(curr_line_node->line_num,"unrecognized register.");
            curr_line_node->error_flag = TRUE;
            return FALSE;
        }
    }
    return FALSE;
}
/********************************************//**
 * \brief check if label is existing and external
 *
 * \return TRUE if label existing and external else FALSE
 ***********************************************/
bool is_label_ext(line_node *line_list_head, char *token)
{
    line_node *line_node_ptr;

    line_node_ptr = line_list_head;

    while(line_node_ptr != NULL)
    {
        if(line_node_ptr->label != NULL)
        {
            if (!strcmp(line_node_ptr->label->label, token) && (!strcmp(line_node_ptr->label->dirc_type,"extern")))
            {
                return TRUE;
            }
        }
        line_node_ptr = line_node_ptr->next_node;
    }
    return FALSE;
}
/********************************************//**
 * \brief check if label
 *
 * \return true if it is else false
 ***********************************************/
bool is_label(char *token)
{
    bool result = FALSE;

    if(isupper(*token))
    {
        result = TRUE;
        while (*token)
        {
            if(isalnum(*token))
            {
                result = TRUE;
            }
            else
            {
                result = FALSE;
            }
            token++;
        }
    }
    return result;
}
/********************************************//**
 * \brief check if operand is relative addressing
 *
 * \return TRUE if it is else FALSE
 ***********************************************/
bool is_relative_addressing(char *token)
{
    bool result = FALSE;
    char *tmp_ptr = token;

    if (*tmp_ptr == '&')
    {

        result = TRUE;
        result = is_label((++tmp_ptr));
    }
    return result;
}
/********************************************//**
 * \brief check if direct addressing
 *
 * \return TRUE if is else FALSE
 ***********************************************/
bool is_direct_addressing(char *token)
{
    bool result = FALSE;

    result = is_label(token);
    return result;
}
/********************************************//**
 * \brief check if immidiate addressing
 *
 * \return TRUE if it is else FALSE
 ***********************************************/
bool is_immidiate_addressing(char *token)
{

    bool result = FALSE;
    char *tmp_ptr = token;


    if (*tmp_ptr == '#' )
    {
        tmp_ptr++;
        while(*tmp_ptr != '\0')
        {
            if (*tmp_ptr == '-' || isdigit(*tmp_ptr) )
            {
                result = TRUE;
            }
            else
            {
                result = FALSE;
                break;
            }
            tmp_ptr++;
        }
    }
    return result;
}


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








