#ifndef ASSEMBLER_H
	#define ASSEMBLER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>

/*-- Macros -- */
#define TRUE 1
#define FALSE 0

/* -- Constants -- */
#define MAX_LINE_LEN 82
#define MAX_LABEL_LEN 31

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

/* == Global Variables == */
img_node *g_inst_head;
img_node *g_data_head;



/* == Functions Declerations == */

/*--First Read --*/
void first_read(FILE *file, line_node **line_head_list, int *error_cout, int *line_count, int *IC, int *DC);


/*--Second Read --*/


/*-- Utilities  --*/
bool clear_q_marks(char **p_token);
char *get_label(line_node *curr_line_node);
bool is_data_or_string(line_node *curr_line_node);
bool is_label_decleration(line_node *curr_line_node);
void print_error(int line_num, const char *format, ...);
void load_file(char *file_name);
FILE *file_open(char *file_name, char *file_type, char *mode);

/*-- Linked_List --*/
void insert_int2data_list(int *DC, line_node *curr_line_node);
void insert_string2data_list(int *DC, line_node *curr_line_node);
void create_data_node(int *DC, int tok_chr);
line_node *create_line_node(line_node **head);
void create_label_node(line_node *);
bool next_node(line_node *curr_node);

/*debug*/
void print_data_list();
#endif
