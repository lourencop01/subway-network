/* iaed-23 - ist195817 - project1 */
/* Author: Lourenço Pacheco */

#include "functions.h"
#include "structs.h"

#include <stdio.h>

int main() {

    /* Initialize track and stop lists */
    list_heads_t *list_heads = initialize_lists();

    /* Parse the commands given by the user */
    command_parse(list_heads);

    return 0;
}
