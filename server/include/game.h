#ifndef GAME_H
#define GAME_H

#include <stdint.h>
#include "constants.h"

typedef struct {
    char *ip;
    uint8_t id;
} player_ip_id_t;

void init_game(void);
void join_player(int sock);
void handle_packet(int sock, void *packet);

// Handle join requests
void join_request(int sock, void *packet);
uint8_t player_exists(char *ipstr);
uint8_t add_player(join_request_t *request, char *ipstr);
void remove_player(uint8_t player_id);
uint8_t find_player_by_ip(char *ipstr, uint8_t *found);

// Handle keep alive messages

#endif /* GAME_H */
