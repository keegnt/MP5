/*
 File: scheduler.C
 
 Author:
 Date  :
 
 */

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "scheduler.H"
#include "thread.H"
#include "console.H"
#include "utils.H"
#include "assert.H"

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*/

SimpleQueue::SimpleQueue() : front(0), rear(0) {}

bool SimpleQueue::is_empty() {
  return front == rear;
}

bool SimpleQueue::is_full() {
  return (rear + 1) % MAX_SIZE == front;
}

void SimpleQueue::push(Thread* thread) {
  if (!is_full()) {
    queue[rear] = thread;
    rear = (rear + 1) % MAX_SIZE;
  }
  // If full, you may want to add error handling here (optional)
}

Thread* SimpleQueue::pop() {
  if (!is_empty()) {
    Thread* thread = queue[front];
    front = (front + 1) % MAX_SIZE;
    return thread;
  }
  return nullptr;  // Return nullptr if queue is empty
}

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* METHODS FOR CLASS   S c h e d u l e r  */
/*--------------------------------------------------------------------------*/

Scheduler::Scheduler() : current_thread(nullptr) {
  //current thread is nullptr for now
  //may need to figure out an idle thread later

  Console::puts("Constructed Scheduler.\n");
}

void Scheduler::yield() {
  // Add the currently running thread to the end of the ready queue
  if (current_thread) {
    ready_queue.push(current_thread);
  }

  // Select the next thread from the front of the queue
  if (!ready_queue.is_empty()) {
    current_thread = ready_queue.pop();
    Thread::dispatch_to(current_thread);  // Context switch to the next thread
  }
  else{
    Console::puts("ME: no threads in queue to yield to\n");
  }

}

void Scheduler::resume(Thread * _thread) {
  ready_queue.push(_thread);
}

void Scheduler::add(Thread * _thread) {
  resume(_thread);
}

void Scheduler::terminate(Thread * _thread) {
  if (_thread == current_thread) {
    // Yield the CPU to ensure this thread finishes gracefully
    yield();
  }
  
  // Lock the queue to prevent concurrent access
  //std::lock_guard<std::mutex> lock(queue_mutex);

  // Remove the thread from the ready queue 
  SimpleQueue temp_queue;

  // Transfer threads from ready_queue to temp_queue, skipping _thread
  
  // **Possible concurrency issues we may need a lock or mutex here**
  while (!ready_queue.is_empty()) {
    Thread* front_thread = ready_queue.pop();
    ready_queue.pop();
    if (front_thread != _thread) {
      temp_queue.push(front_thread);
    }
  }

  // Restore remaining threads back to the original queue
  ready_queue = temp_queue;
  

}

