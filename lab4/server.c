// Частина 1. Побудова мережевого сервера. Скласти програму, яка:
// 1. Виконує демонізацію, подальший функціонал стосується демона.
// 2. Описує глобальний дескриптор файла логу.
// 3. Описує структуру struct sockaddr_in з параметрами: формат сокетів - PF_INET, адреса
// - будь яка, порт - 3200 + номер варіанта.
// 4. Формує сокет типу SOCK_STREAM формату PF_INET.
// 5. Налаштовує сокет на очікування запитів за допомогою bind.
// 6. Запускає нескінченний цикл обробки запитів від клієнтів.
// 7. На кожний запит виконує fork для породження процесу обробки.
// 8. Батьківський процес закриває сокет.
// 9. Процес обробки в нескінченному циклі отримує від клієнта строки за допомогою recv,
// додає до них свій префікс у вигляді поточного часу та власного pid, та повертає
// клієнту за допомогою send.
// 10. Процес обробки завершує обробку даних від клієнта отримавши строку "close".
// 1. Кожна дія сервера супроводжується відміткою у логу.

#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stddef.h>

#define PORT 3221

int log_fd;

char *concat(char *pre, char *post)
{
  char *buf;
  int len;

  len = strlen(pre) + strlen(post) + 1;
  buf = (char *)malloc(len * sizeof(char));

  strcpy(buf, pre);
  strcat(buf, post);

  return buf;
}

int log_msg(char *msg)
{
  msg = concat(msg, "\n");
  return write(log_fd, msg, strlen(msg));
}

int run_daemon();
int run_server();
int bind_listener();
int process_connection(int sock);

int main(int argc, char **argv)
{
  int pid;

  pid = fork();
  if (pid < 0)
  {
    perror("Could not fork");
    return 1;
  }

  if (pid == 0)
  {
    return run_daemon();
  }
  else
  {
    printf("Running with PID %d\n", pid);
    _exit(0);
  }

  return 0;
}

int run_daemon()
{
  int status;
  int sid;
  int devNull;

  sid = setsid();
  if (sid < 0)
  {
    perror("Could not start session");
    return 1;
  }

  chdir("/");

  log_fd = open("/home/den/learning/spz/lab4/log.txt", O_CREAT | O_TRUNC | O_WRONLY, 0644);
  if (log_fd < 0)
  {
    perror("Could not open log file");
    return 1;
  }

  devNull = open("/dev/null", O_WRONLY);
  dup2(devNull, 0);
  dup2(devNull, 1);
  dup2(devNull, 2);

  status = run_server();

  close(log_fd);

  return status;
}

int run_server()
{
  int sock, listener;
  int pid;

  listener = bind_listener();

  log_msg("Binded successfully.");

  listen(listener, 1);
  for (;;)
  {
    sock = accept(listener, NULL, NULL);
    log_msg("Accepted new connection.");

    if (sock < 0)
    {
      log_msg("Error in accept: ");
      log_msg(strerror(sock));
      continue;
    }

    pid = fork();
    if (pid < 0)
    {
      log_msg("Error in fork: ");
      log_msg(strerror(pid));
      continue;
    }

    if (pid > 0)
    {
      close(sock);
      waitpid(pid, NULL, WNOHANG); // did not help :((
    }
    else
    {
      close(listener);
      process_connection(sock);
      close(sock);
      close(log_fd);
      exit(0);
    }
  }

  close(listener);

  return -1;
}

// 9. Процес обробки в нескінченному циклі отримує від клієнта строки за допомогою recv,
// додає до них свій префікс у вигляді поточного часу та власного pid, та повертає
// клієнту за допомогою send.
// 10. Процес обробки завершує обробку даних від клієнта отримавши строку "close".
char *info()
{
  char *buf;

  buf = (char *)malloc(sizeof(char) * 64);

  sprintf(buf, "p: %d, ts: %d ", getpid(), time(NULL));

  return buf;
}

int process_connection(int sock)
{
  int bytes_read;
  char buf[1024];
  char *msg;

  for (;;)
  {
    bytes_read = recv(sock, buf, 1024, 0);
    buf[bytes_read] = '\0';

    if (strcmp(buf, "close") == 0)
    {
      log_msg(concat(info(), "got close message"));
      return 0;
    }

    msg = concat(info(), buf);

    log_msg(msg);

    send(sock, msg, strlen(msg), 0);
  }

  return 0;
}

int bind_listener()
{
  int listener;
  struct sockaddr_in addr;

  listener = socket(PF_INET, SOCK_STREAM, 0);
  if (listener < 0)
  {
    perror("Create listener");
    return -1;
  }

  addr.sin_family = PF_INET;
  addr.sin_port = htons(PORT);
  addr.sin_addr.s_addr = INADDR_ANY;

  if (bind(listener, (struct sockaddr *)&addr, sizeof(addr)) < 0)
  {
    perror("Could not bind");
    return -1;
  }

  return listener;
}
