#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <unistd.h>

#include "handle.h"
#include <bits/sigaction.h>

static volatile int rv0 = 0;

static void term(int sn)
{
   if (sn == SIGINT || sn == SIGTERM) {
      rv0 = 1;
   }
}

int main()
{
   int i;
   ep_data_t ep;
   ep.ep_cnt = 0;

   struct sigaction sa = {.sa_handler = term, .sa_flags = SA_RESTART};
   if (sigaction(SIGINT, &sa, NULL) == -1) {
      perror("sigaction");
      exit(EXIT_FAILURE);
   }
   if (sigaction(SIGTERM, &sa, NULL) == -1) {
      perror("sigaction");
      exit(EXIT_FAILURE);
   }
   ep.epfd = epoll_create1(0);
   if (ep.epfd == -1) {
      perror("epoll_create1");
      exit(EXIT_FAILURE);
   }
   ep.ep_set[ep.ep_cnt++] = add_stdin(ep.epfd);
   ep.ep_set[ep.ep_cnt++] = add_timer(ep.epfd, 1000);
   ep.ep_set[ep.ep_cnt++] = add_timer(ep.epfd, 1500);
   ep.ep_set[ep.ep_cnt++] = add_server(ep.epfd, 12345);

   for (i = 0; i < ep.ep_cnt; i++) {
      if (ep.ep_set[i] == NULL) {
         perror("add_...");
         exit(EXIT_FAILURE);
      }
   }

   while (handle_all(&ep) == rv0) {
      // printf("%d\n", ep.ep_cnt);
   }
   for (i = 0; i < ep.ep_cnt; i++) {
      del_e(ep.epfd, ep.ep_set[i]);
   }
   close(ep.epfd);
   return 0;
}
