   
#include<stdio.h>
#include<stdlib.h>

int mutex = 1, full = 0, empty = 3, x = 0;

int wait(int s){
 return(--s);
 }
 
 
 int signal(int s){
  return(++s);
  }
  
  /* shared buffer is a memory space shared between multiple processes or threads to store and
 retrieve data. It is commonly used in Producer-Consumer problems, where:
Producers add (produce) items into the buffer.
 Consumers remove (consume) items from the buffer.*/
    
    
  void producer(){
  
     mutex = wait(mutex);
     empty = wait(empty);
     x++;
   printf("\n Producer has placed  %d item into the buffer\n", x);
     full = signal(full);
     mutex = signal(mutex);
     }
     
   void consumer(){
       
        mutex = wait(mutex);
        full = wait(full);
        printf("\nConsumer has consumed %d item from the buffer\n", x);
        x--;
        empty = signal(empty);
        mutex = signal(mutex);
        
        }
        
        int main(){
        int choice;
        
        while(1){
        
        printf("\n1. producer\n2. Comsumer\n3. Exit");
        printf("\n Enter your choice:\n");
        scanf("%d", &choice);
        
        switch(choice){
        case 1:
          if((mutex == 1) && ( empty != 0)){
          producer();
          }else{
          printf("\nBuffer is full!\n");
          }
          break;
           
           
         case 2:
           if((mutex == 1) && (full != 0)){
            consumer();
           }else{
            printf("\n Buffer is empty!\n");
            }
            break;
            
          case 3:
            exit(0);
            
            default:
            printf("\n Invalid choice!\n");
            }
         }
            return 0;
        }

        /*mutex → Controls access to the shared buffer (initially 1, meaning it's available).
         full → Number of items currently in the buffer (initially 0, meaning it's empty).
         empty → Number of empty slots in the buffer (initially 3, meaning space for 3 items).
         x → Keeps track of the number of produced items.*/
    
