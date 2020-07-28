#include "assembler.h"
/** for debug **/
void print_data_list()
{
    img_node *curr = g_data_head;
    while(curr != NULL)
    {
        printf("data: %d | address: %d\n",curr->data, curr->address);
        curr = curr->next_node;
    }
}
/** for debug **/
void print_line_nodes(line_node *line_list_head)
{
    char **tokenz;
    int i;
    line_node *curr_node = line_list_head;

    while(curr_node != NULL)
    {
        printf("line:%d ; ",curr_node->line_num);
        tokenz = curr_node->tokenz;
        for (i=0;curr_node->num_tokenz > i;i++ )
        {
            printf("[%s] ", tokenz[i]);
        }
        printf("\n");

        curr_node = curr_node->next_node;
    }
}
/** for debug **/
void print_curr_line_node(line_node *curr_node)
{
    char **tokenz;
    int i;

    printf("line:%d | ",curr_node->line_num);
    tokenz = curr_node->tokenz;
    for (i=0;curr_node->num_tokenz > i;i++ )
    {
        printf("[%s] ", tokenz[i]);
    }
    printf(" | num_tokenz:%d  ",curr_node->num_tokenz);
    printf(" | label_flag:%d  ",curr_node->label_flag);
    printf(" | error_flag:%d  ",curr_node->error_flag);

    printf("\n");
}
/** for debug **/
void print_curr_label_node(label_node *curr_node)
{
    printf("address:%d ",curr_node->address);
    printf(" | dirc_type:%s  ",curr_node->dirc_type);
    printf(" | label:%s  ",curr_node->label);

    printf("\n");
}

void pars_data(line_node *curr_line_node, int i, int *DC)
{

    if (strstr(curr_line_node->tokenz[i], ".string"))
    {
        char **token = curr_line_node->tokenz;
        i++;

        while(i<curr_line_node->num_tokenz)
        {
            char *p_token = token[i];

            while (*p_token!= '\0')
            {
                if (*p_token != '"')
                {
                    printf("%c", *p_token);
                    if (isalpha(*p_token))
                    {
                        create_data_node(*DC, (int)*p_token);
                        (*DC)++;
                    }
                    else
                    {
                        curr_line_node->error_flag = TRUE;
                        print_error(curr_line_node->line_num, "string data type allows only A-Z, a-z chars.");
                        return;
                    }
                }
                (p_token)++;
            }
            i++;
        }
        return;
    }

    if(strstr(curr_line_node->tokenz[i], ".data"))
    {
        i++;

        while(i<curr_line_node->num_tokenz)
        {
            char *p_token;
            char *token;

            token = malloc(strlen(curr_line_node->tokenz[i]) * sizeof(char));
            strcpy(token,curr_line_node->tokenz[i]);
            p_token = strtok(token,",");

            while (p_token != NULL)
            {
                printf("%c", *p_token);
                if (isdigit(*p_token) || ispunct(*p_token))
                {
                    create_data_node(*DC, atoi(p_token));
                    (*DC)++;
                }
                else
                {
                    curr_line_node->error_flag = TRUE;
                    print_error(curr_line_node->line_num, "integer data type allows only 16,777,214 to -16,777,214.");
                    return;
                }
                p_token = strtok(NULL, ",");

            }
            /*free(token);*/
            i++;
        }
        return;
    }

    while (curr_line_node->num_tokenz > i)
    {
        (*DC)++;
        i++;
    }
    (*DC)++;
}


void insert_label(line_node *curr_line_node, char *type, int *DC)
{
    label_node *curr_label_node = curr_line_node->label;
    char *label_str;

    curr_label_node->address = *DC;
    label_str = get_label(curr_line_node);
    curr_label_node->label = label_str;
    curr_label_node->dirc_type = type;
}


line_node *insert_set2line_list(line_node **line_node_head, int *line_count, char ***token_set, int *token_count)
{
    line_node *new_line_node;

    new_line_node = create_line_node(line_node_head);

    new_line_node->num_tokenz   = (*token_count);
    new_line_node->line_num     = (*line_count);
    new_line_node->tokenz       = (*token_set);

    return new_line_node;
}

/********************************************//**
 * \brief insert tokens into set, by allocating array of chars, copying token into it,
 *        reallocating token_set to new memory with the new token
 * \return None
 *
 ***********************************************/
void insert_token2set(char *token, char ***token_set, int *tok_count, int *tok_set_size)
{
    char **token_set_new;
    char *token_new = malloc(sizeof(char) * ((strlen(token) + 1)));;

    if (!token_new)
    {
        printf("[ERROR] Failed to allocate memory for token_new.\n");
        return;
    }
    strcpy(token_new, token);

    if (*tok_count >= (*tok_set_size))
    {
        token_set_new = realloc(*token_set, sizeof(*token_set_new) * (*tok_count + 1));
        if (!token_set_new)
        {
            printf("[ERROR] Failed to reallocate memory for token_set.\n");
            return;
        }
        *token_set = token_set_new;
    }

    (*token_set)[*tok_count] = token_new;
    (*tok_set_size)++;
}

/********************************************//**
 * \brief converts string of char to tokens and insert into set(array strings)
 *
 * \param token_set - array of char strings
 * \return None
 *
 ***********************************************/
void tokenize_line(char ***token_set, char *tmp_line, int *tok_count)
{
    char *token;
    char *delims = " \t\n";
    int tok_set_size=0;

    *tok_count=0;

    token = strtok(tmp_line, delims);

    while (token != NULL)
    {
        insert_token2set(token, token_set, tok_count, &tok_set_size);
        token = strtok(NULL, delims);
        (*tok_count)++;
    }

}

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
 * \brief check if line is a empty or comment and ignore it.
 *        convert line to tokens and store in array of tokens(token_set).
 *        free token_set
 *
 *
 * \param tmp_line - pointer to line in the file.
 * \return None
 *
 ***********************************************/
void line_parser(line_node **line_list_head, char *tmp_line, int *line_count, int *error_count, int *IC, int *DC)
{
    char **token_set = NULL;
    int token_count = 0, i;
    line_node *curr_line_node;

    char *directive=NULL;

    if(is_comment_empty(tmp_line, line_count))
    {
        return;
    }

    tokenize_line(&token_set, tmp_line, &token_count);
    curr_line_node = insert_set2line_list(line_list_head, line_count, &token_set, &token_count);

    for(i=0 ; i<curr_line_node->num_tokenz ; i++)
    {
        if (is_label_decleration(curr_line_node, i))
        {
            curr_line_node->label_flag = TRUE;
            continue;
        }

        if (is_data_or_string(curr_line_node->tokenz[i]))
        {
            if(curr_line_node->label_flag)
            {
                create_label_node(curr_line_node);
                insert_label(curr_line_node, "data", DC);
                pars_data(curr_line_node, i, DC);

                print_curr_line_node(curr_line_node);
                print_curr_label_node(curr_line_node->label);


                return;
            }
        }
        else if ((directive = strstr(curr_line_node->tokenz[i], ".extern")))
        {
            return;
        }
        else if ((directive = strstr(curr_line_node->tokenz[i], ".entry")))
        {
            /*add to external table*/
        }
        else
        {
        /*pars instructions here*/
        }

    }

        /*  debug   */
    print_curr_line_node(curr_line_node);
}



/********************************************//**
 * \brief read_line - reads a line from file of max length, replacing new line char with null char
 *       if line is larger then max len keep reading until new line is found or eof
 *
 * \param file - pointer to file
 * \param tmp_line - pointer to string read from a file
 * \param max_len - maximum length of line to read
 * \return TRUE if read line was successful, otherwise FALSE
 *
 ***********************************************/
bool read_line(FILE *file, char *tmp_line, int max_len)
{

    char *end_of_line;

    if (!fgets(tmp_line, max_len, file))
    {
        return FALSE;
    }

    end_of_line = strchr(tmp_line, '\n');

    if (!end_of_line)
    {
        char c;
        while ((c=fgetc(file)))
        {
            if (c == '\n' || c == EOF)
            {
                return FALSE;
            }
        }
    }

    return TRUE;
}


/********************************************//**
 * \brief first_read - organizes all the actions for the first pass described by the project booklet
 *
 * \param file - pointer to file
 * \param line_list_head - pointer to the start of linked list of type line_node
 *
 * \return no value returned
 *
 ***********************************************/
void first_read(FILE *file, line_node **line_list_head, int *error_count, int *line_count, int *IC, int *DC)
{
    char tmp_line[MAX_LINE_LEN];

    while(!feof(file))
    {
        if(read_line(file, tmp_line, MAX_LINE_LEN))
        {
            (*line_count)++;

            line_parser(line_list_head, tmp_line, line_count, error_count, IC, DC);

            /* head is prob error count important
            if ( line_list_head->error_flag)
            {
                ++(*error_count);
            }
            */
        }
        else if(!feof(file))
        {
            printf("[ERROR] Line %d is too long, max size of line:%d.\n", *line_count, MAX_LINE_LEN);
            ++(*line_count);
            ++(*error_count);
        }
    }

}

