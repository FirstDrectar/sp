// Частина 1. Обробка сигналів. Скласти програму, яка:
// 1. Описує глобальний дескриптор файла логу.
// 2. Описує функцію-обробник сигналів, прототипу
// void signal_handler( int signo, siginfo_t *si, void * ucontext );
// 3. У функції-обробнику виконати запис у файлі логу з докладним розкриттям структури
// siginfo_t, яка подана на вхід.
// 4. Відкриває файл логу на запис.
// 5. Відмічає в ньому факт власного запуску та свій pid.
// 6. Описує структуру sigaction, у якій вказує на функцію обробник.
// 7. Реєструє обробник для сигналу SIGHUP із збереженням попереднього обробника.
// 8. Переходить до нескінченного циклу із засинанням на кілька секунд та відмітками у
// файлі логу.
// 9. Протестувати отриману програму, посилаючи до неї сигнали утілітою kill, та
// спостерігаючи результат у файлі логу.
// 10. Пояснити отримані результати

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <strings.h>

int log_fd;

void signal_handler(int signo, siginfo_t *si, void *ucontext);

int main()
{
  log_fd = open("log.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);

  char msg[64];
  int len = sprintf(msg, "Program started with PID: %d\n", getpid());
  write(log_fd, msg, len);

  struct sigaction new_handler, old_handler;

  new_handler.sa_sigaction = &signal_handler;
  new_handler.sa_flags = SA_SIGINFO;

  if (sigaction(SIGHUP, &new_handler, &old_handler) < 0)
  {
    perror("Could not register handler");
    return 1;
  }

  len = sprintf(msg, "Sleep for 10s.\n");
  for (;;)
  {
    write(log_fd, msg, len);
    sleep(10);
  }

  return 0;
}

// typedef struct {
// 	int si_signo;
// 	int si_code;
// 	union sigval si_value;
// 	int si_errno;
// 	pid_t si_pid;
// 	uid_t si_uid;
// 	void *si_addr;
// 	int si_status;
// 	int si_band;
// } siginfo_t;

void signal_handler(int signo, siginfo_t *si, void *ucontext)
{
  char buf[512];

  int len = sprintf(buf, "signo: %d \n\
code: %d\n\
errno: %d\n\
pid: %lu\n\
uid: %lu\n\
addr: %d\n\
status: %d\n\n",
                    si->si_signo, si->si_code, si->si_errno, si->si_pid, si->si_uid, si->si_addr, si->si_status);

  write(log_fd, buf, len);
}
