#ifndef PACKETS_H
#define PACKETS_H

#include <stdint.h>

typedef struct join_request {
    int8_t packet_id;
    char player_name[23]; // Without ending \0
} join_request_t;

typedef struct join_response {
    int8_t packet_id;
    int8_t response_code;
    int8_t player_id;
} join_response_t;

typedef struct lobby_status {
    int8_t packet_id;
    int8_t player_count;
    char player_name[23]; // TODO: allow multiple players
    uint8_t player_status;
} lobby_status_t;

typedef struct keep_alive {
    int8_t packet_id;
    int8_t player_id;
} keep_alive_t;

typedef struct player_ready {
    int8_t packet_id;
    int8_t player_id;
} player_ready_t;

typedef struct player_input {
    int8_t packet_id;
    int8_t player_id;
    int16_t player_input;
} player_input_t;

typedef struct disconnect {
    int8_t packet_id;
    int8_t player_id;
} disconnect_t;

typedef struct game_start {
    int8_t packet_id;
    int8_t player_count;
    // TODO: add all the left out fields
} game_start_t;

typedef struct map_update {
    int8_t packet_id;
    int16_t field_changes;
    // TODO: add all the left out fields
} map_update_t;

typedef struct objects {
    int8_t packet_id;
    int16_t timer;
    int8_t dynamite_count;
    // TODO: add all the left out fields
} objects_t;

typedef struct game_over {
    int8_t packet_id;
    int8_t player_count;
    // TODO: add all the left out fields
} game_over;

#endif /* PACKETS_H */
