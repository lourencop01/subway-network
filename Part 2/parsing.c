/* Author: Lourenço Pacheco */
/* File where the command parsing is done */

#include "functions.h"
#include "structs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUT_SIZE 65535


void command_parse(list_heads_t* list_heads) {

    size_t memory_was_freed = 0;

    /* Allocates space for storing the input commands */
    char *base = (char*)malloc(INPUT_SIZE*sizeof(char));

    char *copy = base;

    memset(base, '\0', BUFSIZ);

    /* Reads input from stdin and chooses which function to perform */
    while(fgets(copy, INPUT_SIZE, stdin) != NULL){

        copy[strlen(copy)-1] = '\0';

        if (memory_was_freed) {
            list_heads = initialize_lists();
            memory_was_freed = 0;
        }

        switch (copy[0]) {
        case 'q':
            q_command(list_heads);
            free(base);
            exit(0);
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
        case 'r':
            r_command(++copy, list_heads);
            break;
        case 'e':
            e_command(++copy, list_heads);
            break;
        case 'a':
            a_command(list_heads);
            memory_was_freed = 1;
            break;
        default:
            continue;
        }

    /* Makes copy point to the base again */
    copy = base;

    /* Sets the allocated memory back to \0 */
    memset(base, '\0', INPUT_SIZE);

    }

    free(base);

}

void q_command(list_heads_t *list_heads) {

    a_command(list_heads);

}

void a_command(list_heads_t *list_heads) {

    /* Frees all of the allocated memory */
    track_node_t *track_aux = list_heads->track_head;
    track_node_t *track_temp = track_aux;

    connection_node_t *conn_aux = NULL;
    connection_node_t *conn_temp = NULL;

    stop_node_t *stop_aux = list_heads->stop_head;
    stop_node_t *stop_temp = stop_aux;

    track_name_list_t *track_name_aux = NULL;
    track_name_list_t *track_name_temp = NULL;
    
    /* Runs through all tracks */
    while(track_aux != NULL) {
        conn_aux = track_aux->track.connections;
        conn_temp = conn_aux;
        /* Runs through all track's connections */
        while(conn_aux != NULL) {
            conn_temp = conn_aux->next;
            free(conn_aux);
            conn_aux = conn_temp;
        }
        track_temp = track_aux->next;
        free(track_aux->track.name);
        free(track_aux);
        track_aux = track_temp;
    }

    /* Runs through all stops */
    while(stop_aux != NULL) {
        track_name_aux = stop_aux->stop.tracks;
        track_name_temp = track_name_aux;
        /* Runs through all stop's track names */
        while(track_name_aux != NULL) {
            track_name_temp = track_name_aux->next;
            free(track_name_aux);
            track_name_aux = track_name_temp;
        }
        stop_temp = stop_aux->next;
        free(stop_aux->stop.name);
        free(stop_aux);
        stop_aux = stop_temp;
    }

    free(list_heads);

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
            /* Incorrect sort option */
            printf("incorrect sort option.\n");
            return;
        }

    /* Track does not exist */
    } else {
        /* Adds track */
        add_track(track_name, list_heads);

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
    token = strtok(input, " ");

    if (token != NULL && token[0] == '"') {

        input = parse_name(++input, &stop_name);
        quotation_aux = 1;
        token = strtok(input, " ");
    
    }

    /* No other arguments */
    if (token == NULL) {
        list_stops(list_heads->stop_head);
        return;
    }

    if (stop_name == NULL) {
        stop_name = token;
    }
    
    /* Checks if stop exists */
    if ((aux = stop_lookup(stop_name, list_heads->stop_head)) != NULL) {

        /* No other arguments */
        if ((token = strtok(NULL, " ")) == NULL) {
            printf("%16.12f %16.12f\n", aux->stop.lat, aux->stop.lon);

        } else {
            /* Stop already exists can't add it */
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

            /* No such stop  */
            printf("%s: no such stop.\n", stop_name);
            return;

        } else {
            /* Adds stop */
            stop_lat = token;
            /* Checks if input is correct */
            if ((token = strtok(NULL, " ")) == NULL) {
                return;
            }
            add_stop(stop_name, stop_lat, token, list_heads);
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

    /* Auxiliary variable that checks if the name had quotation marks */
    int quotation_aux[2] = {0, 0};

    /* Variable that stores the arguments */
    /* track name */
    char *token = strtok(input, " ");

    /* track node */
    track_node_t *track_aux = track_lookup(token, list_heads->track_head);

    /* check if exists */
    if (track_aux == NULL) {
        printf("%s: no such line.\n", token);
        return;
    }

    /* origin stop */
    token = strtok(NULL, " ");

    /* Checks if the name is enclosed in quotation marks */
    if (token[0] == '"') {

        quotation_aux[0] = 1;

        token = parse_name(token, &origin_stop_name);

        while (token[strlen(token)-1] != '"') {
            token = strtok(NULL, " ");
        }

    } else {
        origin_stop_name = token;
    }
    
    /* origin stop node */
    origin_stop_aux = stop_lookup(origin_stop_name, list_heads->stop_head);

    
    /* check if exists */
    if (origin_stop_aux == NULL) {
        printf("%s: no such stop.\n", token);
        return;
    }

     /* dest stop */
    token = strtok(NULL, " ");

    /* Checks if the name is enclosed in quotation marks */
    if (token[0] == '"') {

        quotation_aux[1] = 1;

        token = parse_name(token, &dest_stop_name);

        while (token[strlen(token)-1] != '"') {
            token = strtok(NULL, " ");
        }

    } else {
        dest_stop_name = token;
    }

    dest_stop_aux = stop_lookup(dest_stop_name, list_heads->stop_head);

    /* check if exists */
    if (dest_stop_aux == NULL) {
        printf("%s: no such stop.\n", token);
        return; 
    }

    /* cost */
    token = strtok(NULL, " ");
    cost = atof(token);


    token = strtok(NULL, " ");
    dur = atof(token);

    /* check if cost and duration are positive */
    if (cost < 0 || dur < 0) {
        printf("negative cost or duration.\n");
        return;
    }

    /* If track has no stops */
    if (track_aux->track.dest_name == NULL) {
        add_connection_end(origin_stop_aux, dest_stop_aux, track_aux, cost, 
                                                            dur, list_heads);
        update_origin_name(track_aux, origin_stop_aux);

    /* origin stop is last stop of track */
    } else if (!strcmp(origin_stop_aux->stop.name, 
                                                track_aux->track.dest_name)) {
        add_connection_end(origin_stop_aux, dest_stop_aux, track_aux, cost, 
                                                            dur, list_heads);

    /* dest stop is first stop of track */
    } else if (!strcmp(dest_stop_aux->stop.name, 
                                            track_aux->track.origin_name)) {
        add_connection_start(origin_stop_aux, dest_stop_aux, track_aux, cost,
                                                            dur, list_heads);
    } else {
        /* none is a beginning or end point */
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

void r_command(char *input, list_heads_t* list_heads) {

    track_node_t *aux_track = list_heads->track_head;
    track_node_t *aux = aux_track;

    stop_node_t *aux_stop = NULL;

    connection_node_t *aux_connection = NULL;

    /* input becomes track name */
    input = strtok(input, " ");

    /* Looks up for the track's node */
    if ((aux_track = track_lookup(input, list_heads->track_head)) == NULL) {
        printf("%s: no such line.\n", input);
        return;
    }

    /* Looks for previous track */
    while (aux->next != aux_track) {
        aux = aux->next;
    }

    aux->next = aux_track->next;

    aux_connection = aux_track->track.connections->next;

    /* Removes the track's name from the stops it had and frees all of the 
    memory related to the track */
    while (aux_connection != NULL) {

        aux_stop = stop_lookup(aux_connection->connection.origin->name, 
                                                        list_heads->stop_head);
        if (remove_track_name(aux_track->track.name, aux_stop->stop.tracks)) {
            aux_stop->stop.n_tracks--;
        }

        if (aux_connection->next == NULL) {
            aux_stop = stop_lookup(aux_connection->connection.dest->name, 
                                                        list_heads->stop_head);
            if (remove_track_name(aux_track->track.name,
                                                    aux_stop->stop.tracks)) {
                aux_stop->stop.n_tracks--;
            }
            free(aux_connection);
            break;
        }

        aux_connection = aux_connection->next;

        free(aux_connection->prev);
    }
    
    free(aux_track->track.connections);
    free(aux_track->track.name);
    free(aux_track);

}

void e_command(char *input, list_heads_t* list_heads) {

    stop_node_t *aux_stop = list_heads->stop_head;
    stop_node_t *temp_stop = list_heads->stop_head;

    track_node_t *aux_track = list_heads->track_head->next;

    track_name_list_t *aux_track_names = NULL;
    track_name_list_t *temp = NULL;

    connection_node_t *aux_connection = NULL;

    char *stop_name = NULL;
    char *token = strtok(input, " ");

    size_t quotation_aux = 0;

    /* Checks if the name is enclosed in quotation marks */
    if (token != NULL && token[0] == '"') {
        parse_name(++input, &stop_name);
        quotation_aux = 1;
    } else {
        stop_name = token;
    }

    /* Checks if stop exists */
    if ((aux_stop = stop_lookup(stop_name, list_heads->stop_head)) == NULL) {
        printf("%s: no such stop.\n", stop_name);
        return;
    }

    aux_track_names = aux_stop->stop.tracks->next;
    free(aux_stop->stop.tracks);

    /* Removes stop from tracks that have it */
    /* At least one track exists */
    while (aux_track != NULL) {

        /* Not in track */
        if ((aux_connection = already_in_track(aux_stop, aux_track)) == NULL) {
            aux_track = aux_track->next;


        /* In track */
        } else {

            /* Stop is in first connection */
            if (aux_connection->prev == aux_track->track.connections) {

                aux_track->track.connections->next = aux_connection->next;

                if (aux_connection->next != NULL) {

                    aux_connection->next->prev = aux_connection->prev;

                    /* Stop is origin */
                    if (aux_track->track.origin_name == aux_stop->stop.name) {
                        aux_track->track.cost -= 
                                                aux_connection->connection.cost;
                        aux_track->track.dur -= aux_connection->connection.dur;
                        update_origin_name(aux_track,
                                        stop_lookup(
                                        aux_connection->connection.dest->name, 
                                                        list_heads->stop_head));
                    } else {
                        aux_connection->next->connection.origin = 
                                            aux_connection->connection.origin;
                        aux_connection->next->connection.cost += 
                                                aux_connection->connection.cost;
                        aux_connection->next->connection.dur +=
                                                aux_connection->connection.dur;
                    }

                /* Only connection that exists */
                } else {

                    aux_track->track.cost = 0;
                    aux_track->track.dur = 0;

                    stop_lookup(aux_connection->connection.dest->name,
                                        list_heads->stop_head)->stop.n_tracks--;
                    update_origin_name(aux_track, NULL);
                    update_dest_name(aux_track, NULL);

                }

            /* Stop is destination */
            } else if (aux_connection->next == NULL) {

                aux_connection->prev->next = NULL;


                aux_track->track.cost -= aux_connection->connection.cost;
                aux_track->track.dur -= aux_connection->connection.dur;
                update_dest_name(aux_track,
                                    stop_lookup(
                                    aux_connection->connection.origin->name, 
                                                    list_heads->stop_head));


            /* Stop is in an intermediate connection */
            } else {

                aux_connection->next->connection.origin = 
                                            aux_connection->connection.origin;
                aux_connection->prev->next = aux_connection->next;
                aux_connection->next->prev = aux_connection->prev;
                aux_connection->next->connection.cost += 
                                                aux_connection->connection.cost;
                aux_connection->next->connection.dur += 
                                                aux_connection->connection.dur;


            }

            aux_track->track.n_stops--;

        }

        free(aux_connection);
    }

    /* Frees all of the stop's memory */

    while (aux_track_names != NULL) {
        temp = aux_track_names;
        aux_track_names = aux_track_names->next;
        free(temp);
    }

    while (temp_stop->next != aux_stop && temp_stop != NULL) {
        temp_stop = temp_stop->next;
    }

    temp_stop->next = aux_stop->next;

    free(aux_stop->stop.name);
    free(aux_stop);

    if (quotation_aux) {
        free(stop_name);
    }

}

