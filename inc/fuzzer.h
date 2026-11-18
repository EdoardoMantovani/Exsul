#ifndef __EXSUL_FUZZING_H__
#define __EXSUL_FUZZING_H__

#ifdef ENABLE_FUZZING

#define FUZZ_URB_QUEUE_MAX 256
static unsigned long long fuzz_urb_total_drops;
static bool fuzz_urb_overflow_pending;

/* fuzz_urb_list: linked list of urbs which will be submitted to the usb dongle for fuzzing */
struct fuzz_urb_list {
	void *data;
	int len;
	struct fuzz_urb_list *next;
};

/* fuzz_queue_clear_locked: clear the fuzzer urb queue */
void fuzz_queue_clear_locked(void){
	struct fuzz_urb_list *node = NULL;
	struct fuzz_urb_list *next = NULL;

	node = director.fuzz_urb_list;
	while(node != NULL){
		next = node->next;
		free(node->data);
		free(node);
		node = next;
	}
	director.fuzz_urb_list = NULL;
	director.fuzz_urb_tail = NULL;
	director.fuzz_urb_count = 0;
	fuzz_urb_overflow_pending = false;
}

/* fuzz_runtime_init: init the fuzzing environment */
int fuzz_runtime_init(void){
	int ret = 0;

	if(director.fuzz_runtime_initialized){
		return ret;
	}

	pthread_mutex_init(&director.fuzz_lock, NULL);
	pthread_cond_init(&director.fuzz_cond, NULL);
	director.fuzzing_mode_enabled = false;
	director.fuzz_accept_valid_urb = NULL;
	director.fuzz_urb_list = NULL;
	director.fuzz_urb_tail = NULL;
	director.fuzz_urb_count = 0;
	fuzz_urb_total_drops = 0;
	fuzz_urb_overflow_pending = false;
	director.fuzz_callback_refs = 0;
	director.fuzz_runtime_initialized = true;
	return ret;
}

void fuzz_urb_clear(void){
	if(!director.fuzz_runtime_initialized){
		return;
	}
	pthread_mutex_lock(&director.fuzz_lock);
	fuzz_queue_clear_locked();
	pthread_mutex_unlock(&director.fuzz_lock);
}

int fuzz_urb_append(void *data, size_t len){
	int ret = 0;
	struct fuzz_urb_list *node = NULL;
	struct fuzz_urb_list *oldest = NULL;

	if(data == NULL || len == 0){
		ret = -EINVAL;
		return ret;
	}
	if(!director.fuzz_runtime_initialized){
		ret = -EINVAL;
		return ret;
	}

	node = calloc(1, sizeof(*node));
	if(node == NULL){
		ret = -ENOMEM;
		return ret;
	}
	node->data = calloc(1, len);
	if(node->data == NULL){
		free(node);
		ret = -ENOMEM;
		return ret;
	}
	memcpy(node->data, data, len);
	node->len = len;

	pthread_mutex_lock(&director.fuzz_lock);
	if(director.fuzzing_mode_enabled == false){
		pthread_mutex_unlock(&director.fuzz_lock);
		free(node->data);
		free(node);
		ret = -ECANCELED;
		return ret;
	}
	if(director.fuzz_urb_count >= FUZZ_URB_QUEUE_MAX){
		fuzz_urb_total_drops++;
		fuzz_urb_overflow_pending = true;
		if((fuzz_urb_total_drops & (fuzz_urb_total_drops - 1)) == 0){
			#ifdef ENABLE_DBG_PRINT
			printf("[!] fuzz feedback queue overflow, total dropped=%llu\n", fuzz_urb_total_drops);
			#endif
		}
		oldest = director.fuzz_urb_list;
		director.fuzz_urb_list = oldest->next;
		if(director.fuzz_urb_list == NULL){
			director.fuzz_urb_tail = NULL;
		}
		director.fuzz_urb_count--;
		free(oldest->data);
		free(oldest);
	}
	if(director.fuzz_urb_tail == NULL){
		director.fuzz_urb_list = node;
	}else{
		director.fuzz_urb_tail->next = node;
	}
	director.fuzz_urb_tail = node;
	director.fuzz_urb_count++;
	pthread_cond_signal(&director.fuzz_cond);
	pthread_mutex_unlock(&director.fuzz_lock);
	return ret;
}

int fuzz_urb_wait(void *data, size_t capacity, size_t *out_len, unsigned int timeout_ms){
	int ret = 0;
	int wait_ret = 0;
	struct fuzz_urb_list *node = NULL;
	#ifdef __linux__
	struct timespec deadline = {};
	#endif

	if(data == NULL || capacity == 0 || out_len == NULL || timeout_ms == 0){
		ret = -EINVAL;
		return ret;
	}
	*out_len = 0;
	if(!director.fuzz_runtime_initialized){
		ret = -EINVAL;
		return ret;
	}

	#ifdef __linux__
	clock_gettime(CLOCK_REALTIME, &deadline);
	deadline.tv_sec += timeout_ms / 1000;
	deadline.tv_nsec += (long)(timeout_ms % 1000) * 1000000L;
	if(deadline.tv_nsec >= 1000000000L){
		deadline.tv_sec++;
		deadline.tv_nsec -= 1000000000L;
	}
	#endif

	pthread_mutex_lock(&director.fuzz_lock);
	if(fuzz_urb_overflow_pending){
		fuzz_urb_overflow_pending = false;
		pthread_mutex_unlock(&director.fuzz_lock);
		ret = -EOVERFLOW;
		return ret;
	}
	while(director.fuzz_urb_list == NULL && director.fuzzing_mode_enabled){
		#ifdef __linux__
		wait_ret = pthread_cond_timedwait(&director.fuzz_cond, &director.fuzz_lock, &deadline);
		if(wait_ret == ETIMEDOUT){
			pthread_mutex_unlock(&director.fuzz_lock);
			ret = -ETIMEDOUT;
			return ret;
		}
		#else
		wait_ret = pthread_cond_timedwait(&director.fuzz_cond, &director.fuzz_lock, timeout_ms);
		if(wait_ret == 1){
			pthread_mutex_unlock(&director.fuzz_lock);
			ret = -ETIMEDOUT;
			return ret;
		}
		#endif
		if(wait_ret != 0){
			pthread_mutex_unlock(&director.fuzz_lock);
			ret = -EIO;
			return ret;
		}
	}
	if(fuzz_urb_overflow_pending){
		fuzz_urb_overflow_pending = false;
		pthread_mutex_unlock(&director.fuzz_lock);
		ret = -EOVERFLOW;
		return ret;
	}
	if(director.fuzz_urb_list == NULL){
		pthread_mutex_unlock(&director.fuzz_lock);
		ret = -ECANCELED;
		return ret;
	}

	node = director.fuzz_urb_list;
	*out_len = node->len;
	if(node->len > capacity){
		director.fuzz_urb_list = node->next;
		if(director.fuzz_urb_list == NULL){
			director.fuzz_urb_tail = NULL;
		}
		director.fuzz_urb_count--;
		pthread_mutex_unlock(&director.fuzz_lock);
		free(node->data);
		free(node);
		ret = -ENOSPC;
		return ret;
	}
	director.fuzz_urb_list = node->next;
	if(director.fuzz_urb_list == NULL){
		director.fuzz_urb_tail = NULL;
	}
	director.fuzz_urb_count--;
	pthread_mutex_unlock(&director.fuzz_lock);
	memcpy(data, node->data, node->len);
	free(node->data);
	free(node);
	return ret;
}

/* start_fuzzing: wrapper for setting 'director.fuzzing_mode_enabled' to true */
int start_fuzzing(void){
	int ret = 0;

	ret = fuzz_runtime_init();
	if(ret != 0){
		return ret;
	}
	pthread_mutex_lock(&director.fuzz_lock);
	if(director.fuzzing_mode_enabled){
		ret = -EBUSY;
	}else if(director.fuzz_accept_valid_urb == NULL){
		ret = -EINVAL;
	}else{
		fuzz_queue_clear_locked();
		director.fuzzing_mode_enabled = true;
	}
	pthread_mutex_unlock(&director.fuzz_lock);
	return ret;
}

/* fuzz_urb_drop_count: reports only host transport state */
unsigned long long fuzz_urb_drop_count(void){
	int ret = 0;
	unsigned long long count = 0;

	if(!director.fuzz_runtime_initialized){
		return ret;
	}
	pthread_mutex_lock(&director.fuzz_lock);
	count = fuzz_urb_total_drops;
	pthread_mutex_unlock(&director.fuzz_lock);
	return count;
}

/* fuzz_transport_status: test if the device under fuzzing disconnected from the host (it may means that it is crashed) */
int fuzz_transport_status(void){
	int ret = 0;

	if(director.usb_dev == NULL || atomic_load_explicit(&director.usb_device_lost, memory_order_acquire)){
		ret = -ENODEV;
		return ret;
	}
	return ret;
}

int stop_fuzzing(void){
	int ret = 0;

	if(!director.fuzz_runtime_initialized){
		return ret;
	}

	pthread_mutex_lock(&director.fuzz_lock);
	director.fuzzing_mode_enabled = false;
	pthread_cond_broadcast(&director.fuzz_cond);
	while(director.fuzz_callback_refs != 0){
		pthread_cond_wait(&director.fuzz_cond, &director.fuzz_lock);
	}
	fuzz_queue_clear_locked();
	pthread_mutex_unlock(&director.fuzz_lock);
	return ret;
}

/* fuzz_report_usb_rx: invoke the registered observer without holding fuzz_lock */
int fuzz_report_usb_rx(void *data, int len){
	int ret = 0;
	int (*callback)(void *data, int len) = NULL;

	if(data == NULL || len <= 0 || !director.fuzz_runtime_initialized){
		return ret;
	}
	pthread_mutex_lock(&director.fuzz_lock);
	if(director.fuzzing_mode_enabled && director.fuzz_accept_valid_urb != NULL){
		callback = director.fuzz_accept_valid_urb;
		director.fuzz_callback_refs++;
	}
	pthread_mutex_unlock(&director.fuzz_lock);
	if(callback == NULL){
		return ret;
	}

	ret = callback(data, len);
	pthread_mutex_lock(&director.fuzz_lock);
	director.fuzz_callback_refs--;
	if(director.fuzz_callback_refs == 0){
		pthread_cond_broadcast(&director.fuzz_cond);
	}
	pthread_mutex_unlock(&director.fuzz_lock);
	return ret;
}

/* fuzz_assign_urb_cb: I had the idea of this function because the normal fuzzing cannot validate if
 * an urb contains the correct 'seq' number for linking a fuzzer urb with a response one, ideally,
 * any vendor module needs to setup it's own custom function for classifying if a fuzzer urb is legit or not */
int fuzz_assign_urb_cb(int (*fuzz_accept_urb_cb)(void *data, int len)){
	int ret = 0;

	ret = fuzz_runtime_init();
	if(ret != 0){
		return ret;
	}
	/* if the vendor callback used for accepting or not the urb is 'NULL', return */
	if(fuzz_accept_urb_cb == NULL){
		stop_fuzzing();
		pthread_mutex_lock(&director.fuzz_lock);
		director.fuzz_accept_valid_urb = NULL;
		pthread_mutex_unlock(&director.fuzz_lock);
		return ret;
	}

	pthread_mutex_lock(&director.fuzz_lock);
	if(director.fuzzing_mode_enabled || director.fuzz_callback_refs != 0){
		ret = -EBUSY;
	}else{
		/* assign the callback for the vendor module */
		director.fuzz_accept_valid_urb = fuzz_accept_urb_cb;
	}
	pthread_mutex_unlock(&director.fuzz_lock);
	return ret;
}

/* fuzz_runtime_destroy: deinitialize the fuzz subsystem */
void fuzz_runtime_destroy(void){
	if(!director.fuzz_runtime_initialized){
		return;
	}
	fuzz_assign_urb_cb(NULL);
	pthread_cond_destroy(&director.fuzz_cond);
	pthread_mutex_destroy(&director.fuzz_lock);
	director.fuzz_runtime_initialized = false;
}
#else
int fuzz_assign_urb_cb(int (*fuzz_accept_urb_cb)(void *data, int len)){
	int ret = 0;

	ret = -EOPNOTSUPP;
	return ret;
}

int fuzz_transport_status(void){
	int ret = 0;

	ret = -EOPNOTSUPP;
	return ret;
}
#endif
#endif /* __EXSUL_FUZZING_H__ */
