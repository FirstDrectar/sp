// Частина 2. Демонізація процесу. Скласти програму, яка:
// 1. Відкриває на запис текстовий файл логу та робить у нього запис про старт програми.
// 2. Виконує демонізацію свого функціоналу за таким алгоритмом:
//  Виконує fork()
//  Для процесу-батька робить запис у лог про породження нащадка та закриває
// себе викликом exit().
//  Для поцесу-нащадка:
//  робить setsid()
//  змінює поточний каталог на “/”
//  закриває усі відкриті батьком дескриптори
//  відкриває “/dev/null” на запис для трьох стандартних потоків
// 3. Відкриває лог та робить у нього запис про параметри демонізованого процесу.
// 4. Виконує витримку часу у нескінченному циклі.
// 5. Пояснити отримані результати

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>

int write_log(int fd, char *msg)
{
  return write(fd, msg, strlen(msg));
}

int main(int argc, char **argv)
{
  int log = creat("log.txt", 0644);
  if (log < 0)
  {
    perror("Cannot open log file");
  }

  write_log(log, "Program started.\n");

  if (fork() == 0)
  {
    int devNull;
    char msg[64];

    setsid();
    // wait(NULL);
    chdir("/");
    close(log);

    devNull = open("/dev/null", O_WRONLY);
    dup2(devNull, 0);
    dup2(devNull, 1);
    dup2(devNull, 2);

    log = open("/home/den/learning/spz/lab2/log.txt", O_WRONLY);
    if (log < 0)
    {
      perror("Cannot open log file");
    }
    lseek(log, 0, SEEK_END);
    sprintf(msg, "pid: %d, gid: %d\n", getpid(), getgid());
    write_log(log, msg);
    close(log);

    for (;;)
    {
      sleep(5);
    }
  }
  else
  {
    write_log(log, "Called fork and exiting.\n");
    _exit(0);
  }

  return 0;
}
