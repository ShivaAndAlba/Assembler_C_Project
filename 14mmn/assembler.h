#ifndef ASSEMBLER_H
	#define ASSEMBLER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>

/*-- Macros -- */
#define TRUE    1
#define FALSE   0
/* used to determine directory type */
#define COMMAND_LINE        0
#define DATA_STRING_DIRC    1
#define ENTRY_DIRC          2
#define EXTERN_DIRC         3
/* used to determine command type */
#define CMD_CODE            1
#define CMD_FUNC            2
/* used to describe instruction word bits */
#define E_BIT           0
#define R_BIT           1
#define A_BIT           2
#define FUNCT_BITS      3
#define REG_DEST_BITS   8
#define ADD_DEST_BITS   11
#define REG_SRC_BITS    13
#define ADD_SRC_BITS    16
#define OPCODE_BITS     18
/* used to determine operand addressing type */
#define IMMIDIATE_ADDRESSING        0
#define DIRECT_ADDRESSING           1
#define RELATIVE_ADDRESSING         2
#define DIRECT_REGISTER_ADDRESSING  3
/* used to determine operand count*/
#define FIRST_OP 1
#define SECOND_OP 2
#define NO_REG_NEEDED 0
#define NO_ARE_BITS_NEEDED -1
/* used to describe word A,R,E bits already described*/
#define DATA_BITS 3
/* -- Constants -- */
#define NO_OPERANDS             0
#define NO_ADDRESSING_NEEDED    0
#define ONE                     1
#define MAX_LINE_LEN            82
#define MAX_LABEL_LEN           31
#define NUM_CMD                 16
#define FULL_WORD_LEN           24

/*-- Data Structures -- */

typedef unsigned int bool;

typedef struct img{
    int address : 24;
    int data : 24;

    struct img* next_node;
}img_node;

typedef struct label{
    char *label;
    int address;
    char *dirc_type;
}label_node;

typedef struct line{
    int line_num;           /* number of line in source code                        */
    char **tokenz;          /* array of pointer containing all tokens from a line   */
    int tok_idx;            /* index to current token being processed               */
    int num_tokenz;         /* number of tokens in array                            */
    bool label_flag;        /* TRUE if label declaration                            */
    label_node *label;      /* pointer to label struct                              */
    bool error_flag;        /* TRUE if error in assembly code line is present       */

   struct line *next_node;
}line_node;

typedef enum{
    No_operand ,
    One_operand ,
    Two_operand
}operands;

typedef struct cmd{
    char *cmd_name;
    unsigned int cmd_code;
    unsigned int cmd_func;
    operands num_op;
}cmd_type;


/* == Global Variables == */
img_node *g_inst_head;      /* global pointer to instruction image       */
img_node *g_data_head;      /* global pointer to data image              */


/* == Functions Declerations == */

/*--First Read --*/
void first_read(FILE *file, line_node **line_head_list, int *error_cout, int *line_count, int *IC, int *DC);
void update_label_address(line_node *line_list_head, int *IC);


/*--Second Read --*/

/*-- Utilities  --*/
void add_opcode_and_funct(int cmd_idx, img_node *curr_inst_node);
int search_cmd(char *p_token);
void insert_label(line_node *curr_line_node, char *type, int DC, bool extern_entry);
line_node *insert_set2line_list(line_node **line_node_head, int *line_count, char ***token_set, int *token_count);
bool is_comment_empty(char *tmp_line, int *line_count);
void add_addressing_and_registers(img_node *curr_inst_node, int op_count, int adr_val, int reg_val);
bool is_register_addressing(line_node *curr_line_node, char *token);
bool is_label_ext(line_node *line_list_head, char *token);
bool is_relative_addressing(char *token);
bool is_immidiate_addressing(char *token);
bool is_direct_addressing(char *token);
char *get_label(line_node *curr_line_node);
int dirc_type(line_node *curr_line_node);
bool is_label_decleration(line_node *curr_line_node);
void print_error(int line_num, const char *format, ...);
void load_file(char *file_name);
FILE *file_open(char *file_name, char *file_type, char *mode);

/*-- Linked_List --*/
void add_num2data_list(char *token, int *IC, int are_bits);
void insert_int2data_list(int *DC, line_node *curr_line_node);
void insert_string2data_list(int *DC, line_node *curr_line_node);
void create_data_node(int *DC, int tok_chr);
line_node *create_line_node(line_node **head);
void create_label_node(line_node *);
bool next_node(line_node *curr_node);
img_node *create_inst_node(int *IC);

/*debug*/
void print_line_nodes(line_node *line_list_head);
void print_data_list();
void print_inst_node();
#endif
