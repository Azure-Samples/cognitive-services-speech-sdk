#ifndef EVENT_QUEUE_H
#define EVENT_QUEUE_H

#include <stddef.h>

/**
 *
 * EVENT QUEUE
 *
 * A queue that stores events to be executed. When `event_queue_do_work` is
 * called, the event queue will fire the events in the queue.
 * 
 * The event queue is designed to allow many processes to be concurrently 
 * executed on a single thread. `event_queue_do_work` is expected to be 
 * called by one thread. It is thread safe to push threads from any thread.
 *
 * TODO: Add registration of event owners, that way owners can die and they won't be called
 */

struct event_queue;
typedef struct event_queue* EventQueue;

/**
 * A function that is called when an event is fired.
 * @param pContext The user context that was pushed with 
 * the event.
 */
typedef void (*EventCallback)(void * pContext);

/**
 * Create an event queue.
 * @param num_events_estimate An estimate of maximum events. 
 * @return Returns an event queue. NULL is returned on failure.
 */
EventQueue event_queue_create(size_t num_events_estimate);

/**
 * Destroy an event queue.
 * @param The event queue to destroy
 */
void event_queue_destroy(EventQueue handle);

/**
 * Push an event onto the queue. This function is thread safe.
 * @param handle The event queue to push the event onto.
 * @param callback The function to call one the event is fired.
 * @param pContext User context to be passed back to the event callback
 * @return 0 is returned on success.
 */
int event_queue_push(EventQueue handle, EventCallback callback, void *pContext);

/**
 * Push an event onto the queue with data. The data is copied onto the queue.
 * The caller does not need to garauntee that the `data` pointer points to valid
 * data when the event gets fired. This function is thread safe.
 * @param handle The event queue to push the event onto.
 * @param callback The function to call one the event is fired.
 * @param data A pointer to data to be copied onto the queue. This data will be 
 *             passed to the callback once the event is fired.
 * @param data_size The size of the data to be copied.
 * @return 0 is returned on success.
 */
int event_queue_push_with_data(EventQueue handle, EventCallback callback, void *data, size_t data_size);

/**
 * Preform work required by the event queue. This will pop elements from the
 * event queue and fire them. 
 * @param handle The event queue.
 * @return Returns 0 if there is no more work to do. Returns non-zero
 *         if there is no more work to do.
 */
int event_queue_do_work(EventQueue handle);

/**
 * Blocks until more work is pushed onto the queue. 
 * @param queue The queue to wait until there is more work pushed.
 * @param timeout The maximum number of miliseconds to wait. A value of 
 *                0 will mean the function will block indefinitely.
 * @return True if more work was pushed. False if the timeout or an
 * error occured.
 */
int event_queue_wait(EventQueue queue, int timeout);

#endif