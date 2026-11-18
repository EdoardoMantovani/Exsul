void delayed_work_timer_fn(struct workqueue_struct *wq){
        struct timespec now = {};

        now = wq_now(wq);
        while (!list_empty(&wq->delayed_list)) {
                struct delayed_work *dwork = NULL;
                struct work_struct *work = NULL;

                work = list_first_entry(&wq->delayed_list, struct work_struct, entry);
                dwork = container_of(work, struct delayed_work, work);

                if (timespec_cmp(&dwork->expires, &now) > 0){
                        break;
                }

                list_del_init(&work->entry);

                atomic_store_explicit(&work->data, work_encode(wq, WORK_STRUCT_PENDING), memory_order_relaxed);
                list_add_tail(&work->entry, &wq->work_list);
        }
}
