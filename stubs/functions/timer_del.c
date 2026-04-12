int timer_del(struct timer_list *timer){
        int was_pending = 0;
        struct hlist_node *next = NULL;
        struct hlist_node **pprev = NULL;

        pthread_mutex_lock(&director.timer_lock);

        if (timer->entry.pprev != NULL) {
                was_pending = 1;
                next  = timer->entry.next;
                pprev = timer->entry.pprev;
                *pprev = next;
                if (next != NULL){
                        next->pprev = pprev;
                }
                timer->entry.next  = NULL;
                timer->entry.pprev = NULL;
                pthread_cond_signal(&director.timer_cond);
        }

        pthread_mutex_unlock(&director.timer_lock);
        return was_pending;
}
