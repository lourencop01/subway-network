/* Author: Lourenço Pacheco */
/* This file holds the implementation of utility functions */

#include "functions.h"
#include "structs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

list_heads_t* initialize_lists() {

    /* Initializes a list_heads_t struct (holds stop's and track's head) */
    list_heads_t *list_heads = (list_heads_t*)malloc(sizeof(list_heads_t));
    check_alloc((void*)list_heads, list_heads);

    /* Initializes the track list head */
    list_heads->track_head = (track_node_t*)malloc(sizeof(track_node_t));
    list_heads->track_head->next = NULL;
    list_heads->track_head->track.connections = NULL;
    list_heads->track_head->track.cost = 0;
    list_heads->track_head->track.dur = 0;
    list_heads->track_head->track.n_stops = 0;
    list_heads->track_head->track.name = NULL;
    list_heads->track_head->track.dest_name = NULL;
    list_heads->track_head->track.origin_name = NULL;

    /* Initializes the stop list head */
    list_heads->stop_head = (stop_node_t*)malloc(sizeof(stop_node_t));
    list_heads->stop_head->next = NULL;
    list_heads->stop_head->stop.lat = 0;
    list_heads->stop_head->stop.lon = 0;
    list_heads->stop_head->stop.n_tracks = 0;
    list_heads->stop_head->stop.tracks = NULL;
    list_heads->stop_head->stop.name = NULL;

    return list_heads;
}

void check_alloc(void *pointer, list_heads_t *list_heads) {
    
    if (pointer == NULL) {
        /* Error allocating memory */
        printf("No memory.\n");
        q_command(list_heads);
    }

}

size_t inv_abbreviation(char *name) {

    /* Compares name to the abbreviations of inverso */
    if (!strcmp(name, "inv") || !strcmp(name, "inve") || !strcmp(name, "inver")
        || !strcmp(name, "invers") || !strcmp(name, "inverso")) {
        return 1;
    }

    return 0;
}

char* parse_name(char *input, char **name) {

    int i = 0;

    /* If input has spaces before the start of the name -> (    "name") */
    while (input[0] != '"') {
        input++;
    }

    /* Skips first '"' */
    input++;

    /* Finds where the name ends, skips over \0 placed by strtok */
    while (input[i] != '"') {
        if (input[i] == '\0') {
            input[i] = ' ';
        }
        i++;
    }

    *name = (char*)malloc(i*sizeof(char));
    memset(*name, '\0', i);
    memcpy(*name, input, i);

    /* Advances pointer */
    input += i+1;

    return input;

}

int compare(char *name1, char *name2) {

    int i = 0;

    /* Compares character by character */
    while (name1[i] != '\0' && name2[i] != '\0') {
        if (tolower(name1[i]) < tolower(name2[i])) {
            return -1;
        } else if (tolower(name1[i]) > tolower(name2[i])) {
            return 1;
        }
        i++;
    }
    
    /* If one of the names prefix is equal to other */
    if (name1[i] == '\0') {
        return -1;
    } else if (name2[i] == '\0') {
        return 1;   
    }

    /* If equal */
    return 0;
}
