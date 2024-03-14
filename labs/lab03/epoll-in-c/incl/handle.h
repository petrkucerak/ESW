/* Process functions for ESW Epoll example. */
#ifndef ESW_HANDLE_H
#define ESW_HANDLE_H

#include "ds.h"

struct ep_entry *add_stdin(int epfd);
int handle_stdin(struct ep_entry *e);

struct ep_entry *add_timer(int epfd, unsigned int ms);
int handle_timer(struct ep_entry *e);

struct ep_entry *new_e();
int add_e(int epfd, struct ep_entry *e);
int del_e(int epfd, struct ep_entry *e);
int handle_all(int epfd);

struct ep_entry *add_server(int epfd, uint16_t port);

#endif /* ESW_HANDLE_H */
