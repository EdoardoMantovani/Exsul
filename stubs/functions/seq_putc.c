void seq_putc(struct seq_file *m, char c){
	if (m->count >= m->size){
		goto end;
	}
	m->buf[m->count++] = c;
	end:
}
