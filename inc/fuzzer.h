#ifndef __EXSUL_FUZZING_H__
#define __EXSUL_FUZZING_H__

/* Generic fuzzing transport support. Driver-specific parsing belongs in the
 * runtime callback registered with fuzz_assign_urb_cb(). */
#ifdef ENABLE_FUZZING

#define FUZZ_URB_QUEUE_MAX 256

struct fuzz_urb_list {
	void *data;
	int len;
	struct fuzz_urb_list *next;
};

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
}

int fuzz_runtime_init(void){
	if(director.fuzz_runtime_initialized){
		return 0;
	}

	pthread_mutex_init(&director.fuzz_lock, NULL);
	pthread_cond_init(&director.fuzz_cond, NULL);
	director.fuzzing_mode_enabled = false;
	director.fuzz_accept_valid_urb = NULL;
	director.fuzz_urb_list = NULL;
	director.fuzz_urb_tail = NULL;
	director.fuzz_urb_count = 0;
	director.fuzz_callback_refs = 0;
	director.fuzz_runtime_initialized = true;
	return 0;
}

void fuzz_urb_clear(void){
	if(!director.fuzz_runtime_initialized){
		return;
	}
	pthread_mutex_lock(&director.fuzz_lock);
	fuzz_queue_clear_locked();
	pthread_mutex_unlock(&director.fuzz_lock);
}

int fuzz_urb_append(const void *data, size_t len){
	struct fuzz_urb_list *node = NULL;
	struct fuzz_urb_list *oldest = NULL;

	if(data == NULL || len == 0){
		return -EINVAL;
	}
	if(!director.fuzz_runtime_initialized){
		return -EINVAL;
	}

	node = calloc(1, sizeof(*node));
	if(node == NULL){
		return -ENOMEM;
	}
	node->data = malloc(len);
	if(node->data == NULL){
		free(node);
		return -ENOMEM;
	}
	memcpy(node->data, data, len);
	node->len = len;

	pthread_mutex_lock(&director.fuzz_lock);
	if(!director.fuzzing_mode_enabled){
		pthread_mutex_unlock(&director.fuzz_lock);
		free(node->data);
		free(node);
		return -ECANCELED;
	}
	if(director.fuzz_urb_count >= FUZZ_URB_QUEUE_MAX){
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
	return 0;
}

int fuzz_urb_wait(void *data, size_t capacity, size_t *out_len, unsigned int timeout_ms){
	int wait_ret = 0;
	struct fuzz_urb_list *node = NULL;
	#ifdef __linux__
	struct timespec deadline = {};
	#endif

	if(data == NULL || capacity == 0 || out_len == NULL || timeout_ms == 0){
		return -EINVAL;
	}
	*out_len = 0;
	if(!director.fuzz_runtime_initialized){
		return -EINVAL;
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
	while(director.fuzz_urb_list == NULL && director.fuzzing_mode_enabled){
		#ifdef __linux__
		wait_ret = pthread_cond_timedwait(&director.fuzz_cond,
					  &director.fuzz_lock, &deadline);
		if(wait_ret == ETIMEDOUT){
			pthread_mutex_unlock(&director.fuzz_lock);
			return -ETIMEDOUT;
		}
		#else
		wait_ret = pthread_cond_timedwait(&director.fuzz_cond,
					  &director.fuzz_lock, timeout_ms);
		if(wait_ret == 1){
			pthread_mutex_unlock(&director.fuzz_lock);
			return -ETIMEDOUT;
		}
		#endif
		if(wait_ret != 0){
			pthread_mutex_unlock(&director.fuzz_lock);
			return -EIO;
		}
	}
	if(director.fuzz_urb_list == NULL){
		pthread_mutex_unlock(&director.fuzz_lock);
		return -ECANCELED;
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
		return -ENOSPC;
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
	return 0;
}

int start_fuzzing(void){
	int ret = fuzz_runtime_init();

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

/* This reports only host transport state. Firmware-specific liveness checks
 * remain the responsibility of the runtime fuzzing script. */
int fuzz_transport_status(void){
	if(director.usb_dev == NULL ||
	   atomic_load_explicit(&director.usb_device_lost, memory_order_acquire)){
		return -ENODEV;
	}
	return 0;
}

int stop_fuzzing(void){
	if(!director.fuzz_runtime_initialized){
		return 0;
	}

	pthread_mutex_lock(&director.fuzz_lock);
	director.fuzzing_mode_enabled = false;
	pthread_cond_broadcast(&director.fuzz_cond);
	while(director.fuzz_callback_refs != 0){
		pthread_cond_wait(&director.fuzz_cond, &director.fuzz_lock);
	}
	fuzz_queue_clear_locked();
	pthread_mutex_unlock(&director.fuzz_lock);
	return 0;
}

/* Invoke the registered observer without holding fuzz_lock. The reference prevents TCC from unloading the callback while an RX completion uses it. */
int fuzz_report_usb_rx(void *data, int len){
	int ret = 0;
	int (*callback)(void *data, int len) = NULL;

	if(data == NULL || len <= 0 || !director.fuzz_runtime_initialized){
		return 0;
	}
	pthread_mutex_lock(&director.fuzz_lock);
	if(director.fuzzing_mode_enabled && director.fuzz_accept_valid_urb != NULL){
		callback = director.fuzz_accept_valid_urb;
		director.fuzz_callback_refs++;
	}
	pthread_mutex_unlock(&director.fuzz_lock);
	if(callback == NULL){
		return 0;
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

/* Assigning NULL is the required unregister operation before unloading TCC. */
int fuzz_assign_urb_cb(int (*fuzz_accept_urb_cb)(void *data, int len)){
	int ret = fuzz_runtime_init();

	if(ret != 0){
		return ret;
	}
	if(fuzz_accept_urb_cb == NULL){
		stop_fuzzing();
		pthread_mutex_lock(&director.fuzz_lock);
		director.fuzz_accept_valid_urb = NULL;
		pthread_mutex_unlock(&director.fuzz_lock);
		return 0;
	}

	pthread_mutex_lock(&director.fuzz_lock);
	if(director.fuzzing_mode_enabled || director.fuzz_callback_refs != 0){
		ret = -EBUSY;
	}else{
		director.fuzz_accept_valid_urb = fuzz_accept_urb_cb;
	}
	pthread_mutex_unlock(&director.fuzz_lock);
	return ret;
}

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
	return -EOPNOTSUPP;
}
#endif
#endif /* __EXSUL_FUZZING_H__ */
