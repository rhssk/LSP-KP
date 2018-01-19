#include <unistd.h>
#include <time.h>
#include "debug_macros.h"
#include "constants.h"
#include "common.h"
#include "game.h"

#define SERVER_TIMEOUT  1 // Time to wait for server response
#define BUFFER_SIZE     255

int sock;
uint8_t player_id;

int talk_to_server(int serv_sock)
{
    int ret;
    join_request_t join_request;
    char *msg = malloc(BUFFER_SIZE);
    char input[BUFFER_SIZE];
    char *input_result;
    char player_name[PLAYER_NAME_LENGTH + 2];
    size_t name_length;

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

    // Join request
    memset(msg, '\0', BUFFER_SIZE); // Clear msg
    join_request.packet_id = P_JOIN_REQUEST;
    strcpy(join_request.player_name, player_name);
    memcpy(msg, &join_request, sizeof(join_request));
    // Ask to join lobby every 10s in case of ongoing game etc.
    while ((ret = send_msg(sock, msg, BUFFER_SIZE, SERVER_TIMEOUT)) != C_OK) {
        sleep(1);
    }

    while (1) {
        ret = recv_msg(sock, msg, BUFFER_SIZE, SERVER_TIMEOUT);
        // Dont' care if receiving gets a time-out
        if (ret != C_OK && ret != C_TIMEOUT)
            goto error;
        if (ret == C_OK)
            handle_packet(msg);

        input_result = fgets(input, BUFFER_SIZE, stdin);
        // Ctrl+D by user, end program
        if (input_result == NULL) {
            log_info("Disconnecting");
            disconnect();
            // Skip check for connection errors
            ret = C_OK;
            goto error;
        } else {
            // Remove newline
            input[strlen(input) - 1] = '\0';
        }
        handle_input(input);
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

}

void join_response(void *packet)
{
    join_response_t response;

    memcpy(&response, packet, sizeof(response));
    if (response.response_code == S_OK)
        player_id = response.player_id;
    else
        keep_alive();

}

void lobby_status(void *packet)
{

}

void game_start(void *packet)
{

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
    printf("ID: %u\n", ready.player_id);
    debug("Asked to be marked as ready");

    send_msg(sock, &ready, BUFFER_SIZE, SERVER_TIMEOUT);
}

void handle_input(char *input)
{
    if (strcmp(input, "READY") == 0) {
        player_ready();
    }
}
