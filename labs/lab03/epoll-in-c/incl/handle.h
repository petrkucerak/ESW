/* Process functions for ESW Epoll example. */
#ifndef ESW_HANDLE_H
#define ESW_HANDLE_H

#include "ds.h"

ep_entry_t *add_stdin(int epfd);
int handle_stdin(ep_entry_t *e);

ep_entry_t *add_timer(int epfd, unsigned int ms);
int handle_timer(ep_entry_t *e);

ep_entry_t *new_e();
int add_e(int epfd, ep_entry_t *e);
int del_e(int epfd, ep_entry_t *e);
int handle_all(ep_data_t ep);

ep_entry_t *add_server(int epfd, uint16_t port);
int handle_new_connection(ep_entry_t *e);

#endif /* ESW_HANDLE_H */
