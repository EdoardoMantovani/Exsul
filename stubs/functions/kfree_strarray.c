void kfree_strarray(char **array, size_t n){
	if (!array){
		return;
	}
	for (int i = 0; i < n; i++){
		kfree(array[i]);
	}
	kfree(array);
}
