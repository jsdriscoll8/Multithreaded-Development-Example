#include <stdio.h>
#include <pthread.h>

// Define global thresholds
#define NUMVALS (1024 * 1024)
#define NUMTHREADS 5

// Struct & function declarations
typedef struct {
    int startIndex;
    int endIndex;
    float threshold;
    int count;
} CountInfo;

int prand();
void *findOverThreshold(void *param);

// Global variables
float g_vals[NUMVALS];

// prand function
int prand() {
    static int p = 1;
    const int a = 105491;
    const int b = 733;
    const int c = 6619;
    const int base = 100;
    int val;
    p = p*a + b;
    p = p % c;
    val = p % base;
    return val;
}

// Threshold function
void *findOverThreshold(void *param) {
    CountInfo *info = (CountInfo *) param;

    // Append if corresponding array value is over thread info threshold
    for(int i = info->startIndex; i < info->endIndex; i++) {
        if(g_vals[i] > info->threshold)
            info->count++;
    }

    pthread_exit(NULL);
}

//-----------------------------------------------------------------------------------

int main() {
    // Create array of threads, thread informations
    CountInfo infos[NUMTHREADS];
    pthread_t threads[NUMTHREADS];

    // Define chunk sizes and adjustments if NUMVALS % NUMTHREADS != 0
    int chunkSize = NUMVALS / NUMTHREADS;
    int adjust = NUMVALS % NUMTHREADS;
    int position = 0;

    // Initialize global float array with values
    int i1, i2;
    float f1, f2;
    for(int i = 0; i < NUMVALS; i++) {
        i1 = prand();
        i2 = prand();
        f1 = i1 / 100.0;
        f2 = i2 / 100.0;
        g_vals[i] = f1 / (1.0 + f2);
    }

    // Initialize info structs
    for(int i = 0; i < NUMTHREADS; i++) {
        // Give info values
        infos[i].startIndex = position;
        infos[i].endIndex = (position + chunkSize);
        infos[i].threshold = 0.5;
        infos[i].count = 0;

        // Account for unevenness
        if(adjust > 0) {
            infos[i].endIndex++;
            adjust--;
            position++;
        }

        // Increment the chunk
        position += chunkSize;
    }

    // Initialize threads
    for(int i = 0; i < NUMTHREADS; i++)
        pthread_create(&threads[i], NULL, findOverThreshold, &infos[i]);

    // Wait for child threads
    for(int i = 0; i < NUMTHREADS; i++)
        pthread_join(threads[i], NULL);

    // Get the total over the threshold
    int totalOverThreshold = 0;
    for(int i = 0; i < NUMTHREADS; i++)
        totalOverThreshold += infos[i].count;

    printf("Total over threshold: %d\n\n", totalOverThreshold);
    return 0;
}