 /*
* File connection.h
*/

#define SOCKET_NAME "/tmp/9Lq7BNBnBycd6nxy.socket"
#define BUFFER_SIZE 12

/*
* File server.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include "connection.h"

int main(int argc, char *argv[])
{
   struct sockaddr_un name;
   int down_flag = 0;
   int ret;
   int connection_socket;
   int data_socket;
   int result;
   char buffer[BUFFER_SIZE];

   /* Create local socket. */

   connection_socket = socket(AF_UNIX, SOCK_DGRAM, 0);
   if (connection_socket == -1) {
       perror("socket");
       exit(EXIT_FAILURE);
   }

   /*
    * For portability clear the whole structure, since some
    * implementations have additional (nonstandard) fields in
    * the structure.
    */

   memset(&name, 0, sizeof(name));

   /* Bind socket to socket name. */

   name.sun_family = AF_UNIX;
   strncpy(name.sun_path, SOCKET_NAME, sizeof(name.sun_path) - 1);

   ret = bind(connection_socket, (const struct sockaddr *) &name,
	      sizeof(name));
   if (ret == -1) {
       perror("bind");
       exit(EXIT_FAILURE);
   }

   /* This is the main loop for handling connections. */
   /* Wait for incoming connection. */
   result = 0;
   for (;;) {

   /* Wait for next data packet. */

   ret = read(data_socket, buffer, sizeof(buffer));
   if (ret == -1) {
       perror("read");
       exit(EXIT_FAILURE);
   }

   /* Ensure buffer is 0-terminated. */

   buffer[sizeof(buffer) - 1] = 0;

   /* Handle commands. */

   if (!strncmp(buffer, "DOWN", sizeof(buffer))) {
       down_flag = 1;
       break;
   }

   if (!strncmp(buffer, "END", sizeof(buffer))) {
       break;
   }

   /* Add received summand. */

   result += atoi(buffer);
   }

   /* Send result. */

   sprintf(buffer, "%d", result);
   ret = write(data_socket, buffer, sizeof(buffer));
   if (ret == -1) {
   perror("write");
   exit(EXIT_FAILURE);
   }

   /* Close socket. */

   close(data_socket);

   close(connection_socket);

   /* Unlink the socket. */

   unlink(SOCKET_NAME);

   exit(EXIT_SUCCESS);
}
