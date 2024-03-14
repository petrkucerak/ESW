#include <arpa/inet.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "handle.h"

ep_entry_t *add_server(int epfd, uint16_t port)
{

   int sfd = socket(AF_INET, SOCK_STREAM, 0);
   struct sockaddr_in saddr;

   memset(&saddr, 0, sizeof(saddr));
   saddr.sin_family = AF_INET;                // IPv4
   saddr.sin_addr.s_addr = htonl(INADDR_ANY); // Bind to all available
   // interfaces
   saddr.sin_port = htons(port); // Requested port

   if (bind(sfd, (struct sockaddr *)&saddr, sizeof(saddr)) == -1) {
      perror("add_server bind");
      return NULL;
   }

   int flags = fcntl(sfd, F_GETFL, 0);
   if (flags == -1) {
      perror("add_server F_GETFL");
      return NULL;
   }
   flags |= O_NONBLOCK;
   if (fcntl(sfd, F_SETFL, flags) == -1) {
      perror("add_server F_SETFL");
      return NULL;
   }

   listen(sfd, SOMAXCONN);

   ep_entry_t *e;
   e = new_e();
   e->type = ESW_EPOLL_SERVER_INIT;
   e->fd = sfd;

   add_e(epfd, e);
   return e;
}

int handle_new_connection(ep_entry_t *e, ep_data_t *ep)
{
   // accetp connection
   int cfd = accept(e->fd, NULL, NULL);
   if (cfd == -1) {
      perror("handle_new_connection");
      return -1;
   }
   printf("handle_new_connection %d\n", e->fd);
   ep->ep_set[ep->ep_cnt++] = new_communication(cfd, ep);
   return 0;
}

ep_entry_t *new_communication(int cfd, ep_data_t *ep)
{
   ep_entry_t *e;
   e = new_e();
   e->type = ESW_EPOLL_SERVER_COMMUNICATION;
   e->fd = cfd;
   e->count = 0;
   add_e(ep->epfd, e);
   return e;
}