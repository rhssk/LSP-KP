#ifndef PACKETS_H
#define PACKETS_H

#include <stdint.h>

#define PLAYER_NAME_LENGTH 23 // Without ending \0

/*********************************************************************/
typedef struct {
    uint8_t packet_id;
    char player_name[PLAYER_NAME_LENGTH];
} join_request_t;
/*********************************************************************/
typedef struct {
    uint8_t packet_id;
    uint8_t response_code;
    uint8_t player_id;
} join_response_t;
/*********************************************************************/
typedef struct {
    uint8_t player_id;
    char player_name[PLAYER_NAME_LENGTH];
    uint8_t player_status;
} player_status_t;

typedef struct {
    uint8_t packet_id;
    uint8_t player_count;
    player_status_t **players; // List of players with their statuses
} lobby_status_t;
/*********************************************************************/
typedef struct {
    uint8_t packet_id;
    uint8_t player_id;
} keep_alive_t;
/*********************************************************************/
typedef struct {
    uint8_t packet_id;
    uint8_t player_id;
} player_ready_t;
/*********************************************************************/
typedef struct {
    uint8_t packet_id;
    uint8_t player_id;
    int16_t player_input;
} player_input_t;
/*********************************************************************/
typedef struct {
    uint8_t packet_id;
    uint8_t player_id;
} disconnect_t;
/*********************************************************************/
typedef struct {
    uint8_t player_id;
    char player_name[PLAYER_NAME_LENGTH];
    uint16_t x_pos;
    uint16_t y_pos;
    uint8_t direction;
} player_pos_t;

typedef struct {
    uint8_t packet_id;

    uint8_t player_count;
    player_pos_t **players; // List of players with their positions

    uint8_t field_width;
    uint8_t field_height;
    uint16_t field_contents; // List of field objects (walls, boxes etc.)

    uint8_t dynamite_time;
} game_start_t;
/*********************************************************************/
typedef struct {
    uint8_t x_pos;
    uint8_t y_pos;
    uint8_t content;
} map_change_t;

typedef struct {
    uint8_t packet_id;
    int16_t field_change_count;
    map_change_t **map_changes; // List of all field squares that need updating
} map_update_t;
/*********************************************************************/
typedef struct {
    uint8_t x_pos;
    uint8_t y_pos;
} dynamite_t;

typedef struct {
    uint8_t x_pos;
    uint8_t y_pos;
} flame_t;

typedef struct {
    uint8_t x_pos;
    uint8_t y_pos;
    uint8_t type;
} powerup_t;

typedef struct {
    uint8_t player_id;
    uint8_t dead;
    uint8_t x_pos;
    uint8_t y_pos;
    uint8_t direction;
    uint8_t powerup;
    uint8_t dynamite_power;
    uint8_t movement_speed;
    uint8_t dynamite_count;
} player_active_status_t;

typedef struct {
    uint8_t packet_id;
    int16_t timer;

    uint8_t dynamite_count;
    dynamite_t **dynamites;

    uint8_t flame_count;
    flame_t **flames;

    uint8_t powerup_count;
    powerup_t **powerups;

    uint8_t player_count;
    player_active_status_t **players;
} objects_t;
/*********************************************************************/
typedef struct {
    uint8_t packet_id;
    uint8_t player_count;
    uint8_t *player_ids;
} game_over;

#endif /* PACKETS_H */
