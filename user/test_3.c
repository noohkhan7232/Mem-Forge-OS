#include "types.h"
#include "stat.h"
#include "user.h"

// Test that pages are copied on write and that the parent values are not affected by the child

int main(void) {
    int sz = 1000;
	int* arr = (int*)malloc(sz * sizeof(int));
    int pid = getpid();

    int init_free = getFreePagesCount();
    
    for(int i = 0; i < sz; i++){
        arr[i] = i * pid;
    }

    int pid2 = fork();

    if(pid2 == 0) {
        int mypid = getpid();
        printf(1, "child %d\n", mypid);
        int child_free_cnt1 = getFreePagesCount();
        for(int i = 0; i < sz; i++){
            arr[i] = i * mypid;
        }
        int* arr2 = (int*)malloc(sz * sizeof(int));
        for(int i = 0; i < sz; i++){
            arr2[i] = i * mypid;
        }
        int* arr3 = (int*)malloc(sz * sizeof(int));
        for(int i = 0; i < sz; i++){
            arr3[i] = i * mypid;
        }
        int* arr4 = (int*)malloc(sz * sizeof(int));
        for(int i = 0; i < sz; i++){
            arr4[i] = i * mypid;
        }
        free(arr);
        free(arr2);
        free(arr3);
        int child_free_cnt2 = getFreePagesCount();
        printf(1, "Free pages changed by child. Before: %d, after: %d\n", child_free_cnt1, child_free_cnt2);
        exit();
    } else {
        wait();
        printf(1, "parent %d\n", pid);

        // Check that values in parent are not changed
        for(int i = 0; i < sz; i++){
            if(arr[i] != i * pid){
                printf(1, "Parent changed\n");
                printf(1, "XV6_COW\t FAILED\n");
                exit();
            }
        }

        int final_free = getFreePagesCount();
        
        // After child exits, parent should have same number of free pages
        // **but for the paged allocated during fork()** The original test 
        // is mistaken. -- note by Alicia
        if(final_free != init_free){
            printf(1, "Parent have fewer freepages. Before: %d, after: %d\n", init_free, final_free);
            printf(1, "XV6_COW\t FAILED\n");
            exit();
        }

        printf(1, "XV6_COW\t SUCCESS\n");
    }
 	exit();
}