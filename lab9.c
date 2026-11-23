/* 1. What is the address of the server it is trying to connect to (IP address and port number)
 * A: IP address 127.0.0.1 and port 8000
 *#define addr 127.0.0.1, port 8000
 * 2. is it UDP or TCP? how do you know?
 * A: TCP. the socket is created with socket(AF_INET, SOCK_STREAM, 0)
 * sock_stream means reliable stream(TCP)
 * 3. The client is going to send some data to the server. Where does it get this data from? How can
 * you tell in the code?
 * A: from standard input. the loop calls read and then write sends whatever was read to the server
 * 4. How does the client program end?How can you tell that in the code?
 * A: The while loop ends when read() on stdin returns <= 1 byte. After the loop, it checks for
 * read() error, then calls closed to close the socket and exit.
 */
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8000
#define BUF_SIZE 64
#define ADDR "127.0.0.1"

#define handle_error(msg)                                                                          \
  do {                                                                                             \
    perror(msg);                                                                                   \
    exit(EXIT_FAILURE);                                                                            \
  } while (0)

int main() {
  struct sockaddr_in addr;
  int sfd;
  ssize_t num_read;
  char buf[BUF_SIZE];

  sfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sfd == -1) {
    handle_error("socket");
  }

  memset(&addr, 0, sizeof(struct sockaddr_in));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(PORT);
  if (inet_pton(AF_INET, ADDR, &addr.sin_addr) <= 0) {
    handle_error("inet_pton");
  }

  int res = connect(sfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
  if (res == -1) {
    handle_error("connect");
  }

  while ((num_read = read(STDIN_FILENO, buf, BUF_SIZE)) > 1) {
    if (write(sfd, buf, num_read) != num_read) {
      handle_error("write");
    }
    printf("Just sent %zd bytes.\n", num_read);
  }

  if (num_read == -1) {
    handle_error("read");
  }

  close(sfd);
  exit(EXIT_SUCCESS);
}
#include <arpa/inet.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUF_SIZE 64
#define PORT 8000
#define LISTEN_BACKLOG 32

#define handle_error(msg)                                                                          \
  do {                                                                                             \
    perror(msg);                                                                                   \
    exit(EXIT_FAILURE);                                                                            \
  } while (0)

// Shared counters for: total # messages, and counter of clients (used for
// assigning client IDs)
int total_message_count = 0;
int client_id_counter = 1;

// Mutexs to protect above global state.
pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t client_id_mutex = PTHREAD_MUTEX_INITIALIZER;

struct client_info {
  int cfd;
  int client_id;
};

void *handle_client(void *arg) {
  struct client_info *client = (struct client_info *)arg;

  // TODO: print the message received from client
  int cfd = client->cfd;
  int client_id = client->client_id;
  free(client);
  char buf[BUF_SIZE];
  ssize_t num_read;

  while ((num_read = read(cfd, buf, BUF_SIZE - 1)) > 0) {
    buf[num_read] = '\0';
    char *save_ptr = NULL;
    for (char *line = strtok_r(buf, "\r\n", &save_ptr); line != NULL;
         line = strtok_r(NULL, "\r\n", &save_ptr)) {
      pthread_mutex_lock(&count_mutex);
      total_message_count++;
      printf("Msg #%4d; Client ID %d: %s\n", total_message_count, client_id, line);
      fflush(stdout);
      pthread_mutex_unlock(&count_mutex);
    }
  }

  // TODO: increase total_message_count per message
  total_message_count++;
  return NULL;
}

int main() {
  struct sockaddr_in addr;
  int sfd;

  sfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sfd == -1) {
    handle_error("socket");
  }

  memset(&addr, 0, sizeof(struct sockaddr_in));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(PORT);
  addr.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(sfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1) {
    handle_error("bind");
  }

  if (listen(sfd, LISTEN_BACKLOG) == -1) {
    handle_error("listen");
  }

  for (;;) {
    // TODO: create a new thread when a new connection is encountered
    // TODO: call handle_client() when launching a new thread, and provide
    // client_info
    int cfd = accept(sfd, NULL, NULL);
    if (cfd == -1) {
      perror("accept");
      continue;
    }

    struct client_info *client = malloc(sizeof(struct client_info));
    if (client == NULL) {
      perror("malloc");
      close(cfd);
      continue;
    }

    // assign the client ID
    pthread_mutex_lock(&client_id_mutex);
    client->client_id = client_id_counter++;
    pthread_mutex_unlock(&client_id_mutex);
    client->cfd = cfd;

    printf("New client created! ID %d on socket FD %d\n", client->client_id, client->cfd);
    fflush(stdout);

    pthread_t thread;
    int err = pthread_create(&thread, NULL, handle_client, client);
    if (err != 0) {
      fprintf(stderr, "pthread create: %s\n", strerror(err));
      close(cfd);
      free(client);
      continue;
    }

    pthread_detach(thread);
  }

  if (close(sfd) == -1) {
    handle_error("close");
  }

  return 0;
}
