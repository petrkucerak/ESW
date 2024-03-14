/* Data structures for ESW Epoll example. */
#ifndef ESW_DS_H
#define ESW_DS_H

#define MAX_EVENTS 64

struct ep_entry {
   int fd;
   enum { ESW_EPOLL_STDIN, ESW_EPOLL_TIMER, ESW_EPOLL_SERVER_INIT } type;
};

#endif /* ESW_DS_H */
