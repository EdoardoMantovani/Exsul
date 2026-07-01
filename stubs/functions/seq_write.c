int seq_write(struct seq_file *seq, const void *data, size_t len){
	int ret = 0;

	if (seq->count + len < seq->size) {
		memcpy(seq->buf + seq->count, data, len);
		seq->count += len;
		return ret;
	}
	seq->count = seq->size;
	ret = -EIO;
	return ret;
}
