/* Author: Lourenço Pacheco */
/* File where the command parsing is done */

#include "functions.h"
#include "structs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void command_parse(list_heads_t* list_heads) {

    /* Allocates space for storing the input commands */
    char *base = (char*)malloc(BUFSIZ*sizeof(char));

    char *copy = base;

    memset(base, '\0', BUFSIZ);

    /* Reads input from stdin and chooses which function to perform */
    while(fgets(copy, BUFSIZ, stdin) != NULL){

        copy[strlen(copy)-1] = '\0';

        switch (copy[0]){
        case 'q':
            q_command(list_heads);
            break;
        case 'c':
            c_command(++copy, list_heads);
            break;
        case 'p':
            p_command(++copy, list_heads);
            break;
        case 'l':
            l_command(++copy, list_heads);
            break;
        case 'i':
            i_command(list_heads);
            break;
        default:
            continue;
        }

    /* Makes copy point to the base again */
    copy = base;

    /* Sets the allocated memory back to \0 */
    memset(base, '\0', BUFSIZ);

    }

    free(base);

}

void q_command(list_heads_t *list_heads) {
    
    /* Frees all of the allocated memory */
    track_node_t *track_aux = list_heads->track_head;
    track_node_t *track_temp = track_aux;

    connection_node_t *conn_aux = NULL;
    connection_node_t *conn_temp = NULL;

    stop_node_t *stop_aux = list_heads->stop_head;
    stop_node_t *stop_temp = stop_aux;

    track_name_list_t *track_name_aux = NULL;
    track_name_list_t *track_name_temp = NULL;
    
    while(track_aux != NULL) {
        conn_aux = track_aux->track.connections;
        conn_temp = conn_aux;
        while(conn_aux != NULL) {
            conn_temp = conn_aux->next;
            free(conn_aux);
            conn_aux = conn_temp;
        }
        track_temp = track_aux->next;
        free(track_aux);
        track_aux = track_temp;
    }

    while(stop_aux != NULL) {
        track_name_aux = stop_aux->stop.tracks;
        track_name_temp = track_name_aux;
        while(track_name_aux != NULL) {
            track_name_temp = track_name_aux->next;
            free(track_name_aux);
            track_name_aux = track_name_temp;
        }
        stop_temp = stop_aux->next;
        free(stop_aux);
        stop_aux = stop_temp;
    }

    free(list_heads);

    /* Exits the program */
    exit(0);
}

void c_command(char *input, list_heads_t* list_heads) {

    /* Variable that stores the arguments */
    char *token = strtok(input, " ");

    char *track_name = NULL;
    
    track_node_t* aux = NULL;

    /* No other arguments */
    if (token == NULL) {
        
        list_tracks(list_heads->track_head);
        return;

    }
    
    track_name = token;
    
    /* Checks if the track already exists */
    if ((aux = track_lookup(track_name, list_heads->track_head)) != NULL) {
        
        token = strtok(NULL, " ");

        /* Track exists and next argument is "inverso" */
        if (token != NULL && inv_abbreviation(token)) {
            /* Checks if input is correct */
            if((token = strtok(NULL, " ")) != NULL) {
                return;
            }
            /* Lists connections in inverted order */
            list_connections_inverted(aux->track.connections);


        /* No other arguments */
        } else if (token == NULL) {
            /* List connections */
            list_connections(aux->track.connections);

        } else {
            /* ERRO Incorrect sort option */
            printf("incorrect sort option.\n");
            return;
        }

    /* Track does not exist */
    } else {
        /* Adds track */
        add_track(track_name, list_heads->track_head);

    }


}

void p_command(char *input, list_heads_t* list_heads) {
    
    /* Variable that stores the arguments */
    char *token = NULL;
    
    char *stop_name = NULL;
    char *stop_lat = NULL;

    stop_node_t* aux = NULL;

    int quotation_aux = 0;

    /* Checks if the name is enclosed in quotation marks */
    if (input[1] == '"') {

        stop_name = (char*)malloc(sizeof(char)*MAX_STOP_NAME);
        memset(stop_name, '\0', MAX_STOP_NAME);
        input = parse_name(++input, stop_name);

        quotation_aux = 1;
    
    }

    token = strtok(input, " ");

    if (token == NULL) {
        list_stops(list_heads->stop_head);
        return;
    }

    if (stop_name == NULL) {
        stop_name = token;
    }

    /* No other arguments */
    

    /* Checks if stop exists */
    if ((aux = stop_lookup(stop_name, list_heads->stop_head)) != NULL) {

        /* No other arguments */
        if ((token = strtok(NULL, " ")) == NULL) {
            printf("%16.12f %16.12f\n", aux->stop.lat, aux->stop.lon);

        } else {
            /* ERRO Stop already exists can't add it */
            printf("%s: stop already exists.\n", aux->stop.name);
            return;
        }

    /* Stop that doesn't exist */
    } else {

        if(quotation_aux == 0) {
            token = strtok(NULL, " ");
        }

        /* No other arguments */
        if (token == NULL) {

            /* No such stop ERRO */
            printf("%s: no such stop.\n", stop_name);
            return;

        } else {
            /* Adds stop */
            stop_lat = token;
            /* Checks if input is correct */
            if ((token = strtok(NULL, " ")) == NULL) {
                return;
            }
            add_stop(stop_name, stop_lat, token, list_heads->stop_head);
        }
    }

    if (quotation_aux == 1 && stop_name != NULL) {
        free(stop_name);
    }

}

void l_command(char *input, list_heads_t* list_heads) {

    double cost = 0;
    double dur = 0;

    stop_node_t* origin_stop_aux = NULL;
    stop_node_t* dest_stop_aux = NULL;

    char *origin_stop_name = NULL;
    char *dest_stop_name = NULL;

    int quotation_aux[2] = {0, 0};

    /* Variable that stores the arguments */
    /* track name */
    char *token = strtok(input, " ");

    /* track node */
    track_node_t *track_aux = track_lookup(token, list_heads->track_head);
    
    /* ERRO check if exists */
    if (track_aux == NULL) {
        printf("%s: no such line.\n", token);
        return;
    }

    /* origin stop */
    token = strtok(NULL, " ");

    /* Checks if the name is enclosed in quotation marks */
    if (token[0] == '"') {

        quotation_aux[0] = 1;

        origin_stop_name = (char*)malloc(sizeof(char)*MAX_STOP_NAME);
        memset(origin_stop_name, '\0', MAX_STOP_NAME);
        token = parse_name(token, origin_stop_name);

        while (token[strlen(token)-1] != '"') {
            token = strtok(NULL, " ");
        }

    } else {
        origin_stop_name = token;
    }
    
    /* origin stop node */
    origin_stop_aux = stop_lookup(origin_stop_name, list_heads->stop_head);
    
    /* ERRO check if exists */
    if (origin_stop_aux == NULL) {
        printf("%s: no such stop.\n", token);
        return;
    }

     /* dest stop */
    token = strtok(NULL, " ");

    /* Checks if the name is enclosed in quotation marks */
    if (token[0] == '"') {

        quotation_aux[1] = 1;

        dest_stop_name = (char*)malloc(sizeof(char)*MAX_STOP_NAME);
        memset(dest_stop_name, '\0', MAX_STOP_NAME);
        token = parse_name(token, dest_stop_name);

        while (token[strlen(token)-1] != '"') {
            token = strtok(NULL, " ");
        }

    } else {
        dest_stop_name = token;
    }

    dest_stop_aux = stop_lookup(dest_stop_name, list_heads->stop_head);

    /* ERRO check if exists */
    if (dest_stop_aux == NULL) {
        printf("%s: no such stop.\n", token);
        return; 
    }

    /* cost */
    token = strtok(NULL, " ");
    cost = atof(token);


    token = strtok(NULL, " ");
    dur = atof(token);

    /* ERRO check if cost and duration are positive */
    if (cost < 0 || dur < 0) {
        printf("negative cost or duration.\n");
        return;
    }

    /* If track has no stops */
    if (track_aux->track.dest_name == NULL) {
        add_connection_end(origin_stop_aux, dest_stop_aux, track_aux);
        update_origin_name(track_aux, origin_stop_aux);

    /* origin stop is last stop of track */
    } else if (!strcmp(origin_stop_aux->stop.name, 
                                                track_aux->track.dest_name)) {
        add_connection_end(origin_stop_aux, dest_stop_aux, track_aux);

    /* dest stop is first stop of track */
    } else if (!strcmp(dest_stop_aux->stop.name, 
                                            track_aux->track.origin_name)) {
        add_connection_start(origin_stop_aux, dest_stop_aux, track_aux);
    } else {
        /* ERRO none is a beginning or end point */
        printf("link cannot be associated with bus line.\n");
        return;
    }

    /* Updates track cost, duration and number of stops */
    track_aux->track.cost += cost;
    track_aux->track.dur += dur;
    track_aux->track.n_stops++;

    if (quotation_aux[0] == 1) {
        free(origin_stop_name);
    }

    if (quotation_aux[1] == 1) {
        free(dest_stop_name);
    }

}

void i_command(list_heads_t *list_heads) {

    track_name_list_t *aux_track_names = NULL;
    track_node_t *aux_track = list_heads->track_head->next;
    stop_node_t *aux_stop = list_heads->stop_head->next;

    /* check if there are no tracks */
    if (aux_track == NULL) {
        return;
    }

    /* runs through all stops */
    while (aux_stop != NULL) {

        /* if stop has more than one track */
        if (aux_stop->stop.n_tracks > 1) {
            printf("%s %d: ", aux_stop->stop.name, aux_stop->stop.n_tracks);
            
            /* gets all tracks where this stop exists */
            aux_track_names = aux_stop->stop.tracks->next;

            while (aux_track_names != NULL) {

                if(aux_track_names->next != NULL) {
                    printf("%s ", aux_track_names->name);
                } else {
                    printf("%s\n", aux_track_names->name);
                }

                aux_track_names = aux_track_names->next;
            }
        }
        aux_stop = aux_stop->next;
    }
}
