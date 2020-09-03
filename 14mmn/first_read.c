#include "assembler.h"

const cmd_type cmd_array[NUM_CMD] = {
    {"mov" ,  0, 0, Two_operand},
    {"cmp" ,  1, 0, Two_operand},
    {"add" ,  2, 1, Two_operand},
    {"sub" ,  2, 2, Two_operand},
    {"lea" ,  4, 0, Two_operand},
    {"clr" ,  5, 1, One_operand},
    {"not" ,  5, 2, One_operand},
    {"inc" ,  5, 3, One_operand},
    {"dec" ,  5, 4, One_operand},
    {"jmp" ,  9, 1, One_operand},
    {"bne" ,  9, 2, One_operand},
    {"jsr" ,  9, 3, One_operand},
    {"red" , 12, 0, One_operand},
    {"prn" , 13, 0, One_operand},
    {"rts" , 14, 0, No_operand},
    {"stop", 15, 0, No_operand}
};
/** for debug **/
void print_inst_node()
{
    img_node *curr_node = g_inst_head;

    printf("instructions:\n");
    while ( curr_node != NULL)
    {
        printf("address:%d | data:%.6x\n",curr_node->address, curr_node->data);
        curr_node = curr_node->next_node;
    }
}

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

    printf("line nodes:\n");
    while(curr_node != NULL)
    {
        printf("line:%d ; ",curr_node->line_num);
        tokenz = curr_node->tokenz;
        for (i=0;curr_node->num_tokenz > i;i++ )
        {
            printf("[%s] ", tokenz[i]);
        }

        if (curr_node->label_flag)
        {
            printf("\naddress: %d | dirc_type: %s | label:%s\n",curr_node->label->address,curr_node->label->dirc_type,curr_node->label->label);
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

/********************************************//**
 * \brief updates abeel address with IC
 *
 * \return none
 ***********************************************/

void update_label_address(line_node *line_list_head, int *IC)
{
    line_node *curr_node = line_list_head;

    while (curr_node != NULL)
    {
        if (curr_node->label_flag && !strcmp(curr_node->label->dirc_type, "data"))
        {
            curr_node->label->address += *IC;
        }
        curr_node = curr_node->next_node;
    }
}

/********************************************//**
 * \brief simple search of cmd in array
 *
 * \return index of cmd in array or -1 if failed
 ***********************************************/

int search_cmd(char *p_token)
{
    int i;

    for(i=0 ; i < NUM_CMD ; i++)
    {
        if (!strcmp(p_token, cmd_array[i].cmd_name))
        {
            return i;
        }
    }
    return -1;
}

/********************************************//**
 * \brief add opcode and function bits
 *
 * \return none
 ***********************************************/
void add_opcode_and_funct(int cmd_idx, img_node *curr_inst_node)
{
    int inst_24bit = 0;

    inst_24bit |= (cmd_array[cmd_idx].cmd_code << OPCODE_BITS);
    inst_24bit |= (cmd_array[cmd_idx].cmd_func << FUNCT_BITS );

    curr_inst_node->data |= inst_24bit;

    return;
}

/********************************************//**
 * \brief determine operand type
 *
 * \return type of operand defined in header file or -1 if failed
 ***********************************************/
int op_type(line_node *line_list_head, line_node *curr_line_node, char *token)
{
    if (is_immidiate_addressing(token))
    {
        return IMMIDIATE_ADDRESSING;
    }
    if (is_direct_addressing(token))
    {
        return DIRECT_ADDRESSING;
    }
    if (is_relative_addressing(token))
    {
        if (is_label_ext(line_list_head, ++token))
        {
            print_error(curr_line_node->line_num,"relative addressing allows only labels defined in source code.");
            curr_line_node->error_flag=TRUE;
            return -1;
        }
        return RELATIVE_ADDRESSING;
    }
    if (is_register_addressing(curr_line_node, token))
    {
        return DIRECT_REGISTER_ADDRESSING;
    }
    return -1;
}
/********************************************//**
 * \brief function to parsing operands, for each operand determine addressing and input into instruction word
 *
 * \return None
 ***********************************************/
void pars_oprnd(line_node *line_list_head, line_node *curr_line_node,img_node *curr_inst_node, int cmd_idx, int *IC)
{
    int op_count = 0;
    char *token;
    char *empty_token = NULL;
    char *cmd_name = NULL;

    while (curr_line_node->num_tokenz > curr_line_node->tok_idx)
    {
        token = curr_line_node->tokenz[curr_line_node->tok_idx];
        token = strtok(token, ",\0");

        while (token != NULL)
        {
            if((++op_count) > cmd_array[cmd_idx].num_op)
            {
                print_error(curr_line_node->line_num, "Too many operands for instruction.");
                curr_line_node->error_flag = TRUE;
                return;
            }

            switch (op_type(line_list_head, curr_line_node, token))
            {
                case IMMIDIATE_ADDRESSING:
                    printf("IMMIDIATE_ADDRESSING\n");
                    add_addressing_and_registers(curr_inst_node, op_count, IMMIDIATE_ADDRESSING, NO_REG_NEEDED);
                    add_num2data_list(token, IC, A_BIT);
                    break;

                case DIRECT_ADDRESSING:
                    printf("DIRECT_ADDRESSING\n");
                    add_addressing_and_registers(curr_inst_node, op_count, DIRECT_ADDRESSING, NO_REG_NEEDED);
                    add_num2data_list(empty_token, IC, NO_ARE_BITS_NEEDED);
                    break;

                case RELATIVE_ADDRESSING:
                    printf("RELATIVE_ADDRESSING\n");
                    cmd_name = cmd_array[cmd_idx].cmd_name;

                    if (!strcmp(cmd_name, "jsr") || !strcmp(cmd_name, "bne") || !strcmp(cmd_name, "jmp"))
                    {
                        add_addressing_and_registers(curr_inst_node, op_count, RELATIVE_ADDRESSING, NO_REG_NEEDED);
                        add_num2data_list(empty_token, IC, NO_ARE_BITS_NEEDED);
                    }
                    else
                    {
                        print_error(curr_line_node->line_num,"invalid command used with relative addressing.");
                        curr_line_node->error_flag = TRUE;
                    }
                    break;

                case DIRECT_REGISTER_ADDRESSING:
                    printf("DIRECT_REGISTER_ADDRESSING\n");
                    add_addressing_and_registers(curr_inst_node, op_count, DIRECT_REGISTER_ADDRESSING, atoi(++token));
                    break;

            }
            token = strtok(NULL, ",\0");

        }
        curr_line_node->tok_idx++;
    }
    if (op_count < cmd_array[cmd_idx].num_op)
    {
        print_error(curr_line_node->line_num, "Too few operands for instruction.");
        curr_line_node->error_flag = TRUE;
        return;
    }
    if (op_count == 0)
    {
        add_addressing_and_registers(curr_inst_node, op_count, NO_ADDRESSING_NEEDED, NO_REG_NEEDED);
    }

}

/********************************************//**
 * \brief here we pars instructions
 *
 * \return Nonde
 ***********************************************/
void pars_inst(line_node *line_list_head, line_node *curr_line_node, int *IC)
{
    img_node *curr_inst_node;
    char *p_token;
    int tok_idx = curr_line_node->tok_idx;
    int cmd_idx = -1;

    p_token = curr_line_node->tokenz[tok_idx];

    if (tok_idx >= curr_line_node->num_tokenz)
    {
        print_error(curr_line_node->line_num, "Too few operands for instruction.");
        curr_line_node->error_flag = TRUE;
        return;
    }

    cmd_idx = search_cmd(p_token);

    if(cmd_idx == -1)
    {
        print_error(curr_line_node->line_num, "unknown command.");
        curr_line_node->error_flag = TRUE;
        return;
    }

    curr_inst_node = create_inst_node(IC);
    add_opcode_and_funct(cmd_idx, curr_inst_node);
    curr_line_node->tok_idx++;

    pars_oprnd(line_list_head, curr_line_node, curr_inst_node, cmd_idx, IC);


    return;
}

/********************************************//**
 * \brief here we pars data of directive line(string\data)
 *
 * \return Nonde
 ***********************************************/
void pars_data(line_node *curr_line_node, int *DC)
{
    char *token_p = curr_line_node->tokenz[curr_line_node->tok_idx];

    if (strstr(token_p, ".string"))
    {
        curr_line_node->tok_idx++;
        insert_string2data_list(DC, curr_line_node);
    }
    else if (strstr(token_p, ".data"))
    {
        curr_line_node->tok_idx++;
        insert_int2data_list(DC, curr_line_node);
    }
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
    int token_count = 0;
    line_node *curr_line_node;

    if(is_comment_empty(tmp_line, line_count))
    {
        return;
    }

    tokenize_line(&token_set, tmp_line, &token_count);
    curr_line_node = insert_set2line_list(line_list_head, line_count, &token_set, &token_count);

    if (is_label_decleration(curr_line_node))
    {
        curr_line_node->label_flag = TRUE;
    }


    switch (dirc_type(curr_line_node))
    {
        case DATA_STRING_DIRC:
            if(curr_line_node->label_flag)
            {
                create_label_node(curr_line_node);
                insert_label(curr_line_node, "data", *DC, FALSE);

                    /* debug */
                printf("\n");
                print_curr_label_node(curr_line_node->label);
                printf("\n");
            }

            pars_data(curr_line_node, DC);
            break;

        case ENTRY_DIRC:
            break;

        case EXTERN_DIRC:
            create_label_node(curr_line_node);
            insert_label(curr_line_node, "extern", 0, TRUE);

                        /* debug */
            printf("\n");
            print_curr_label_node(curr_line_node->label);
            printf("\n");
            break;

        case COMMAND_LINE:
            if (curr_line_node->label_flag)
            {
                create_label_node(curr_line_node);
                insert_label(curr_line_node, "code", *IC, FALSE);

                    /* debug */
                printf("\n");
                print_curr_label_node(curr_line_node->label);
                printf("\n");
            }

            pars_inst(*line_list_head, curr_line_node, IC);
            break;
    }

    *error_count =+ curr_line_node->error_flag;
        /*  debug   */
    print_curr_line_node(curr_line_node);
    printf("\n\n");
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
        }
        else if(!feof(file))
        {
            printf("[ERROR] Line %d is too long, max size of line:%d.\n", *line_count, MAX_LINE_LEN);
            ++(*line_count);
            ++(*error_count);
        }
    }
}
