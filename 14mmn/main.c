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
            extern_list *extern_list_head = NULL;

            first_read(file, &line_list_head, &error_cout, &line_count, &IC, &DC);
            update_label_address(line_list_head, &IC);

            /*for debug*/
            print_line_nodes(line_list_head);

            /*for debug*/
            print_data_list();

            /*for debug*/
            print_inst_node();

            if (error_cout > 0){
                printf("[INFO] Errors found on first read.\nExiting...\n");
                return 1;
            }
            else
            {
            /*second read*/
                printf("[INFO] Starting second read...\n");
                second_read(line_list_head, &extern_list_head);

                /*for debug*/
                print_inst_node();

            }
            /*for debug*/
            print_external_list(extern_list_head);

            printf("[INFO] Finished parsing file:%s\n",argv[i]);
        }
    }
    return 1;
}
