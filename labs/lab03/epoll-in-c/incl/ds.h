/* Data structures for ESW Epoll example. */
#ifndef ESW_DS_H
#define ESW_DS_H

#define MAX_EVENTS 64

typedef struct ep_entry {
   int fd;
   enum { ESW_EPOLL_STDIN, ESW_EPOLL_TIMER, ESW_EPOLL_SERVER_INIT } type;
} ep_entry_t;

typedef struct ep_data {
   int ep_cnt;
   int epfd;
   ep_entry_t *ep_set[MAX_EVENTS];
} ep_data_t;

#endif /* ESW_DS_H */
