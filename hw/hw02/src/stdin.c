#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <unistd.h>

#include "ds.h"
#include "handle.h"

#define MAX_BUFF 100

ep_entry_t *add_stdin(int epfd)
{
   ep_entry_t *e;
   e = new_e();
   e->type = ESW_EPOLL_STDIN;
   e->fd = STDIN_FILENO;
   add_e(epfd, e);
   return e;
}

int handle_stdin(ep_entry_t *e)
{
   char in[MAX_BUFF];
   memset(in, 0, MAX_BUFF);
   if (read(e->fd, &in, MAX_BUFF) == -1) {
      return -1;
   }
   if (write(STDOUT_FILENO, &in, MAX_BUFF) == -1) {
      return -1;
   }
   return 0;
}
