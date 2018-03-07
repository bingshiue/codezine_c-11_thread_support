/* ご参考: 
 *   pthread による concurrent_box<int> の実装
 *   ※ using Pthreads4W (POSIX Threads for Windows 2.10.0-rc) 
 *              https://sourceforge.net/projects/pthreads4w/
 *      実行に先立ち、pthreads4w/bin/x64/pthreadVC2.dll を x64/Release|Debug にcopyすべし。
 */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define BOX_TYPE int

#pragma region concurrent_box
typedef struct {
  pthread_mutex_t mtx;
  pthread_cond_t  can_get;
  pthread_cond_t  can_set;
  bool            empty;
  BOX_TYPE        data;
} box_t;

/* boxを初期化する*/
void box_init(box_t* box) {
  pthread_mutex_init(&box->mtx, NULL);
  pthread_cond_init(&box->can_get, NULL);
  pthread_cond_init(&box->can_set, NULL);
  box->empty = true;
}

/* boxを廃棄する*/
void box_destroy(box_t* box) {
  pthread_mutex_destroy(&box->mtx);
  pthread_cond_destroy(&box->can_get);
  pthread_cond_destroy(&box->can_set);
}

/* boxが空ならtrue */
bool box_empty(const box_t* box) {
  return box->empty;
}

/* boxが空でないなら true */
bool box_occupied(const box_t* box) {
  return !box->empty;
}

/* boxに値をセットする*/
void box_set(box_t* box, BOX_TYPE value) {
  /* mutexで保護する*/
  pthread_mutex_lock(&box->mtx);

  /* boxが空でない間、'set可能'を待つ*/
  while ( box_occupied(box) ) {
    pthread_cond_wait(&box->can_set, &box->mtx);
  }
  /* 値を書き込み、'get可能'を通知する*/
  box->data = value;
  box->empty = false;
  pthread_cond_signal(&box->can_get);

  pthread_mutex_unlock(&box->mtx);
}

BOX_TYPE box_get(box_t* box) {
  /* mutexで保護する*/
  pthread_mutex_lock(&box->mtx);

  /* boxが空である間、'get可能'を待つ*/
  while ( box_empty(box) ) {
    pthread_cond_wait(&box->can_get, &box->mtx);
  }
  /* 値を読み出し、'set可能'を通知する*/
  BOX_TYPE value = box->data;
  box->empty = true;
  pthread_cond_signal(&box->can_set);

  pthread_mutex_unlock(&box->mtx);
  return value;
}
#pragma endregion

void producer(box_t* box) {
  int sum = 0;
  puts("producer start");
  const int N = 10000;
  for ( int i = 0; i < N; ++i ) {
    int value = rand()%10;
    box_set(box, value);
    sum += value;
  }
  box_set(box, -1);
  printf("%d items produced. sum = %d\n", N, sum);
}

void consumer(box_t* box) {
  int sum = 0;
  puts("consumer start");
  int count = 0;
  while ( true ) {
    int value = box_get(box);
    if ( value < 0 ) break;
    sum += value;
    ++count;
  }
  printf("%d items consumed. sum = %d\n", count, sum);
}

typedef struct {
  void (*fun)(box_t*);
  box_t* box;
} argument_t;

void* thread_entry(void* arg) {
  argument_t argument = *(argument_t*)arg;
  argument.fun(argument.box);
  return NULL;
}  

int main() {  
  box_t box;
  box_init(&box);

  argument_t arg0;
  arg0.fun = consumer;
  arg0.box = &box;
  pthread_t thr0;
  pthread_create(&thr0, NULL, thread_entry, &arg0);

  argument_t arg1;
  arg1.fun = producer;
  arg1.box = &box;
  pthread_t thr1;
  pthread_create(&thr1, NULL, thread_entry, &arg1);

  pthread_join(thr0, NULL);
  pthread_join(thr1, NULL);
  box_destroy(&box);

  return 0;
}
