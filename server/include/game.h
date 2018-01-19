#ifndef GAME_H
#define GAME_H

#include <stdint.h>
#include <time.h>
#include "constants.h"

typedef struct {
    char *ip;
    uint8_t id;
    int sock;
    time_t timeout_start;
} players_local_t;

void init_game(void);
void register_player(int sock);
int handle_packet(int sock, void *packet);

// Handle join requests
void join_request(int sock, void *packet);
uint8_t player_exists(char *ipstr);
uint8_t add_player(int sock, join_request_t *request, char *ipstr);
void remove_player(uint8_t player_id);
uint8_t find_player_by_ip(char *ipstr, uint8_t *found);

// Handle keep alive messages
void keep_alive(int sock, void *packet);
int timeout_player(int sock);

void player_ready(int sock, void *packet);

#endif /* GAME_H */
