void complete_all(struct completion *x){
        atomic_store_explicit(&x->done, UINT_MAX, memory_order_release);
        futex_wake(&x->done, 0, 0, 0);
}
