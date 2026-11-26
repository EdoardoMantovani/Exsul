void kfree_strarray(char **array, size_t n){
	if (array == NULL){
		return;
	}
	for (int i = 0; i < n; i++){
		kfree(array[i]);
	}
	kfree(array);
}
