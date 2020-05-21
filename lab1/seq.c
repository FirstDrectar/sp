// Частина 1: Функції послідовного вводу-виводу. Скласти програму, яка:
// 1. Отримує аргументами командної строки два імені файлів
// 2. Перший файл відкриває для читання, другий до перезапису, або створює, якщо його
// не існує (для створення використати атрибути 0644)
// 3. Послідовно читає дані з першого файлу буферами фіксованого об’єму (наприклад
// 512 байт) та записує у другий.
// 4. Перед записом із вмістом буферу проводить таке перетворення – усі рядкові літери
// латинського алфавіту перетворює на відповідні прописні. Для цього розрахувати
// відповідний зсув за таблицею ASCII (див. man ascii).
// 5. В процесі перезапису проводить підрахунок перезаписаних байт, та наприкінці
// виводить у вихідний потік сумарний обсяг переписаних даних.
// 97 - 122 - az
// 65 - 90 - AZ

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#define CHUNK 512

void transform(char buf[CHUNK], int size)
{
  for (int i = 0; i < size; i++)
  {
    if (buf[i] > 96 && buf[i] < 123)
    {
      buf[i] = buf[i] - 32;
    }
  }
}

int rewrite(int r, int w)
{
  int bytes, total = 0;
  char buf[CHUNK];

  while ((bytes = read(r, buf, CHUNK)) != 0)
  {
    transform(buf, bytes);
    write(w, buf, bytes);

    total += bytes;
  }

  fsync(w);

  return total;
}

int main(int argc, char **argv)
{
  int read, write, total;

  if (argc != 3)
  {
    printf("Usage: seq file1 file2");
    return 1;
  }

  read = open(argv[1], O_RDONLY);
  if (read == -1)
  {
    perror("Could not open file for read");
    return 1;
  }

  write = open(argv[2], O_WRONLY | O_CREAT, 0644);
  if (write == -1)
  {
    perror("Could not open file for write");
    return 1;
  }

  total = rewrite(read, write);

  printf("Total: %dB.\n", total);

  close(read);
  close(write);

  return 0;
}
