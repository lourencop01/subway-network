#ifndef __FUNCTIONS_H__
#define __FUNCTIONS_H__

#include "structs.h"

#include <stdio.h>
#include <stdlib.h>

/* ____________________UTILITY FUNCTIONS____________________ */

/*
 * Initializes the track list and the stop list
 */
list_heads_t* initialize_lists();

/*
 * Checks if memory allocation was successful
 */
void check_alloc(void* pointer);

/*
 * Checks if the user inputted the inverse option (or abbreviation)
 * 
 * Arguments:
 * *input: argument passed by user
 *
 * Return value:
 * 1 if the user inputted the inverse option, 0 otherwise
 */
size_t inv_abbreviation(char* name);

/*
 * Parses a name with spaces, enclosed in quotes (only used in p command)
 * 
 * Arguments:
 * *input: pointer to first token of input name
 * *track_name: pointer to where the name of the track will be stored
 *
 * Return value:
 * Pointer to the next input
 */
char* parse_name(char *input, char *track_name);

/*
 * Compares two strings alphabetically
 * 
 * Arguments:
 * *name1: first string
 * *name2: second string
 *
 * Return value:
 * 0 if the strings are equal, 1 if name2 is in front of name1, 
 * -1 if name1 is in front of name2
 */
int compare(char *name1, char *name2);


/* ____________________COMMAND PARSING FUNCTIONS____________________ */

/*
 * Parses the commands given by the user through stdin
 *
 * Possible Errors:
 * - User doesn't input any correct command (q, c, p, l, i)
 */
void command_parse(list_heads_t* list_heads);

/*
 * Quits the program and frees all the memory allocated
 * 
 * Arguments:
 * *list_heads: a pointer to the list heads
 */
void q_command(list_heads_t *list_heads);

/*
 * Adds and lists tracks
 * 
 * Arguments:
 * *input: a pointer to the remaining arguments that the user inputted
 *
 * Possible Errors:
 *  - Incorrect sort option
 */
void c_command(char *input, list_heads_t* list_heads);

/*
 * Adds and lists stops
 * 
 * Arguments:
 * *input: a pointer to the remaining arguments that the user inputted
 *
 * Possible Errors:
 *  - Stop already exists
 *  - No such stop
 */
void p_command(char *input, list_heads_t* list_heads);

/*
 * Adds connections between tracks by linking stops
 * 
 * Arguments:
 * *input: a pointer to the remaining arguments that the user inputted
 *
 * Possible Errors:
 *  - No such line
 *  - No such stop
 *  - Link cannot be associated with bus line
 *  - Negative cost or duration
 */
void l_command(char *input, list_heads_t* list_heads);

/*
 * Lists intersections between tracks
 */
void i_command();



/* ____________________TRACK MANAGEMENT FUNCTIONS____________________ */

/*
 * Lists all stops
 * 
 * Arguments:
 * *stops_head: a pointer to the first stop node
 */
void list_stops(stop_node_t* stops_head);

/*
 * Looks up for a stop
 * 
 * Arguments:
 * *name: name of the stop to be looked up
 * *stops_head: a pointer to the first stop node
 *
 * Return value:
 * A pointer to the stop node if the stop is found, NULL otherwise
 */
stop_node_t* stop_lookup(char *name, stop_node_t* stops_head);

/*
 * Adds a stop
 * 
 * Arguments:
 * *name: name of the stop to be added
 * *lat: latitude of the stop to be added
 * *lon: longitude of the stop to be added
 * *stops_head: a pointer to the first stop node
 */
void add_stop(char *name, char *lat, char *lon, stop_node_t* stop_head);

/*
 * Lists all tracks
 * 
 * Arguments:
 * *tracks_head: a pointer to the first track node
 */
void list_tracks(track_node_t* tracks_head);

/*
 * Looks up for a track
 * 
 * Arguments:
 * *name: name of the track to be looked up
 * *tracks_head: a pointer to the first track node
 *
 * Return value:
 * A pointer to the track node if the track is found, NULL otherwise
 */
track_node_t* track_lookup(char *name, track_node_t* tracks_head);

/*
 * Lists all connections
 * 
 * Arguments:
 * *connections_head: a pointer to the first connection node (of a track)
 */
void list_connections(connection_node_t* connections_head);

/*
 * Lists all connections in inverted order
 * 
 * Arguments:
 * *connections_head: a pointer to the first connection node
 */
void list_connections_inverted(connection_node_t* connections_head);

/*
 * Adds a connection to the end of the list
 * 
 * Arguments:
 * *origin_stop_aux: a pointer to the origin stop node
 * *dest_stop_aux: a pointer to the destination stop node
 * *track_aux: a pointer to the track node
 */
void add_connection_end(stop_node_t* origin_stop_aux,
                            stop_node_t* dest_stop_aux,
                            track_node_t* track_aux);

/*
 * Adds a connection to the beginning of the list
 * 
 * Arguments:
 * *origin_stop_aux: a pointer to the origin stop node
 * *dest_stop_aux: a pointer to the destination stop node
 * *track_aux: a pointer to the track node
 */
void add_connection_start(stop_node_t* origin_stop_aux,
                            stop_node_t* dest_stop_aux, 
                            track_node_t* track_aux);

/*
 * Adds a track
 * 
 * Arguments:
 * *name: name of the track to be added
 * *tracks_head: a pointer to the first track node
 */
void add_track(char *name, track_node_t* tracks_head);

/*
 * Updates the origin_name parameter of a track
 * 
 * Arguments:
 * *track_node: the track to be altered
 * *stop_node: the stop to be used as origin
 */
void update_origin_name(track_node_t *track_node, stop_node_t *stop_node);

/*
 * Updates the destination_name parameter of a track
 * 
 * Arguments:
 * *track_node: the track to be altered
 * *stop_node: the stop to be used as destination
 */
void update_dest_name(track_node_t *track_node, stop_node_t *stop_node);

/*
 * Checks if a stop is already in a track
 * 
 * Arguments:
 * *stop_node: the stop to be checked
 * *track_node: the track to be checked
 *
 * Return value:
 * 1 if the stop is already in the track, 0 otherwise
 */
size_t already_in_track(stop_node_t *stop_node, track_node_t *track_node);


/*
 * Adds a track to a stop's list of tracks alphabetically
 * 
 * Arguments:
 * *name: the name to add
 * *name_list head: the stop's track list head
 *
 */
void add_track_name(char *name, track_name_list_t *name_list_head);

#endif
