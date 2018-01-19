#include <arpa/inet.h>
#include <pthread.h>
#include "debug_macros.h"
#include "common.h"
#include "game.h"

#define BUFFER_SIZE     255
#define LOBBY_WAIT_TIME 30

// Shared variables
lobby_status_t *lobby;
players_local_t *players_local[MAX_PLAYERS];
uint8_t *server_status;
uint8_t *data_lock;

// Create shared memory which will be seen and used by all server instances
void init_game(void)
{
    lobby = malloc(sizeof(*lobby));
    lobby->packet_id = P_LOBBY_STATUS;
    lobby->player_count = 0;
    server_status = malloc(sizeof(*server_status));
    data_lock = malloc(sizeof(*data_lock));
    *server_status = S_WAITING;
    *data_lock = 0;
}

void register_player(int sock)
{
    uint8_t id, found;
    int port, ret, packet_handle_ret;
    char *ipstr;
    void *msg;

    ipstr = malloc(INET6_ADDRSTRLEN);
    get_remote_ip_port(sock, ipstr, &port);

    msg = malloc(BUFFER_SIZE);
    ret = recv_msg(sock, msg, BUFFER_SIZE, PLAYER_TIMEOUT);
    // Gets updated when client responds
    while (1) {
        packet_handle_ret = handle_packet(msg, sock);
        if (packet_handle_ret == -1) goto error;

        // Gets updated every second
        while (1) {
            // Timeout every 1s to allow the rest of logic to execute
            ret = recv_msg(sock, msg, BUFFER_SIZE, 1);
            if (*server_status == S_WAITING || *server_status == S_IN_GAME) {
                // Check if player should get a timeout
                if (timeout_player(sock) == 1) {
                    log_info("%s:%d disconnected for inactivity", ipstr, port);
                    goto error;
                }
            }

            if (ret == C_DISCONNECT) {
                log_info("%s:%d has disconnected", ipstr, port);
                goto error;
            } else if (ret == C_OK)
                break;
        }
    }

    return;
error:
    ;
    id = find_player_by_conn(sock, &found);
    if (found == 1) remove_player(id);

    free(ipstr);
    free(msg);
    pthread_exit(NULL);
}

int handle_packet(void *packet, int sock)
{
    uint8_t packet_id;

    packet_id = *((uint8_t *)packet) & 0xFF; // Get the first byte
    switch (packet_id) {
    case P_JOIN_REQUEST:
        join_request(packet, sock);
        break;
    case P_KEEP_ALIVE:
        keep_alive(packet, sock);
        break;
    case P_DISCONNECT:
        return -1;
        break;
    case P_PLAYER_READY:
        player_ready(packet);
        break;
    default:
        debug("Received packet with unrecognized id: %d", packet_id);
        break;
    }

    return 0;
}

/****************************************************************************/

void join_request(void *packet, int sock)
{
    uint8_t player_id;
    join_response_t response;
    join_request_t request;

    response.packet_id = P_JOIN_RESPONSE;
    if (*server_status == S_FULL) {
        response.response_code = S_FULL;
        debug("Join request denied, server full");
    } else if (*server_status == S_IN_GAME) {
        response.response_code = S_IN_GAME;
        debug("Join request denied, a game is in progress");
    } else if (*server_status == S_PREPARING) {
        response.response_code = S_PREPARING;
        debug("Join request denied, preparing to start the game");
    } else if (*server_status == S_WAITING) {
        uint8_t found;
        find_player_by_conn(&found, sock);
        if (found == 0) {
            debug("Join request accepted");
            memcpy(&request, packet, sizeof(request));
            player_id = add_player(&request, sock);
            send_updated_lobby();
            response.player_id = player_id;
            response.response_code = S_OK;
        } else {
            response.response_code = S_WAITING;
        }
    } else {
        response.response_code = S_ALREADY_CONNECTED;
        debug("Join request denied, player is already connected");
    }

    send_msg(sock, &response, BUFFER_SIZE, -1);
}

uint8_t add_player(join_request_t *request, int sock)
{
    players_local_t *pl;
    player_status_t *player;

    player = malloc(sizeof(*player));
    strcpy(player->player_name, request->player_name);
    player->player_id = lobby->player_count;
    player->player_status = PL_NOT_READY;

    // Add the player to lobby
    lobby->players[lobby->player_count] = player;
    lobby->player_count++;
    log_info("%s (id: %u) has joined the lobby", player->player_name,
             player->player_id);

    // Save player IP and port
    pl = malloc(sizeof(*pl));

    // Set start for player timeout
    time(&pl->timeout_start);

    // Save sock (used for identification between threads)
    pl->sock = sock;

    pl->id = player->player_id;
    players_local[player->player_id] = pl;

    return player->player_id;
}

void remove_player(uint8_t player_id)
{
    log_info("%s (id: %u) has left the lobby",
             lobby->players[player_id]->player_name,
             player_id);

    free(lobby->players[player_id]);

    free(players_local[player_id]);
    players_local[player_id] = NULL;

    lobby->player_count--;
}

uint8_t find_player_by_conn(uint8_t *found, int sock)
{
    size_t i;

    if (lobby->player_count == 0)
        return 0;

    for (i = 0; i < MAX_PLAYERS; ++i) {
        if (players_local[i] != NULL && players_local[i]->sock == sock) {
            *found = 1;
            return players_local[i]->id;
        }
    }
    return 0;
}

/****************************************************************************/
// Keep alive

void keep_alive(void *packet, int sock)
{
    size_t i;

    for (i = 0; i < lobby->player_count; i++) {
        if (players_local[i] != NULL && players_local[i]->sock == sock) {
            time(&players_local[i]->timeout_start);
        }
    }
}

int timeout_player(int sock)
{
    time_t current_time;
    double time_diff, time_precision;
    size_t i;

    time_precision = 0.01;

    time(&current_time);
    // Check if player should get a timeout
    for (i = 0; i < lobby->player_count; i++) {
        if (players_local[i] != NULL && players_local[i]->sock == sock) {
            time_diff = difftime(current_time,
                                 players_local[i]->timeout_start);
            if (time_diff - PLAYER_TIMEOUT > time_precision) {
                return 1;
            }
        }
    }
    return 0;
}

/****************************************************************************/
// Mark player in lobby as ready

void player_ready(void *packet)
{
    uint8_t player_id;
    size_t i;
    player_ready_t ready;

    memcpy(&ready, packet, sizeof(ready));
    player_id = ready.player_id;
    for (i = 0; i < lobby->player_count; i++) {
        if (i == player_id) {
            debug("%u marked as ready", player_id);
            lobby->players[i]->player_status = PL_READY;
            send_updated_lobby();
            break;
        }
    }
}

/****************************************************************************/
// Send the updated lobby to all players in the lobby

void send_updated_lobby(void)
{
    int client_sock;
    size_t i;

    debug("Sending lobby status");
    for (i = 0; i < lobby->player_count; i++) {
        client_sock = players_local[i]->sock;
        send_msg(client_sock, lobby, BUFFER_SIZE, -1);
    }
}
