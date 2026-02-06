#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int n;
int *fib_sequ;
int *indices;
int count;

void *generate_fibonacci(void *arg) {
    fib_sequ = (int *)malloc((n+1) * sizeof(int));
    if (n >= 0) {
    fib_sequ[0] = 0;
    if (n >= 1) fib_sequ[1] = 1;
    }
    for (int i = 2; i <= n; i++) {
        fib_sequ[i] = fib_sequ[i-1] + fib_sequ[i-2];
    }
    pthread_exit(NULL);
}

void *fibonacci(void *arg) {
    for (int i = 0; i < count; i++) {
        int index = indices[i];
        if (index >= 0 && index <= n) {
            printf("result of search #%d = %d\n", i+1, fib_sequ[index]);
        } else {
            printf("result of search #%d = -1\n", i+1);
        }
    }
    pthread_exit(NULL);
}

int main() {
    pthread_t fib_thread, search_thread;
    
    printf("Enter the term of fibonacci sequence: ");
    scanf("%d", &n);
    
    pthread_create(&fib_thread, NULL, generate_fibonacci, NULL);
    pthread_join(fib_thread, NULL);
    

    for (int i = 0; i <= n; i++) {
        printf("a[%d] = %d\n", i, fib_sequ[i]);
    }
    
    printf("How many numbers you are willing to search?: ");
    scanf("%d", &count);
    
    indices = (int *)malloc(count * sizeof(int));
    for (int i = 0; i < count; i++) {
        printf("Enter search %d: ", i+1);
        scanf("%d", &indices[i]);
    }
    
    pthread_create(&search_thread, NULL, fibonacci, NULL);
    pthread_join(search_thread, NULL);
    
    free(fib_sequ);
    free(indices);
    
    return 0;
}
