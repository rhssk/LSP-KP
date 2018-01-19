#ifndef GAME_H
#define GAME_H

int talk_to_server(int serv_sock);
void handle_packet(void *packet);


void join_response(void *packet);

void lobby_status(void *packet);

void game_start(void *packet);

void map_update(void *packet);

void objects(void *packet);

void game_over(void *packet);

void join_response(void *packet);

void keep_alive(void);

void disconnect(void);

void handle_input(char *input);

void player_ready(void);

#endif /* GAME_H */
