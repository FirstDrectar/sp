// Частина 2: Функції мультиплексованого вводу-виводу. Скласти програму, яка:
// 1. Отримує аргументом командної строки довільний строковий ідентифікатор.
// 2. Налаштовує системний виклик select для очікування читання у вхідному потоці (
// файловий дескріптор STDIN_FILENO ) з таймаутом 5 секунд.
// 3. При отриманні можливості читання, прочитати з потоку буфер довжиною не більше
// 1024 байта та вивести його у вихідний потік з поміткою у вигляді ідентифікатора п.1.
// 4. При спливанні таймауту, вивести у потік помилок повідомлення про це з поміткою у
// вигляді ідентифікатора п.1 та знову налаштувати системний виклик select (п.2).
// 5. Протестувати роботу програми отримуючи через вхідний потік результати вводу з
// клавіатури.

#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>

#define CHUNK 1024

int input_timeout(int filedes, unsigned int seconds)
{
  fd_set set;
  struct timeval timeout;

  /* Initialize the file descriptor set. */
  FD_ZERO(&set);
  FD_SET(filedes, &set);

  /* Initialize the timeout data structure. */
  timeout.tv_sec = seconds;
  timeout.tv_usec = 0;

  /* select returns 0 if timeout, 1 if input available, -1 if error. */
  return select(FD_SETSIZE,
                &set, NULL, NULL,
                &timeout);
}

int main(int argc, char **argv)
{
  if (argc != 2)
  {
    printf("Usage: mux ident");
    return 1;
  }

  char *ident = argv[1];
  int status = input_timeout(STDIN_FILENO, 5);

  while (status == 0)
  {
    fprintf(stderr, "%s: no input, retrying.\n", ident);
    status = input_timeout(STDIN_FILENO, 5);
  }

  if (status == -1)
  {
    perror("Error occured");
  }

  char buf[CHUNK];

  read(STDIN_FILENO, buf, CHUNK);
  fprintf(stdout, "%s: %s", ident, buf);

  return 0;
}
