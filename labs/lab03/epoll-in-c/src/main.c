#include <arpa/inet.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
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
   for (i = 0; i < ep_cnt; i++) {
      if (ep_set[i] == NULL) {
         perror("add_...");
         exit(EXIT_FAILURE);
      }
   }

   int sfd = socket(AF_INET, SOCK_STREAM, 0);
   short int port = 12345;
   struct sockaddr_in saddr;

   memset(&saddr, 0, sizeof(saddr));
   saddr.sin_family = AF_INET;                // IPv4
   saddr.sin_addr.s_addr = htonl(INADDR_ANY); // Bind to all available
   // interfaces
   saddr.sin_port = htons(port); // Requested port

   bind(sfd, (struct sockaddr *)&saddr, sizeof(saddr));

   while (handle_all(epfd) == rv0) {
   }
   for (i = 0; i < ep_cnt; i++) {
      del_e(epfd, ep_set[i]);
   }
   close(epfd);
   return 0;
}
