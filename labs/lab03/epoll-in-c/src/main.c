#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <unistd.h>

#include "handle.h"

static volatile int rv0 = 0;

static void term(int sn)
{
   if (sn == SIGINT || sn == SIGTERM) {
      rv0 = 1;
   }
}

int main()
{
   int epfd, ep_cnt = 0, i;
   struct ep_entry *ep_set[MAX_EVENTS];
   struct sigaction sa = {.sa_handler = term, .sa_flags = SA_RESTART};
   if (sigaction(SIGINT, &sa, NULL) == -1) {
      perror("sigaction");
      exit(EXIT_FAILURE);
   }
   if (sigaction(SIGTERM, &sa, NULL) == -1) {
      perror("sigaction");
      exit(EXIT_FAILURE);
   }
   epfd = epoll_create1(0);
   if (epfd == -1) {
      perror("epoll_create1");
      exit(EXIT_FAILURE);
   }
   ep_set[ep_cnt++] = add_stdin(epfd);
   ep_set[ep_cnt++] = add_timer(epfd, 1000);
   ep_set[ep_cnt++] = add_timer(epfd, 1500);
   ep_set[ep_cnt++] = add_server(epfd, 12345);

   for (i = 0; i < ep_cnt; i++) {
      if (ep_set[i] == NULL) {
         perror("add_...");
         exit(EXIT_FAILURE);
      }
   }

   while (handle_all(epfd) == rv0) {
   }
   for (i = 0; i < ep_cnt; i++) {
      del_e(epfd, ep_set[i]);
   }
   close(epfd);
   return 0;
}
