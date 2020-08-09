#include "assembler.h"
/********************************************//**
 * \brief search for the end of linked list, create, initialize and insert a line_node
 *
 * \return newly created line_node
 ***********************************************/
line_node *create_line_node(line_node **head)
{
    line_node *tail = *head;
    line_node *new_node = malloc(sizeof(line_node));

    if (new_node == NULL)
    {
        printf("[ERROR] Failed to allocate memory for line_list_head.\n");
        exit(0);
    }

    if(tail == NULL)
    {
        *head = new_node;
        tail  = *head;
    }
    else
    {
        while(tail->next_node != NULL)
        {
            tail = tail->next_node;
        }
        tail->next_node = new_node;
        tail = tail->next_node;
    }
    tail->label_flag = FALSE;
    tail->error_flag = FALSE;
    tail->line_num   = 0;
    tail->num_tokenz = 0;
    tail->tokenz     = NULL;
    tail->next_node  = NULL;
    tail->tok_idx = 0;
    return tail;
}
/********************************************//**
 * \brief create and initialize label_node, label_node is attached to line_node struct
 *
 * \return None
 ***********************************************/
void create_label_node(line_node *curr_line_node)
{
    label_node *new_node = malloc(sizeof(label_node));

    if (new_node == NULL)
    {
        printf("[ERROR] Failed to allocate memory for label_list_head.\n");
        exit(0);
    }

    new_node->address   = 0;
    new_node->dirc_type = NULL;
    new_node->label     = NULL;
    curr_line_node->label = new_node;
}
/********************************************//**
 * \brief search for end of linked list of data, create and initialize a new node
 *
 * \return Node
 ***********************************************/
void create_data_node(int *DC, int tok_chr)
{
    img_node *tail = g_data_head;
    img_node *new_node = malloc(sizeof(img_node));

    if (new_node == NULL)
    {
        printf("[ERROR] Failed to allocate memory for line_list_head.\n");
        exit(0);
    }

    if(tail == NULL)
    {
        g_data_head = new_node;
        tail        = g_data_head;
    }
    else
    {
        while(tail->next_node != NULL)
        {
            tail = tail->next_node;
        }
        tail->next_node = new_node;
        tail = tail->next_node;
    }
    tail->address   = (*DC);
    tail->data      = tok_chr;
    tail->next_node = NULL;
    (*DC)++;
}
/********************************************//**
 * \brief pars string data and a null char at the end and insert to data list
 *
 * \return None
 ***********************************************/
void insert_string2data_list(int *DC, line_node *curr_line_node)
{
    char *p_token = curr_line_node->tokenz[curr_line_node->tok_idx];

    while (curr_line_node->tok_idx < curr_line_node->num_tokenz)
    {
        while (*p_token != '\0')
        {
            if(*p_token != '"')
            {
                if (isalpha(*p_token))
                {
                    create_data_node(DC, (int)*p_token);
                }
                else
                {
                    curr_line_node->error_flag = TRUE;
                    print_error(curr_line_node->line_num, "string data type allows only A-Z, a-z chars.");
                }
            }
            p_token++;
        }
        curr_line_node->tok_idx++;
    }
    create_data_node(DC, '\0');
}
/********************************************//**
 * \brief pars integer data and insert to data linked list
 *
 * \return None
 ***********************************************/
void insert_int2data_list(int *DC, line_node *curr_line_node)
{
    char *delims = ", \t";
    char *p_token = curr_line_node->tokenz[curr_line_node->tok_idx];
    char *token;

    while (curr_line_node->tok_idx < curr_line_node->num_tokenz)
    {
        token = strtok(p_token, delims);

        while(token != NULL)
        {
            if (isdigit(*token) || *token == '-' || *token == '+')
            {
                create_data_node(DC, atoi(token));
            }
            else if (isalpha(*token))
            {
                curr_line_node->error_flag = TRUE;
                print_error(curr_line_node->line_num, "integer data type allows only 16,777,214 to -16,777,214, can be with '+' symbol.");
            }
            token = strtok(NULL, delims);
        }
        p_token = curr_line_node->tokenz[++(curr_line_node->tok_idx)];
    }
}
/********************************************//**
 * \brief create instuction list
 *
 * \return newly created instruction list
 ***********************************************/
img_node *create_inst_node()
{
    img_node *tail = g_inst_head;
    img_node *new_node = malloc(sizeof(img_node));

    if (new_node == NULL)
    {
        printf("[ERROR] Failed to allocate memory for line_list_head.\n");
        exit(0);
    }

    if(tail == NULL)
    {
        g_inst_head = new_node;
        tail        = g_inst_head;
    }
    else
    {
        while(tail->next_node != NULL)
        {
            tail = tail->next_node;
        }
        tail->next_node = new_node;
        tail = tail->next_node;
    }
    tail->address = 0;
    tail->data    = 0;

    return tail;
}
