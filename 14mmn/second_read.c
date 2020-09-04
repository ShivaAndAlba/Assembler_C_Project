#include "assembler.h"
void print_external_list(extern_list *extern_list_head)
{
    extern_list *curr_node = extern_list_head;

    while (curr_node != NULL)
    {
        printf("address:%d | label:%s\n",curr_node->address, curr_node->label);

        curr_node = curr_node->next_node;
    }
}


/*for debug*/
void print_label_list(label_node *label_head_list)
{
    label_node *curr_node = label_head_list;

    while(curr_node != NULL)
    {
        printf("address:%d | dirc_type:%s | entry_flag:%d | label:%s\n",curr_node->address,curr_node->dirc_type,curr_node->entry_flag,curr_node->label);

        curr_node = curr_node->next_node;
    }
}



label_node *find_label(label_node *label_head_list, char *token)
{
    label_node *curr_label_node;

    curr_label_node = label_head_list;

    while (curr_label_node != NULL)
    {
        if (!strcmp(token, curr_label_node->label))
        {
            return curr_label_node;
        }

        curr_label_node = curr_label_node->next_node;
    }
    return curr_label_node;
}

void relat_addr_inst_update(img_node *curr_inst_node, label_node *label_head_list, char *token, int relative_cmd_line_addr)
{
    int relative_addr = 0;
    label_node *curr_label_node;

    curr_label_node = find_label(label_head_list, token);
    relative_addr   = curr_label_node->address - relative_cmd_line_addr;

    cpy_num2data_list(relative_addr, curr_inst_node);
    curr_inst_node->data |= (TRUE << A_BIT);
}

void update_inst(label_node *label_node, img_node *curr_inst_node)
{
    if (( !strcmp(label_node->dirc_type, "code") || !strcmp(label_node->dirc_type, "data") ))
    {
        curr_inst_node->data |= (TRUE << R_BIT);
    }
    curr_inst_node->data |= (label_node->address << DATA_BITS);
}

bool direct_addr_inst_update(label_node *label_head_list, img_node *curr_inst_node, char *token)
{
    label_node *label_node;

    /*search for label in label list*/
    label_node = find_label(label_head_list, token);

    /*in case label not found return TRUE*/
    if (label_node == NULL)
    {
        return TRUE;
    }
    /*otherwise found label and update instruction node and return FALSE*/    update_inst(label_node, curr_inst_node);
    return FALSE;
}

void second_read(line_node *line_head_list, extern_list **extern_list_head){

    char* entry_label = NULL;
    bool entry_found = FALSE;
    line_node *curr_line_node;
    label_node *label_head_list;
    label_node *curr_label_node;
    img_node *curr_inst_node;
    int op_count = 0;
    char *token;
    int relative_cmd_line_addr=0;

    curr_inst_node = g_inst_head;

    /*here we create a label linked list to gain some efficiency when searching for label*/
    curr_line_node = line_head_list;
    label_head_list = create_label_list(line_head_list);

    print_label_list(label_head_list);

    /* start search in line node list */
    while (curr_line_node != NULL){

        curr_line_node -> tok_idx = 0;
        /*check if first token is label declartion*/
        if (is_label(curr_line_node -> tokenz[curr_line_node -> tok_idx])){
            curr_line_node -> tok_idx++;
        }

        /*check directory type- here we are looking for entry dirc and command dirc*/
        switch (dirc_type(curr_line_node))
        {

            case ENTRY_DIRC:
                /*start from first token*/
                entry_label = curr_line_node -> tokenz[curr_line_node -> tok_idx + 1];
                curr_label_node = label_head_list;

                /*search through label linked list */
                while (curr_label_node != NULL){
                    if (strcmp(curr_label_node -> label, entry_label) == 0){
                        curr_label_node -> entry_flag = TRUE;
                        entry_found = TRUE;
                        break;
                    }
                    curr_label_node = curr_label_node -> next_node;
                }
                /* reset search node pointer  */
                curr_label_node = label_head_list;

                if (entry_found == FALSE){
                    print_error(curr_line_node->line_num,".entry label %s not found in label list.\n", entry_label);
                    curr_line_node->error_flag = TRUE;
                }

                /* reset entry_found for the next .entry label search */
                entry_found = FALSE;
                curr_line_node = curr_line_node -> next_node;
                break;

            case COMMAND_LINE:
                /*restart operand count*/
                op_count = 0;
                curr_line_node->tok_idx++;

                /*go through operands and determine addressing type*/
                while (curr_line_node->num_tokenz > (curr_line_node->tok_idx))
                {
                    /*get next token*/
                    token = curr_line_node->tokenz[curr_line_node->tok_idx];
                    ++op_count;

                    /*check what type operand*/
                    switch (op_type(line_head_list, curr_line_node, token))
                    {
                        case DIRECT_ADDRESSING:
                            curr_inst_node = curr_inst_node->next_node;

                            /*check if operand is external label*/
                            if (is_external_label(label_head_list, token))
                            {
                                create_extern_node(extern_list_head, token, curr_inst_node);
                                curr_inst_node->data |= (TRUE << E_BIT);
                                break;
                            }
                            /*else regular label*/
                            else if (direct_addr_inst_update(label_head_list, curr_inst_node, token))
                            {
                                /*case in witch label was not found*/
                                print_error(curr_line_node->line_num,"label:\"%s\" does not exist",token);
                                curr_line_node->error_flag = TRUE;
                            }
                            break;

                        case RELATIVE_ADDRESSING:
                            relative_cmd_line_addr = curr_inst_node->address;
                            curr_inst_node = curr_inst_node->next_node;
                            relat_addr_inst_update(curr_inst_node, label_head_list, ++token, relative_cmd_line_addr);
                            break;

                        case IMMIDIATE_ADDRESSING:
                            curr_inst_node = curr_inst_node->next_node;

                    /*end of "switch case":operand type*/
                    }
                    /*next token*/
                    token = curr_line_node->tokenz[curr_line_node->tok_idx++];
                }
                /*next line and next instruction*/
                curr_inst_node = curr_inst_node->next_node;
                curr_line_node = curr_line_node -> next_node;
                /*end of case:command line*/
                break;

            /*default case: found .string\.data\external, move to next line_node*/
            default:
                curr_line_node = curr_line_node -> next_node;
                break;

        }
    }
}










