#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include "debug_macros.h"
#include "constants.h"
#include "common.h"
#include "game.h"

#define SERVER_TIMEOUT      1 // Time to wait for server response
#define BUFFER_SIZE         255

int sock;
uint8_t player_id, needs_keeping_alive;
time_t timeout_start;

int talk_to_server(int serv_sock)
{
    int ret;
    join_request_t join_request;
    char *msg = malloc(BUFFER_SIZE);
    char input[BUFFER_SIZE];
    char player_name[PLAYER_NAME_LENGTH + 2];
    size_t name_length;
    ssize_t input_read_b;
    time_t timeout_end;
    double time_diff, time_precision;

    time_precision = 0.01;
    time(&timeout_start);
    sock = serv_sock;

    printf("Enter your user name:\n");
    if (fgets(player_name, PLAYER_NAME_LENGTH + 2, stdin) != NULL) {
        // Clean up the name
        name_length = strlen(player_name);
        if (name_length <  PLAYER_NAME_LENGTH + 2) {
            // Remove newline
            player_name[name_length - 1] = '\0';
        } else if (name_length ==  PLAYER_NAME_LENGTH + 2) {
            // Remove newline and null terminator
            player_name[PLAYER_NAME_LENGTH] = '\0';
            player_name[PLAYER_NAME_LENGTH + 1] = '\0';
        }
    }

    // Don't block if no input from stdin was received
    fcntl(0, F_SETFL, fcntl(0, F_GETFL) | O_NONBLOCK);

    while (1) {
        // Join request
        memset(msg, '\0', BUFFER_SIZE); // Clear msg
        join_request.packet_id = P_JOIN_REQUEST;
        strcpy(join_request.player_name, player_name);
        memcpy(msg, &join_request, sizeof(join_request));
        // Ask to join lobby or just get a fresh server status
        ret = send_msg(sock, msg, BUFFER_SIZE, -1);
        if (ret != C_OK) goto error;
        memset(msg, '\0', BUFFER_SIZE); // Clear msg
        ret = recv_msg(sock, msg, BUFFER_SIZE, -1);
        if (ret != C_OK) goto error;
        handle_packet(msg);

        debug("Needs keeping alive: %u", needs_keeping_alive);
        if (needs_keeping_alive) {
            time(&timeout_end);
            // Add 2 seconds because of sleep and SERVER_TIMEOUTS
            time_diff = difftime(timeout_end, timeout_start) + 2;
            if (time_diff - PLAYER_TIMEOUT > time_precision) {
                keep_alive();
                ret = recv_msg(sock, msg, BUFFER_SIZE, SERVER_TIMEOUT);
                if (ret == C_OK)
                    handle_packet(msg);
                // Reset timer start
                time(&timeout_start);
            }
        }

        // 1s to allow to enter a command. In reality it's simply a loop delay
        // because user input gets buffered and persists between loop iterations
        sleep(1);
        input_read_b = read(0, input, BUFFER_SIZE);
        if (input_read_b > 0) {
            // Remove newline
            input[input_read_b - 1] = '\0';
            // End program if received Q
            if (strcmp(input, "Q") == 0) {
                log_info("Disconnecting");
                disconnect();
                // Skip check for connection errors
                ret = C_OK;
                goto error;
            }
            handle_input(input);
        }

        // Dont' care if receiving gets a time-out
        if (ret != C_OK && ret != C_TIMEOUT)
            goto error;
    }

error:
    if (ret == C_DISCONNECT) {
        log_warn("Server has disconnected, exiting...");
    } else if (ret == C_TIMEOUT) {
        log_warn("You have been disconnected due to inactivity");
    }
    free(msg);
    return -1;
}

void handle_packet(void *packet)
{
    uint8_t packet_id;

    packet_id = *((uint8_t *)packet) & 0xFF; // Get the first byte
    debug("Packet id: %d", packet_id);
    switch (packet_id) {
    case P_JOIN_RESPONSE:
        join_response(packet);
        break;
    case P_LOBBY_STATUS:
        lobby_status(packet);
        break;
    case P_GAME_START:
        game_start(packet);
        break;
    case P_MAP_UPDATE:
        map_update(packet);
        break;
    case P_OBJECTS:
        objects(packet);
        break;
    case P_GAME_OVER:
        game_over(packet);
        break;
    default:
        debug("Received packet with unrecognized id");
        break;
    }

    return;
}

void keep_alive()
{
    keep_alive_t ka;
    void *packet;

    ka.packet_id = P_KEEP_ALIVE;
    ka.player_id = player_id;

    send_msg(sock, &ka, BUFFER_SIZE, SERVER_TIMEOUT);
    debug("Sent keep alive request");

}

void join_response(void *packet)
{
    join_response_t response;

    memcpy(&response, packet, sizeof(response));
    debug("join response code: %d", response.response_code);
    if (response.response_code == S_WAITING ||
        response.response_code == S_IN_GAME) {
        needs_keeping_alive = 1;
        return;
    }
    if (response.response_code == S_OK)
        player_id = response.player_id;
    needs_keeping_alive = 0;
}

void lobby_status(void *packet)
{
    debug("Received lobby status");
}

void game_start(void *packet)
{

    debug("Game has started");
}

void map_update(void *packet)
{

}

void objects(void *packet)
{

}

void game_over(void *packet)
{

}

void disconnect()
{
    disconnect_t discon;
    discon.packet_id = P_DISCONNECT;
    discon.player_id = player_id;

    send_msg(sock, &discon, BUFFER_SIZE, SERVER_TIMEOUT);
}

void player_ready()
{
    player_ready_t ready;
    ready.packet_id = P_PLAYER_READY;
    ready.player_id = player_id;
    debug("Asked to be marked as ready");

    send_msg(sock, &ready, BUFFER_SIZE, SERVER_TIMEOUT);
}

void handle_input(char *input)
{
    if (strcmp(input, "R") == 0) {
        player_ready();
    }
}
