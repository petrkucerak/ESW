/* Data structures for ESW Epoll example. */
#ifndef ESW_DS_H
#define ESW_DS_H

#define MAX_EVENTS 64
#define BUF_SIZE 1024
#define TMP_BUF_SIZE 42

typedef struct ep_entry {
   int fd;
   uint32_t count;
   char buffer[BUF_SIZE];
   enum { ESW_EPOLL_STDIN, ESW_EPOLL_TIMER, ESW_EPOLL_SERVER_INIT,ESW_EPOLL_SERVER_COMMUNICATION } type;
} ep_entry_t;

typedef struct ep_data {
   int ep_cnt;
   int epfd;
   ep_entry_t *ep_set[MAX_EVENTS];
} ep_data_t;

#endif /* ESW_DS_H */
