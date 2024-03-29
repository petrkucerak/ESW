#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <unistd.h>

#include "ds.h"
#include "handle.h"

ep_entry_t *add_timer(int epfd, unsigned int ms)
{
   ep_entry_t *e;
   struct timespec t = {.tv_sec = ms / 1000, .tv_nsec = (ms % 1000) * 1000000};
   struct itimerspec it = {.it_interval = t, .it_value = t};
   e = new_e();
   e->type = ESW_EPOLL_TIMER;
   e->fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
   if (e->fd == -1) {
      perror("add_timer timerfd_create");
      return NULL;
   }
   if (timerfd_settime(e->fd, 0, &it, NULL) == -1) {
      perror("add_timer timerfd_settime");
      return NULL;
   }
   add_e(epfd, e);
   return e;
}

int handle_timer(ep_entry_t *e)
{
   uint64_t in;
   if (read(e->fd, &in, 8) == -1) {
      return -1;
   }
   printf("timer: %d\n", e->fd);
   return 0;
}
