// Частина 1. Запуск, завершення та параметри процесу. Скласти програму, яка:
// 1. Отримує та друкує інформацію про параметри свого процесу за допомогою системних
// викликів getpid(), getgid(), getsid() тощо.
// 2. Виконує розгалудження процесу за допомогою системного виклику fork().
// 3. Для процесу-батька та процесу-нащадка окремо роздруковує їхні ідентифікатори у
// циклі.
// 4. Виконати очікування процесом-батьком завершення нащадка.
// 5. Повідомляє про завершення процесів — батька та нащадка.
// 6. Пояснити отримані результати

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>

int main(int argc, char **argv)
{
  int pid;

  pid = getpid();
  printf("pid: %d, gid: %d, sid: %d\n", pid, getgid(), getsid(pid));

  if (fork() == 0)
  {
    for (int i = 0; i < 10; i++)
    {
      printf("Hello from kiddo: %d\n", getpid());
    }

    printf("Exiting child\n");
  }
  else
  {
    for (int i = 0; i < 10; i++)
    {
      printf("Hello from parent: %d\n", getpid());
    }

    printf("Awaiting child Process\n");
    wait(NULL);
    printf("Exiting parent\n");
  }

  return 0;
}
