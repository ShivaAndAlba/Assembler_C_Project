#include "assembler.h"

int main(int argc, char *argv[])
{
    int i;

    if (argc < 2)
    {
        printf("[INFO] No files were supplied.\nExiting...\n");
    }

    for ( i=1;i<argc;i++)
    {
        FILE *file;

        printf("[INFO] Start of file parsing:%s\n",argv[i]);

        if((file = file_open(argv[i], ".as", "r")) != NULL)
        {
            int IC = 100, DC = 0, error_cout = 0, line_count = 0;
            line_node *line_list_head = NULL;

            first_read(file, &line_list_head, &error_cout, &line_count, &IC, &DC);
            update_label_address(line_list_head, &IC);

            /*for debug*/
            print_line_nodes(line_list_head);

            /*for debug*/
            print_data_list();

            /*for debug*/
            print_inst_node();

            /*second read*/
            printf("[INFO] Finished parsing file:%s\n",argv[i]);
        }
    }
    return 1;
}
