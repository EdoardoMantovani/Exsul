void seq_vprintf(struct seq_file *m, const char *f, va_list args){
	int len = 0;

	if (m->count < m->size) {
		len = vsnprintf(m->buf + m->count, m->size - m->count, f, args);
		if (m->count + len < m->size) {
			m->count += len;
			return;
		}
	}
	m->count = m->size;
}

