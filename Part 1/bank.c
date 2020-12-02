#include <semaphore.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

int main(int argc, char **argv)
{
  int fd, i,nloop=10,zero=0,*BankAccount;
  int time = random() % 10;
  sem_t *mutex;

  fd = open("log.txt",O_RDWR|O_CREAT,S_IRWXU);
  write(fd,&zero,sizeof(int));
  BankAccount = mmap(NULL,sizeof(int),PROT_READ |PROT_WRITE,MAP_SHARED,fd,0);
  close(fd);

 if ((mutex = sem_open("examplesemaphore", O_CREAT, 0644, 1)) == SEM_FAILED) {
    perror("semaphore initilization");
    exit(1);
  }
 
  if (fork() == 0) { 
    for (i = 0; i < nloop; i++) {
      sleep(time);
      printf("Poor Student: Attempting to Check Balance\n");
      sem_wait(mutex);
      int num = random();
      int localBalance = *BankAccount;
      if (num % 2 != 0){
        printf("Poor Student: Last Checking Balance = $%d\n", localBalance);
      }
      else {
        int need = random() % 50;
        printf("Poor Student needs $%d\n", need);
        if (need <= localBalance){
          localBalance -= need;
          printf("Poor Student: Withdraws $%d / Balance = $%d\n", need, localBalance);
        }
        else{
          printf("Poor Student: Not Enough Cash ($%d)\n", localBalance);
        }
        *BankAccount = localBalance;
      }
      sem_post(mutex);
      sleep(1);
    }
    exit(0);
  }

  for (i = 0; i < nloop; i++) {
    sleep(time);
    printf("Dear Old Dad: Attempting to Check Balance\n");
    sem_wait(mutex);
    int num = random();
    int localBalance = *BankAccount;
    if (num % 2 != 0) {
      printf("Dear Old Dad: Last Checking Balance = %d\n", localBalance);
    }
    else {
      if (localBalance < 100) {
        int amount = random()%100;
        if (amount % 2 == 0){
          localBalance += amount;
          printf("Dear old Dad: Deposits $%d / Balance = $%d\n", amount, localBalance);
          *BankAccount = localBalance;
        }
        else{printf("Dear old Dad: Doesn't have any money to give\n");}
      }
      else {printf("Dear Old Dad: Thinks Student has enough cash ($%d)\n", localBalance);}
    }
    sem_post(mutex);
    sleep(1);
  }
  exit(0);
}
