/* Author: Lourenço Pacheco */
/* File where structs are defined */

#ifndef __STRUCTS_H__
#define __STRUCTS_H__

#include <stdio.h>

/*
 * Struct for storing track names
 */
typedef struct track_name_list {

    char *name;
    struct track_name_list *next;

} track_name_list_t;

/*
 * Struct for storing a stop
 */
typedef struct stop {

    char *name;
    double lat;
    double lon;
    int n_tracks;
    track_name_list_t *tracks;

} stop_t;

/*
 * Struct for storing a connection between 2 stops
 */
typedef struct connection {

    stop_t *origin;
    stop_t *dest;
    double cost;
    double dur;

} connection_t;

/*
 * Struct for storing a connection and its next and previous nodes
 */
typedef struct connection_node {

    connection_t connection;
    struct connection_node *next;
    struct connection_node *prev;

} connection_node_t;

/*
 * Struct for storing a track
 */
typedef struct track {

    char *name;
    connection_node_t *connections;
    char *dest_name;
    char *origin_name;
    double cost;
    double dur;
    int n_stops;

} track_t;

/*
 * Struct for storing a stop and its next node
 */
typedef struct stop_node {

    stop_t stop;
    struct stop_node *next;

} stop_node_t;

/*
 * Struct for storing a track and its next node
 */
typedef struct track_node {

    track_t track;
    struct track_node *next;

} track_node_t;

/*
 * Struct for storing the head of the stop and track lists
 */
typedef struct list_heads {

    track_node_t *track_head;
    stop_node_t *stop_head;

} list_heads_t;

#endif
