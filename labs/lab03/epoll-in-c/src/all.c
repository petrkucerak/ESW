#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <unistd.h>

#include "ds.h"
#include "handle.h"

ep_entry_t *new_e()
{
   ep_entry_t *e;
   e = (ep_entry_t *)malloc(sizeof(ep_entry_t));
   if (e == NULL) {
      perror("new_e malloc");
      exit(EXIT_FAILURE);
   }
   return e;
}

int add_e(int epfd, ep_entry_t *e)
{
   struct epoll_event ee;
   ee.events = EPOLLIN;
   ee.data.ptr = e;
   if (epoll_ctl(epfd, EPOLL_CTL_ADD, e->fd, &ee) == -1) {
      perror("add_e epoll_ctl");
      exit(EXIT_FAILURE);
   }
   return 0;
}

int del_e(int epfd, ep_entry_t *e)
{
   struct epoll_event ee = {.events = 0, .data.ptr = e};
   if (epoll_ctl(epfd, EPOLL_CTL_DEL, e->fd, &ee) == -1) {
      perror("del_e");
      exit(EXIT_FAILURE);
   }
   close(e->fd);
   free(e);
   e = NULL;
   return 0;
}

int handle_all(ep_data_t *ep)
{
   struct epoll_event events[MAX_EVENTS];
   int n, i;
   n = epoll_wait(ep->epfd, events, MAX_EVENTS, -1);
   if (n == -1) {
      if (errno == EINTR) {
         return -1;
      }
      perror("epoll_wait");
      exit(EXIT_FAILURE);
   }
   for (i = 0; i < n; i++) {
      ep_entry_t *e = (ep_entry_t *)events[i].data.ptr;
      if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP) ||
          !(events[i].events & EPOLLIN)) {
         // An error has occured on this fd, or the socket is not ready for
         // reading
         del_e(ep->epfd, e);
         continue;
      }
      switch (e->type) {
      case ESW_EPOLL_STDIN:
         handle_stdin(events[i].data.ptr);
         break;
      case ESW_EPOLL_TIMER:
         handle_timer(events[i].data.ptr);
         break;
      case ESW_EPOLL_SERVER_INIT:
         handle_server_connection(events[i].data.ptr, ep);
         break;
      case ESW_EPOLL_SERVER_COMMUNICATION:
         handle_server_communication(events[i].data.ptr, ep);
         break;
      default:
         return -1;
      }
   }
   return 0;
}
