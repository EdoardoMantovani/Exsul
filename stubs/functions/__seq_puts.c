void __seq_puts(struct seq_file *m, const char *s){
	seq_write(m, s, strlen(s));
}
