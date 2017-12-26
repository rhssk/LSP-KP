#ifndef SERVER_H
#define SERVER_H

extern void server_mode(const unsigned long port);

void sig_handler(int signal);
int create_sock(unsigned long port);
int wait_for_client(int sock);
int recv_msg(int client);
int send_msg(int client);

#endif /* SERVER_H */
