// Частина 2. Побудова мережевого клієнта. Скласти програму, яка:
// 7. Описує структуру struct sockaddr_in з параметрами: формат сокетів - PF_INET, адреса
// - обчислена за викликом htonl(INADDR_LOOPBACK), порт - 3200 + номер варіанта.
// 8. Формує сокет типу SOCK_STREAM формату PF_INET.
// 9. Налаштовує сокет на підключення до сервера за допомогою connect.
// 10. В нескінченному циклі запитує рядки від оператора, передає їх на сервер та друкує
// отримані відповіді.
// 11. Робота закінчується після вводу оператором рядка "close" та отримання на нього
// відповіді від сервера.
// Частина 3. Дослідити роботу мережевого сервера.
// Дослідження провести запустивши сервер та кілька примірників клієнта у різних сесіях.
// Проаналізувати лог сервера та зробити висновки щодо моменту запуску та закриття
// процесів обробки з'єднань.

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#define PORT 3221

int main()
{
  int sock, read_len;
  struct sockaddr_in addr;
  char buffer[1024];

  sock = socket(PF_INET, SOCK_STREAM, 0);
  if (sock < 0)
  {
    perror("socket");
    exit(1);
  }

  addr.sin_family = PF_INET;
  addr.sin_port = htons(PORT);
  addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

  if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
  {
    printf("\nConnection Failed \n");
    return -1;
  }

  for (;;)
  {
    fgets(buffer, 1024, stdin);
    if (buffer[strlen(buffer) - 1] == '\n')
    {
      buffer[strlen(buffer) - 1] = 0;
    }

    printf("Sending |%s|\n", buffer);

    send(sock, buffer, strlen(buffer), 0);

    if (strcmp("close", buffer) == 0)
    {
      break;
    }

    read_len = read(sock, buffer, 1024);
    buffer[read_len] = '\0';
    printf("Response: %s\n", buffer);
  }

  close(sock);

  return 0;
}