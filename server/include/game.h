#ifndef GAME_H
#define GAME_H

#include <stdint.h>
#include <time.h>
#include "constants.h"

typedef struct {
    uint8_t id;
    int sock;
    time_t timeout_start;
} players_local_t;

void init_game(void);
void register_player(int client_sock);
int handle_packet(void *packet, int sock);

// Handle join requests
void join_request(void *packet, int sock);
uint8_t add_player(join_request_t *request, int sock);
void remove_player(uint8_t player_id);
uint8_t find_player_by_conn(uint8_t *found, int sock);

// Handle keep alive messages
void keep_alive(void *packet, int sock);
int timeout_player(int sock);

void player_ready(void *packet);

void send_updated_lobby(void);

#endif /* GAME_H */
