/* Author: Lourenço Pacheco */
/* This file holds the implementation of every function related to managing 
    tracks and stops */

#include "functions.h"
#include "structs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void list_stops(stop_node_t* stops_head) {

    stop_node_t *aux = stops_head->next;

    /* Runs through list of stops */
    while (aux != NULL) {
        printf("%s: %16.12f %16.12f %d\n", aux->stop.name, 
                                            aux->stop.lat,
                                            aux->stop.lon, 
                                            aux->stop.n_tracks);

        aux = aux->next;
    }

}

stop_node_t* stop_lookup(char *name, stop_node_t* stops_head) {

    stop_node_t* aux = stops_head;

    /* Runs through list of stops */
    while (aux != NULL) {
        if (!strcmp(aux->stop.name, name)) {
            return aux;
        }
        aux = aux->next;
    }

    return NULL;

}

void add_stop(char *name, char *lat, char *lon, stop_node_t* stop_head) {
    
    stop_node_t* aux = stop_head;

    /* Checks if the name is too long */
    if (strlen(name) > (MAX_STOP_NAME-1)) {
        return;
    }

    /* Goes to the last stop */
    while (aux->next != NULL) {
        aux = aux->next;
    }

    /* Creates new stop and links it to the list */
    aux->next = (stop_node_t*)malloc(sizeof(stop_node_t));
    check_alloc((void*)aux->next);
    
    aux = aux->next;

    /* Initializes all of its parameters */
    aux->next = NULL;
    aux->stop.lat = atof(lat);
    aux->stop.lon = atof(lon);
    aux->stop.n_tracks = 0;
    memset(aux->stop.name, '\0', MAX_STOP_NAME);
    strcpy(aux->stop.name, name);

    /* Allocates memory for the track list and initializes it */
    aux->stop.tracks = (track_name_list_t*)malloc(sizeof(track_name_list_t));
    check_alloc((void*)aux->stop.tracks);

    aux->stop.tracks->next = NULL;
    memset(aux->stop.tracks->name, '\0', MAX_TRACK_NAME);

}

void list_tracks(track_node_t* tracks_head) {

    track_node_t *aux = tracks_head->next;
    
    /* This variable is for code readabilty reasons */
    track_t *aux_track = NULL;

    /* Runs through list of tracks */
    while (aux != NULL) {
        aux_track = &(aux->track);
        /* If the track has at least 1 stop */
        if (aux_track->origin_name != NULL) {
            printf("%s %s %s %d %.2f %.2f\n", aux_track->name, 
                                aux_track->origin_name,
                                aux_track->dest_name, 
                                aux_track->n_stops, 
                                aux_track->cost, 
                                aux_track->dur);
        }
        else {
            printf("%s %d %.2f %.2f\n", aux_track->name,
                                        (aux_track->n_stops-1),
                                        aux_track->cost, 
                                        aux_track->dur);
        }
        aux = aux->next;
    }
}

track_node_t* track_lookup(char *name, track_node_t* tracks_head) {

    track_node_t* aux = tracks_head;
    
    /* Runs through list of tracks */
    while (aux != NULL) {
        if (!strcmp(aux->track.name, name)) {
            return aux;
        }
        aux = aux->next;
    }

    return NULL;

}

void add_track(char *name, track_node_t* tracks_head) {

    track_node_t* aux = tracks_head;

    /* Checks if the name is too long */
    if (strlen(name) > (MAX_TRACK_NAME-1)) {
        return;
    }
    /* Goes to the last track */
    while (aux->next != NULL) {
        aux = aux->next;
    }

    /* Creates new track and links it to the list */
    aux->next = (track_node_t*)malloc(sizeof(track_node_t));
    check_alloc((void*)aux->next);
    
    aux = aux->next;

    /* Initializes all of its parameters */
    aux->next = NULL;
    aux->track.n_stops = 1;
    aux->track.cost = 0;
    aux->track.dur = 0;
    aux->track.origin_name = NULL;
    aux->track.dest_name = NULL;
    memset(aux->track.name, '\0', MAX_TRACK_NAME);
    strcpy(aux->track.name, name);

    /* Allocates memory for the connections list and initializes it */
    aux->track.connections = 
                        (connection_node_t*)malloc(sizeof(connection_node_t));
    check_alloc((void*)aux->track.connections);
    aux->track.connections->prev = NULL;
    aux->track.connections->next = NULL;
    aux->track.connections->connection.origin = NULL;
    aux->track.connections->connection.dest = NULL;
    
}

void list_connections(connection_node_t* connections_head) {

    connection_node_t *aux = connections_head;

    /* Runs through all connections */
    while (aux->next != NULL) {
        aux = aux->next;
        printf("%s, ", aux->connection.origin->name);

        /* If it's the last connection */
        if(aux->next == NULL) {
            printf("%s\n", aux->connection.dest->name);
            break;
        }
    }

}

void list_connections_inverted(connection_node_t* connections_head) {

    connection_node_t *aux = connections_head;

    /* Goes to the last connection */
    while (aux->next != NULL) {
        aux = aux->next;
    }

    /* Runs through all connections (inverted) */
    while(aux->prev != NULL) {
        printf("%s, ", aux->connection.dest->name);

        /* If it's the last connection */
        if(aux->prev->prev == NULL) {
            printf("%s\n", aux->connection.origin->name);
            break;
        }
        aux = aux->prev;
    }
}

void add_connection_end(stop_node_t* origin_stop_aux,
                            stop_node_t* dest_stop_aux, 
                            track_node_t* track_aux) {

    connection_node_t* aux = track_aux->track.connections;

    /* Checks if it can increase the number of tracks and add the track name to 
    the list of tracks or if it has been done before */
    if (!already_in_track(origin_stop_aux, track_aux)) {
        origin_stop_aux->stop.n_tracks++;
        add_track_name(track_aux->track.name, origin_stop_aux->stop.tracks);
    }
    if (!already_in_track(dest_stop_aux, track_aux)) {
        dest_stop_aux->stop.n_tracks++;
        add_track_name(track_aux->track.name, dest_stop_aux->stop.tracks);
    }

    /* Goes to the last connection */
    while (aux->next != NULL) {
        aux = aux->next;
    }

    /* Creates new connection and links it to the list */
    aux->next = (connection_node_t*)malloc(sizeof(connection_node_t));
    check_alloc((void*)aux->next);
    aux->next->prev = aux;

    aux = aux->next;

    /* Initializes all of its parameters */
    aux->next = NULL;
    aux->connection.origin = &(origin_stop_aux->stop);
    aux->connection.dest = &(dest_stop_aux->stop);

    update_dest_name(track_aux, dest_stop_aux);

}

void add_connection_start(stop_node_t* origin_stop_aux,
                            stop_node_t* dest_stop_aux, 
                            track_node_t* track_aux) {

    connection_node_t *top = track_aux->track.connections;

    connection_node_t* aux = top->next;

    connection_node_t *new = 
                        (connection_node_t*)malloc(sizeof(connection_node_t));
    check_alloc((void*)new);

    /* Checks if it can increase the number of tracks and add the track name to
    the list of tracks or if it has been done before */
    if (!already_in_track(origin_stop_aux, track_aux)) {
        origin_stop_aux->stop.n_tracks++;
        add_track_name(track_aux->track.name, origin_stop_aux->stop.tracks);

    }
    if (!already_in_track(dest_stop_aux, track_aux)) {
        dest_stop_aux->stop.n_tracks++;
        add_track_name(track_aux->track.name, dest_stop_aux->stop.tracks);
    }

    /* Links new connection to the list (in the beginning) and initialized it */
    new->next = aux;
    new->prev = top;
    top->next = new;
    aux->prev = new;
    new->connection.dest = &(dest_stop_aux->stop);
    new->connection.origin = &(origin_stop_aux->stop);
    

    update_origin_name(track_aux, origin_stop_aux);

}

void update_dest_name(track_node_t *track_node, stop_node_t *stop_node) {

    track_node->track.dest_name = stop_node->stop.name;

}

void update_origin_name(track_node_t *track_node, stop_node_t *stop_node) {

    track_node->track.origin_name = stop_node->stop.name;

}

size_t already_in_track(stop_node_t *stop_node, track_node_t *track_node) {

    connection_node_t *aux = track_node->track.connections;

    while (aux->next != NULL) {
        aux = aux->next;
        if (!strcmp(aux->connection.origin->name, stop_node->stop.name) ||
            !strcmp(aux->connection.dest->name, stop_node->stop.name)) {
            return 1;
        }
    }

    return 0;

}

void add_track_name(char *name, track_name_list_t *track_names_head) {

    track_name_list_t *prev = NULL;
    track_name_list_t *curr = NULL;

    /* Creates new node and initializes it */
    track_name_list_t *new_node = 
                        (track_name_list_t *)malloc(sizeof(track_name_list_t));
    strcpy(new_node->name, name);
    new_node->next = NULL;

    /* If it's the first node */
    if (track_names_head->next == NULL) {
        track_names_head->next = new_node;
        return;
    }

    curr = track_names_head->next;
    prev = track_names_head;

    /* Goes through the list until it finds the right place to insert the 
    new node */
    while (curr != NULL && compare(curr->name, name) < 0) {
        prev = curr;
        curr = curr->next;
    }

    /* If it's between the head and the first node */
    if (prev == track_names_head) {
        new_node->next = curr;
        track_names_head->next = new_node;

    } else {
        new_node->next = prev->next;
        prev->next = new_node;
    }
}
