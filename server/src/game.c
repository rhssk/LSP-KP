#include <arpa/inet.h>
#include <pthread.h>
#include "debug_macros.h"
#include "common.h"
#include "game.h"

#define BUFFER_SIZE     255
#define LOBBY_WAIT_TIME 30

lobby_status_t *lobby;
char *player_ip[MAX_PLAYERS];
player_ip_id_t *players_ip_id[MAX_PLAYERS];
uint8_t server_status;
uint8_t data_lock;

// Create shared memory which will be seen and used by all server instances
void init_game()
{
    lobby = malloc(sizeof(*lobby));
    lobby->player_count = 0;
    server_status = S_OK;
    data_lock = 0;
}

void join_player(int sock)
{
    uint8_t id, found;
    int port;
    char *ipstr;
    void *msg;

    msg = malloc(BUFFER_SIZE);
    while (1) {
        if (recv_msg(sock, msg, BUFFER_SIZE) == -1) goto error;
        handle_packet(sock, msg);
    }

    return;
error:
    ;
    ipstr = malloc(INET6_ADDRSTRLEN);
    get_remote_ip_port(sock, ipstr, &port);
    id = find_player_by_ip(ipstr, &found);
    if (found == 1) remove_player(id);
    free(ipstr);

    free(msg);
    pthread_exit(NULL);
}

void handle_packet(int sock, void *packet)
{
    uint8_t packet_id;

    packet_id = *((uint8_t *)packet) & 0xFF; // Get the first byte
    switch (packet_id) {
    case P_JOIN_REQUEST:
        join_request(sock, packet);
        break;
    case P_KEEP_ALIVE:
        /* keep_alive(sock, packet); */
        break;
    default:
        log_err("Received packet with unrecognized id");
        break;
    }

    return;
}

/****************************************************************************/

void join_request(int sock, void *packet)
{
    char *ipstr;
    int port;
    uint8_t player_id;
    join_response_t response;

    ipstr = malloc(INET6_ADDRSTRLEN);
    get_remote_ip_port(sock, ipstr, &port);

    response.packet_id = P_JOIN_RESPONSE;
    if (server_status == S_FULL) {
        response.response_code = S_FULL;
        debug("Join request denied, server full");
    } else if (server_status == S_IN_GAME) {
        response.response_code = S_IN_GAME;
        debug("Join request denied, a game is in progress");
    } else if (player_exists(ipstr) == 0) {
        join_request_t request;
        memcpy(&request, packet, sizeof(request));
        player_id = add_player(&request, ipstr);
        response.player_id = player_id;
        response.response_code = S_OK;
    } else {
        response.response_code = S_ALREADY_CONNECTED;
        debug("Join request denied, player is already connected");
    }
    free(ipstr);
}

uint8_t player_exists(char *ipstr)
{
    size_t i;

    if (lobby->player_count == 0)
        return 0;

    for (i = 0; i < lobby->player_count; ++i) {
        if (strcmp(ipstr, players_ip_id[i]->ip) == 0) {
            return 1;
        }
    }

    return 0;
}

uint8_t add_player(join_request_t *request, char *ipstr)
{
    char *ip;
    player_ip_id_t *pl;
    player_status_t *player;

    player = malloc(sizeof(*player));
    strcpy(player->player_name, request->player_name);
    player->player_id = lobby->player_count;
    player->player_status = S_OK;

    // Add the player to lobby
    lobby->players[lobby->player_count] = player;
    lobby->player_count++;
    log_info("Player %u has joined the lobby", player->player_id);

    // Save player IP
    ip = malloc(sizeof(*ip) * INET6_ADDRSTRLEN);
    memcpy(ip, ipstr, sizeof(*ip) * INET6_ADDRSTRLEN);
    player_ip[player->player_id] = ip;

    pl = malloc(sizeof(*pl));
    pl->ip = malloc(INET6_ADDRSTRLEN);
    memcpy(pl->ip, ipstr, INET6_ADDRSTRLEN);
    pl->id = player->player_id;
    players_ip_id[player->player_id] = pl;

    return player->player_id;
}

void remove_player(uint8_t player_id)
{
    free(lobby->players[player_id]);

    free(players_ip_id[player_id]->ip);
    free(players_ip_id[player_id]);
    players_ip_id[player_id] = NULL;

    lobby->player_count--;
    log_info("Player %u has left the lobby", player_id);
}

uint8_t find_player_by_ip(char *ipstr, uint8_t *found)
{
    size_t i;

    if (lobby->player_count == 0)
        return 0;

    for (i = 0; i < MAX_PLAYERS; ++i) {
        if (players_ip_id[i] != NULL && strcmp(ipstr, players_ip_id[i]->ip) == 0) {
            *found = 1;
            return players_ip_id[i]->id;
        }
    }
    return 0;
}

/****************************************************************************/
// Keep alive
