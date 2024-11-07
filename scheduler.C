/*
 File: scheduler.C
 
 Author:
 Date  :
 
 */

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/



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

struct Node {
  Thread* thread;
  Node* next;

  Node(Thread* t) : thread(t), next(nullptr) {}
};


LinkedList::LinkedList() : head(nullptr), tail(nullptr) {}

// Add a thread to the end of the list
void LinkedList::add(Thread* t) {
    Node* new_node = new Node(t);
    if (!head) {
        head = tail = new_node;
    } else {
        tail->next = new_node;
        tail = new_node;
    }
}

// Remove a specific thread from the list
bool LinkedList::remove(Thread* t) {
  Node* prev = nullptr;
  Node* current = head;

  while (current) {
    if (current->thread == t) {
      if (prev) {
        prev->next = current->next;
      } else {
        head = current->next;
      }

      if (current == tail) {
        tail = prev;
      }

      delete current;
      return true;
    }
    prev = current;
    current = current->next;
  }
  return false; // Thread not found
}

// Pop the first thread from the list and return it
Thread* LinkedList::pop_front() {
  if (!head) return nullptr;
  
  Node* node_to_remove = head;
  Thread* t = node_to_remove->thread;

  head = head->next;
  if (!head) {
    tail = nullptr;
  }

  delete node_to_remove;
  return t;
}

// Check if the list is empty
bool LinkedList::is_empty() const {
return head == nullptr;
}

// Destructor to clean up all nodes
LinkedList::~LinkedList() {
while (head) {
  Node* next = head->next;
  delete head;
  head = next;
}
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

Scheduler* Scheduler::current_scheduler = nullptr;  // Definition and initialization

Scheduler::Scheduler() : current_thread(nullptr) {
  //current thread is nullptr for now
  //may need to figure out an idle thread later
  current_scheduler = this;
  Console::puts("Constructed Scheduler.\n");
}

void Scheduler::yield() {
  // Add the currently running thread to the end of the ready queue
  if (current_thread) {
    ready_queue.add(current_thread);
  }

  // Select the next thread from the front of the queue
  if (!ready_queue.is_empty()) {
    current_thread = ready_queue.pop_front();
    Thread::dispatch_to(current_thread);  // Context switch to the next thread
  }
  else{
    Console::puts("ME: no threads in queue to yield to\n");
  }

}

void Scheduler::resume(Thread * _thread) {
  ready_queue.add(_thread);
}

void Scheduler::add(Thread * _thread) {
  resume(_thread);
}

void Scheduler::terminate(Thread * _thread) {  
  // Lock the queue to prevent concurrent access
  //std::lock_guard<std::mutex> lock(queue_mutex);

  // Remove the specified thread from the ready queue
  ready_queue.remove(_thread);

  // If the thread being terminated is the currently running thread
  if (_thread == current_thread) {
    // Yield the CPU to ensure this thread finishes gracefully
    yield();
  }

}

