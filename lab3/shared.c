// Частина 2. Розподілена пам`ять. Скласти програму, яка:
// 1. Описує структуру датума, яка містить ціле значення для ідентифікатора процесу, ціле
// значення для мітки часу та строку фіксованої довжини.
// 2. Реєструє обїект розподіленої пам`яті через виклик shm_open
// 3. Приводить його до розміру кратного розміру структури датуму
// 4. Відображае отриманий об`єкт у пам`ять через показчик на структуру датуму та
// виклик mmap
// 5. Переходить до нескінченного циклу у якому:
// a. Запитує строку з клвіатури
// b. Вичитує та презентує вміст структури датуму
// c. Записує у структуру натомість свій ідентификатор процесу, поточний час та
// отриману строку
// 6. Протестувати отриману програму, запустивши два її примірники у різних сесїях.

#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

typedef struct
{
  int pid;
  int ts;
  char data[512];
} datum;

int main()
{
  int fd;
  int res;
  void *addr;
  datum data_write, data_read;

  fd = shm_open("/shm_lab3", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
  if (fd == -1)
  {
    perror("Could not open shm");
    return 1;
  }

  res = ftruncate(fd, sizeof(datum));
  if (res == -1)
  {
    perror("Could not ftruncate");
    return 1;
  }

  // map shared memory to process address space
  addr = mmap(NULL, sizeof(datum), PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
  if (addr == MAP_FAILED)
  {
    perror("Could not mmap");
    return 1;
  }

  sprintf(data_write.data, "Initial data\n");
  data_write.pid = getpid();
  data_write.ts = time(NULL);

  // load data
  memcpy(addr, &data_write, sizeof(datum));

  // run
  while (!feof(stdin))
  {
    printf("Input, my dear: ");
    fgets(data_write.data, 512, stdin);
    memcpy(&data_read, addr, sizeof(datum));
    printf("Prev -> pid: %d, ts: %d,  data: %s", data_read.pid, data_read.ts, data_read.data);
    memcpy(addr, &data_write, sizeof(datum));
  }

  return 0;
}
