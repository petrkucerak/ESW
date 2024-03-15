#include <arpa/inet.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

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

int handle_server_connection(ep_entry_t *e, ep_data_t *ep)
{
   // accetp connection
   int cfd = accept(e->fd, NULL, NULL);
   if (cfd == -1) {
      perror("handle_server_connection");
      return -1;
   }
   // printf("handle_server_connection %d\n", e->fd);
   fcntl(cfd, F_SETFL, O_NONBLOCK);
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

int handle_server_communication(ep_entry_t *e)
{
   char tmp_buff[BUF_SIZE];
   memset(tmp_buff, 0, BUF_SIZE); // null the buffer
   int tmp_count = read(e->fd, tmp_buff, BUF_SIZE);
   if (tmp_count == -1) {
      perror("handle_server_communication read");
      return -1;
   }
   if (tmp_count != 0) {
      e->count += tmp_count;
      // if string contains end of line \n
      for (uint16_t i = 0; i < BUF_SIZE; ++i) {
         if (tmp_buff[i] == '\n') {
            e->count -= 1;
            char ret_count[10];
            sprintf(ret_count, "%u\n", e->count);
            if (write(e->fd, ret_count, strlen(ret_count)) == -1) {
               perror("handle_server_communication write");
               return -1;
            };
            e->count = 0;
            break;
         }
      }
   }
   return 0;
}