/* this header will contain everything related to the workqueues, tasklets and concurrency */

#ifndef ARCH_SET_GS
#define ARCH_SET_GS 0x1001
#endif

enum tasklet_priority {
	TASKLET_PRIO_NORMAL,
	TASKLET_PRIO_HIGH,
};

extern __thread void *percpu_area;
extern __thread struct __task_struct *tls_current;
extern void *const_current_task;
static __thread struct tasklet_struct *current_tasklet = NULL;

/* private struct used for mantaining the state of the running tasklet */
struct tasklet_runtime_state {
	struct tasklet_struct *tasklet;
	atomic_bool queued;
	atomic_int dispatch_refs;
	atomic_int priority;
	pthread_mutex_t wait_lock;
	pthread_cond_t wait_cond;
	struct tasklet_runtime_state *next;
};

static void tasklet_sync_init_once(void){
	int expected = 0;

	if (!atomic_compare_exchange_strong_explicit(&director.tasklet_sync_state, &expected, 1, memory_order_acq_rel, memory_order_acquire)) {
		while (atomic_load_explicit(&director.tasklet_sync_state, memory_order_acquire) != 2){
			sched_yield();
		}
		return;
	}

	pthread_mutex_init(&director.tasklet_lock, NULL);
	pthread_cond_init(&director.tasklet_wake, NULL);
	atomic_store_explicit(&director.tasklet_sync_state, 2, memory_order_release);
}

static inline void tasklet_sync_init(void){
	tasklet_sync_init_once();
}

static inline int tasklet_count_read(struct tasklet_struct *tasklet){
	return __atomic_load_n(&tasklet->count.counter, __ATOMIC_ACQUIRE);
}

static inline void tasklet_count_set(struct tasklet_struct *tasklet, int value){
	__atomic_store_n(&tasklet->count.counter, value, __ATOMIC_RELEASE);
}

static struct tasklet_runtime_state *tasklet_runtime_find_locked(struct tasklet_struct *tasklet){
	struct tasklet_runtime_state *state = NULL;

	for (state = director.tasklet_runtime_states; state != NULL; state = state->next) {
		if (state->tasklet == tasklet){
			return state;
		}
	}
	return NULL;
}

static struct tasklet_runtime_state *tasklet_runtime_get_locked(struct tasklet_struct *tasklet, bool create){
	struct tasklet_runtime_state *state = NULL;

	state = tasklet_runtime_find_locked(tasklet);
	if (state != NULL || create == false){
		return state;
	}

	state = calloc(1, sizeof(*state));
	if (state == NULL){
		return NULL;
	}
	state->tasklet = tasklet;
	atomic_store_explicit(&state->queued, false, memory_order_release);
	atomic_store_explicit(&state->dispatch_refs, 0, memory_order_release);
	atomic_store_explicit(&state->priority, TASKLET_PRIO_NORMAL, memory_order_release);
	pthread_mutex_init(&state->wait_lock, NULL);
	pthread_cond_init(&state->wait_cond, NULL);
	state->next = director.tasklet_runtime_states;
	director.tasklet_runtime_states = state;
	return state;
}

static struct tasklet_runtime_state *tasklet_runtime_get(struct tasklet_struct *tasklet, bool create){
	struct tasklet_runtime_state *state = NULL;

	if (tasklet == NULL){
		return NULL;
	}
	tasklet_sync_init();
	pthread_mutex_lock(&director.tasklet_lock);
	state = tasklet_runtime_get_locked(tasklet, create);
	pthread_mutex_unlock(&director.tasklet_lock);
	return state;
}

static void tasklet_runtime_remove(struct tasklet_struct *tasklet){
	struct tasklet_runtime_state *state = NULL;
	struct tasklet_runtime_state **link = NULL;

	tasklet_sync_init();
	pthread_mutex_lock(&director.tasklet_lock);
	link = &director.tasklet_runtime_states;
	while (*link != NULL) {
		state = *link;
		if (state->tasklet == tasklet) {
			*link = state->next;
			pthread_mutex_unlock(&director.tasklet_lock);
			pthread_mutex_destroy(&state->wait_lock);
			pthread_cond_destroy(&state->wait_cond);
			free(state);
			return;
		}
		link = &state->next;
	}
	pthread_mutex_unlock(&director.tasklet_lock);
}

static inline unsigned long tasklet_state_bit(int bit){
	return 1UL << bit;
}

static inline bool tasklet_test_state(struct tasklet_struct *tasklet, int bit){
	return (__atomic_load_n(&tasklet->state, __ATOMIC_ACQUIRE) & tasklet_state_bit(bit)) != 0;
}

static inline void tasklet_set_state(struct tasklet_struct *tasklet, int bit){
	__atomic_fetch_or(&tasklet->state, tasklet_state_bit(bit), __ATOMIC_ACQ_REL);
}

static inline void tasklet_clear_state(struct tasklet_struct *tasklet, int bit){
	__atomic_fetch_and(&tasklet->state, ~tasklet_state_bit(bit), __ATOMIC_ACQ_REL);
}

static inline bool tasklet_test_and_set_state(struct tasklet_struct *tasklet, int bit){
	return (__atomic_fetch_or(&tasklet->state, tasklet_state_bit(bit), __ATOMIC_ACQ_REL) & tasklet_state_bit(bit)) != 0;
}

static inline bool tasklet_test_and_clear_state(struct tasklet_struct *tasklet, int bit){
	return (__atomic_fetch_and(&tasklet->state, ~tasklet_state_bit(bit), __ATOMIC_ACQ_REL) & tasklet_state_bit(bit)) != 0;
}

static inline void percpu_set_current_task(void);

static inline void setup_percpu(void){
	#ifdef __linux__
	if (percpu_area == NULL) {
		percpu_area = mmap(NULL, 1024 * 1024 * 8, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		if (percpu_area == MAP_FAILED){
			percpu_area = NULL;
		}
	}
	if (percpu_area != NULL){
		syscall(SYS_arch_prctl, ARCH_SET_GS, (unsigned long)percpu_area);
		percpu_set_current_task();
	}
	#else
	/* need to find a fix for Windows too, apparently it doesn't permit to manipulate the GS register safely */
	#endif
}

static inline void percpu_set_current_task(void){
	#ifdef __linux__
	uintptr_t offset = (uintptr_t)&const_current_task;

	if(percpu_area != NULL && tls_current != NULL && offset <= (1024 * 1024 * 8) - sizeof(void *)){
		*(struct __task_struct **)((u8 *)percpu_area + offset) = tls_current;
	}
	#endif
}

/* Release this thread's existing TLS resources, without a thread registry. */
static void cleanup_percpu(void *unused){
	if(tls_current != NULL && tls_current->threadfn == NULL){
		pthread_cond_destroy(&tls_current->cond);
		pthread_mutex_destroy(&tls_current->lock);
		free(tls_current);
	}
	tls_current = NULL;
	#ifdef __linux__
	if(percpu_area != NULL){
		syscall(SYS_arch_prctl, ARCH_SET_GS, 0UL);
		munmap(percpu_area, 1024 * 1024 * 8);
		percpu_area = NULL;
	}
	#endif
}

static void tasklet_head_init(struct tasklet_head *head){
	head->head = NULL;
	head->tail = &head->head;
}

static void tasklet_head_push(struct tasklet_head *head, struct tasklet_struct *tasklet){
	tasklet->next = NULL;
	*head->tail = tasklet;
	head->tail = &tasklet->next;
}

static struct tasklet_struct *tasklet_head_pop(struct tasklet_head *head){
	struct tasklet_struct *tasklet = NULL;

	tasklet = head->head;
	if (tasklet == NULL){
		return NULL;
	}
        head->head = tasklet->next;
	if (!head->head){
		head->tail = &head->head;
	}
	tasklet->next = NULL;
        return tasklet;
}

static bool tasklet_head_remove(struct tasklet_head *head, struct tasklet_struct *target){
	struct tasklet_struct *cur = NULL;
	struct tasklet_struct **link = NULL;

	link = &head->head;
	while (*link) {
		cur = *link;
		if (cur == target) {
			*link = cur->next;
			if (head->tail == &cur->next){
				head->tail = link;
			}
			cur->next = NULL;
			return true;
		}
		link = &cur->next;
	}
	return false;
}

static struct tasklet_struct *pop_tasklet(void){
	struct tasklet_struct *tasklet = NULL;
	struct tasklet_runtime_state *state = NULL;

	tasklet = tasklet_head_pop(&director.tasklet_high_priority);
	if (tasklet == NULL){
		tasklet = tasklet_head_pop(&director.tasklet_normal_priority);
	}
	if (tasklet != NULL) {
		state = tasklet_runtime_get_locked(tasklet, true);
		if (state != NULL) {
			atomic_store_explicit(&state->queued, false, memory_order_release);
			atomic_fetch_add_explicit(&state->dispatch_refs, 1, memory_order_acq_rel);
		}
	}
	return tasklet;
}

static void tasklet_signal_waiters(struct tasklet_struct *tasklet){
	struct tasklet_runtime_state *state = NULL;

	state = tasklet_runtime_get(tasklet, false);
	if (state == NULL){
		return;
	}
	pthread_mutex_lock(&state->wait_lock);
	pthread_cond_broadcast(&state->wait_cond);
	pthread_mutex_unlock(&state->wait_lock);
}

static bool tasklet_can_queue(struct tasklet_struct *tasklet){
	if (director.tasklet_initialized == false || director.tasklet_stopped == true){
		return false;
	}
	if (tasklet_test_state(tasklet, TASKLET_STATE_KILL)){
		return false;
	}
        return true;
}

static bool __queue_tasklet_locked(struct tasklet_struct *tasklet, enum tasklet_priority priority){
	bool expected = false;
	struct tasklet_head *head = NULL;
	struct tasklet_runtime_state *state = NULL;

	if (tasklet_can_queue(tasklet) == false){
		return false;
	}

	state = tasklet_runtime_get_locked(tasklet, true);
	if (state == NULL){
		return false;
	}
	if (!atomic_compare_exchange_strong_explicit(&state->queued, &expected, true, memory_order_acq_rel, memory_order_acquire)){
		return false;
	}

	atomic_store_explicit(&state->priority, priority, memory_order_release);
	if(priority == TASKLET_PRIO_HIGH){
		head = &director.tasklet_high_priority;
	}else{
		head = &director.tasklet_normal_priority;
	}
	tasklet_head_push(head, tasklet);
	pthread_cond_signal(&director.tasklet_wake);
	return true;
}

static void queue_tasklet(struct tasklet_struct *tasklet, enum tasklet_priority priority){
	tasklet_sync_init();
	pthread_mutex_lock(&director.tasklet_lock);
	if (!__queue_tasklet_locked(tasklet, priority) && tasklet_can_queue(tasklet) == false){
		tasklet_clear_state(tasklet, TASKLET_STATE_SCHED);
	}
	pthread_mutex_unlock(&director.tasklet_lock);
}

static inline void tasklet_schedule(struct tasklet_struct *tasklet){
	if (tasklet == NULL){
		return;
	}
	if (!tasklet_test_and_set_state(tasklet, TASKLET_STATE_SCHED)){
		__tasklet_schedule(tasklet);
	}
}

static void tasklet_queue_pending(struct tasklet_struct *tasklet){
	enum tasklet_priority priority = TASKLET_PRIO_NORMAL;
	struct tasklet_runtime_state *state = NULL;

	if (!tasklet_test_state(tasklet, TASKLET_STATE_SCHED)){
		goto end;
	}
	if (tasklet_test_state(tasklet, TASKLET_STATE_RUN)){
		goto end;
	}
	if (tasklet_count_read(tasklet) != 0){
		goto end;
	}
	state = tasklet_runtime_get(tasklet, true);
	if (state == NULL){
		goto end;
	}
	if (atomic_load_explicit(&state->queued, memory_order_acquire)){
		goto end;
	}
	priority = (enum tasklet_priority)atomic_load_explicit(&state->priority, memory_order_acquire);
	queue_tasklet(tasklet, priority);
	end:
}

static void tasklet_action_common(struct tasklet_struct *tasklet){
	struct tasklet_runtime_state *state = NULL;

	state = tasklet_runtime_get(tasklet, true);
	if (state == NULL){
		goto end;
	}

	if (tasklet_test_and_set_state(tasklet, TASKLET_STATE_RUN)) {
		atomic_fetch_sub_explicit(&state->dispatch_refs, 1, memory_order_acq_rel);
		tasklet_signal_waiters(tasklet);
		goto end;
	}

	if (tasklet_test_state(tasklet, TASKLET_STATE_KILL)) {
		tasklet_clear_state(tasklet, TASKLET_STATE_SCHED);
		tasklet_clear_state(tasklet, TASKLET_STATE_RUN);
		atomic_fetch_sub_explicit(&state->dispatch_refs, 1, memory_order_acq_rel);
		tasklet_signal_waiters(tasklet);
		goto end;
	}

	if (tasklet_count_read(tasklet) != 0) {
		tasklet_clear_state(tasklet, TASKLET_STATE_RUN);
		atomic_fetch_sub_explicit(&state->dispatch_refs, 1, memory_order_acq_rel);
		tasklet_signal_waiters(tasklet);
		goto end;
	}

	if (!tasklet_test_and_clear_state(tasklet, TASKLET_STATE_SCHED)) {
		tasklet_clear_state(tasklet, TASKLET_STATE_RUN);
		atomic_fetch_sub_explicit(&state->dispatch_refs, 1, memory_order_acq_rel);
		tasklet_signal_waiters(tasklet);
		goto end;
	}

	current_tasklet = tasklet;
	if(tasklet->use_callback == true && tasklet->callback != NULL){
		tasklet->callback(tasklet);
	}else{
		if(tasklet->func != NULL){
			tasklet->func(tasklet->data);
		}
	}
	current_tasklet = NULL;
	tasklet_clear_state(tasklet, TASKLET_STATE_RUN);

	tasklet_queue_pending(tasklet);
	atomic_fetch_sub_explicit(&state->dispatch_refs, 1, memory_order_acq_rel);
	tasklet_signal_waiters(tasklet);
	end:
}

static void *tasklet_worker(void *arg){
	char name[16] = {};
	uintptr_t index = 0;
	struct tasklet_struct *tasklet = NULL;

	setup_percpu();
	index = (uintptr_t)arg;
	snprintf(name, sizeof(name), "tasklet/%d", index);
	pthread_setname_np(pthread_self(), name);
	for (;;) {
		pthread_mutex_lock(&director.tasklet_lock);
		while (director.tasklet_stopped == false && director.tasklet_high_priority.head == NULL && director.tasklet_normal_priority.head == NULL){
			pthread_cond_wait(&director.tasklet_wake, &director.tasklet_lock);
		}
		if (director.tasklet_stopped == true && director.tasklet_high_priority.head == NULL && director.tasklet_normal_priority.head == NULL) {
			pthread_mutex_unlock(&director.tasklet_lock);
			break;
		}

                tasklet = pop_tasklet();
		pthread_mutex_unlock(&director.tasklet_lock);

		if (tasklet != NULL){
			tasklet_action_common(tasklet);
		}
	}
	cleanup_percpu(NULL);
        return NULL;
}

static int tasklet_subsystem_init(unsigned int number_tasklet_workers){
	int ret = 0;

	tasklet_sync_init();

	if (number_tasklet_workers == 0){
		number_tasklet_workers = 1;
	}

	pthread_mutex_lock(&director.tasklet_lock);
	if (director.tasklet_initialized == true) {
		pthread_mutex_unlock(&director.tasklet_lock);
		ret = -EALREADY;
		return ret;
	}

	tasklet_head_init(&director.tasklet_normal_priority);
	tasklet_head_init(&director.tasklet_high_priority);
	director.tasklet_stopped = false;
	director.number_tasklet_workers = number_tasklet_workers;
	director.tasklet_workers = calloc(number_tasklet_workers, sizeof(pthread_t));
	if (director.tasklet_workers == NULL) {
		pthread_mutex_unlock(&director.tasklet_lock);
		ret = -ENOMEM;
		return ret;
	}
	director.tasklet_initialized = true;
	pthread_mutex_unlock(&director.tasklet_lock);

	for (int i = 0; i < number_tasklet_workers; ++i) {
		ret = pthread_create(&director.tasklet_workers[i], NULL, tasklet_worker, (void *)(uintptr_t)i);
		if (ret != 0) {
			pthread_mutex_lock(&director.tasklet_lock);
			director.tasklet_stopped = true;
			pthread_cond_broadcast(&director.tasklet_wake);
			pthread_mutex_unlock(&director.tasklet_lock);
			while (i > 0){
				pthread_join(director.tasklet_workers[--i], NULL);
			}
			if(director.tasklet_workers != NULL){
				free(director.tasklet_workers);
			}
			director.tasklet_workers = NULL;
			director.number_tasklet_workers = 0;
			director.tasklet_initialized = false;
			return ret;
		}
	}
	return ret;
}

static void tasklet_subsystem_reset_runtime_state(void){
	atomic_store_explicit(&director.tasklet_sync_state, 0, memory_order_release);
	director.tasklet_initialized = false;
	director.tasklet_stopped = false;
	director.tasklet_workers = NULL;
	director.number_tasklet_workers = 0;
	director.tasklet_runtime_states = NULL;
	tasklet_head_init(&director.tasklet_normal_priority);
	tasklet_head_init(&director.tasklet_high_priority);
}

static void tasklet_subsystem_shutdown(void){
	struct tasklet_runtime_state *state = NULL;
	struct tasklet_runtime_state *next = NULL;
	tasklet_sync_init();
	pthread_mutex_lock(&director.tasklet_lock);
	if (director.tasklet_initialized == false) {
		goto free_states;
	}
	director.tasklet_stopped = true;
	pthread_cond_broadcast(&director.tasklet_wake);
	pthread_mutex_unlock(&director.tasklet_lock);

	for (int i = 0; i < director.number_tasklet_workers; ++i){
		pthread_join(director.tasklet_workers[i], NULL);
	}

	pthread_mutex_lock(&director.tasklet_lock);
	if(director.tasklet_workers != NULL){
		free(director.tasklet_workers);
	}
	director.tasklet_workers = NULL;
	director.number_tasklet_workers = 0;
	director.tasklet_initialized = false;
	director.tasklet_stopped = false;
	tasklet_head_init(&director.tasklet_normal_priority);
	tasklet_head_init(&director.tasklet_high_priority);
	free_states:
	state = director.tasklet_runtime_states;
	director.tasklet_runtime_states = NULL;
	pthread_mutex_unlock(&director.tasklet_lock);
	while(state != NULL){
		next = state->next;
		pthread_cond_destroy(&state->wait_cond);
		pthread_mutex_destroy(&state->wait_lock);
		free(state);
		state = next;
	}
	end:
}

/*
 * worker_thread - the worker thread function
 * @__worker: self
 *
 * The worker thread function.  All workers belong to a worker_pool -
 * either a per-cpu one or dynamic unbound one.  These workers process all
 * work items regardless of their specific target workqueue.  The only
 * exception is work items which belong to workqueues with a rescuer which
 * will be explained in rescuer_thread().
 *
 * Return: 0
 */
static void *worker_thread(void *arg){
	struct work_struct *w = NULL;
	struct delayed_work *dw = NULL;
	struct work_struct *work = NULL;
	struct workqueue_struct *wq = NULL;
	void (*work_func)(struct work_struct *) = NULL;

	setup_percpu();
	wq = (struct workqueue_struct *)arg;
	pthread_mutex_lock(&wq->lock);
	for (;;) {
		if (wq->stop && list_empty(&wq->work_list) && list_empty(&wq->delayed_list) && wq->nr_active == 0){
			break;
		}
		delayed_work_timer_fn(wq);

		if (!list_empty(&wq->work_list)) {
			work = list_first_entry(&wq->work_list, struct work_struct, entry);
			work_func = work->func;
			list_del_init(&work->entry);
			atomic_store_explicit(&work->data, work_encode(wq, WORK_STRUCT_RUNNING), memory_order_relaxed);
			wq->nr_active++;

			pthread_mutex_unlock(&wq->lock);
			pthread_mutex_lock(&director.usb_io_lock);
			while (director.usb_io_depth > 0){
				pthread_cond_wait(&director.usb_io_idle, &director.usb_io_lock);
			}
			pthread_mutex_unlock(&director.usb_io_lock);
			if (work_func != NULL && (uintptr_t)work_func > 4096){
				work_func(work);
			}
			pthread_mutex_lock(&wq->lock);

			wq->nr_active--;
			if (atomic_load_explicit(&work->data, memory_order_acquire) & WORK_STRUCT_PENDING){
				atomic_store_explicit(&work->data, work_encode(wq, WORK_STRUCT_PENDING), memory_order_release);
			}else{
				atomic_store_explicit(&work->data, 0, memory_order_release);
			}
			pthread_cond_broadcast(&wq->cond);
			continue;
		}

		if (wq->stop) {
			pthread_cond_wait(&wq->cond, &wq->lock);
			continue;
		}

		if (list_empty(&wq->delayed_list)) {
			pthread_cond_wait(&wq->cond, &wq->lock);
		}else{
			w = NULL;
			dw = NULL;
			w = list_first_entry(&wq->delayed_list, struct work_struct, entry);
			dw = container_of(w, struct delayed_work, work);
			pthread_cond_timedwait(&wq->cond, &wq->lock, &dw->expires);
		}
	}
	pthread_mutex_unlock(&wq->lock);
	cleanup_percpu(NULL);
	return NULL;
}

static struct __task_struct *current_thread_info(void){
	struct __task_struct *current_task = NULL;
	pthread_condattr_t current_condition_attr = {};

	if (tls_current != NULL){
		return tls_current;
	}

	current_task = (struct __task_struct *)calloc(1, sizeof(struct __task_struct));

	if(current_task == NULL){
		return NULL;
	}

	pthread_mutex_init(&current_task->lock, NULL);

	pthread_condattr_init(&current_condition_attr);
	pthread_condattr_setclock(&current_condition_attr, CLOCK_MONOTONIC);
	pthread_cond_init(&current_task->cond, &current_condition_attr);
	pthread_condattr_destroy(&current_condition_attr);

	atomic_store(&current_task->__state, TASK_RUNNING);
	atomic_store(&current_task->started, true);
	atomic_store(&current_task->should_stop, false);

	current_task->tid = pthread_self();
	snprintf(current_task->name, sizeof(current_task->name), "main");

	tls_current = current_task;
	percpu_set_current_task();
	return current_task;
}

static inline bool wq_entry_is_linked(const struct wait_queue_entry *wq_entry){
	return !(wq_entry->entry.next == &wq_entry->entry && wq_entry->entry.prev == &wq_entry->entry);
}

/* kthread_trampoline: main function used for invoking the kthread cb */
static void *kthread_trampoline(void *arg){
	int ret = 0;
	struct __task_struct *current_thread = NULL;

	current_thread = (struct __task_struct *)arg;
	tls_current = current_thread;
	setup_percpu();
	#ifndef _WIN64
	pthread_cleanup_push(cleanup_percpu, NULL);
	#endif
	pthread_mutex_lock(&current_thread->lock);

	/* wait until the 'current_thread' has the 'started' flag set and the 'should_stop' flag isn't set */
	while (!atomic_load(&current_thread->started) && !atomic_load(&current_thread->should_stop)){
		pthread_cond_wait(&current_thread->cond, &current_thread->lock);
	}
	pthread_mutex_unlock(&current_thread->lock);

	if (!atomic_load(&current_thread->should_stop) && current_thread->threadfn){
		/* invoke the real thread callback */
		ret = current_thread->threadfn(current_thread->data);
	}
	/* set the exit code of the thread callback */
	current_thread->exit_code = ret;

	atomic_store(&current_thread->__state, TASK_RUNNING);
	pthread_mutex_lock(&current_thread->lock);
	pthread_cond_broadcast(&current_thread->cond);
	pthread_mutex_unlock(&current_thread->lock);

	#ifndef _WIN64
	pthread_cleanup_pop(1);
	#endif
	return NULL;
}
