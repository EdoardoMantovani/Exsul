/* made by ChatGPT 5.4 */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/file.h>
#include <linux/fcntl.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/string.h>

#define READ_BUF_SZ 128

static char *path = "/tmp/km_file_demo.txt";
module_param(path, charp, 0644);
MODULE_PARM_DESC(path, "Path of the file used by the demo module");

MODULE_LICENSE("GPL");
MODULE_AUTHOR("OpenAI");
MODULE_DESCRIPTION("Simple kernel module example: open/write/seek/read/close a file");
MODULE_VERSION("1.0");

static int run_file_demo(const char *filename)
{
	struct file *filp = NULL;
	char *read_buf = NULL;
	const char *write_buf = "hello from kernel module\n";
	loff_t pos;
	ssize_t nread, nwritten;
	int ret = 0;

	/* 1) OPEN / CREATE */
	filp = filp_open(filename, O_CREAT | O_RDWR | O_TRUNC, 0644);
	if (IS_ERR(filp)) {
		ret = PTR_ERR(filp);
		pr_err("file_io_demo: filp_open('%s') failed: %d\n", filename, ret);
		return ret;
	}

	pr_info("file_io_demo: opened '%s'\n", filename);

	/* 2) WRITE */
	pos = 0;
	nwritten = kernel_write(filp, write_buf, strlen(write_buf), &pos);
	if (nwritten < 0) {
		ret = (int)nwritten;
		pr_err("file_io_demo: kernel_write failed: %d\n", ret);
		goto out_close;
	}

	pr_info("file_io_demo: wrote %zd bytes\n", nwritten);

	/* 3) SEEK to beginning */
	pos = vfs_llseek(filp, 0, SEEK_SET);
	if (pos < 0) {
		ret = (int)pos;
		pr_err("file_io_demo: vfs_llseek(0, SEEK_SET) failed: %d\n", ret);
		goto out_close;
	}

	pr_info("file_io_demo: seeked to offset %lld\n", pos);

	read_buf = kzalloc(READ_BUF_SZ, GFP_KERNEL);
	if (!read_buf) {
		ret = -ENOMEM;
		goto out_close;
	}

	/* 4) READ whole content back */
	nread = kernel_read(filp, read_buf, READ_BUF_SZ - 1, &pos);
	if (nread < 0) {
		ret = (int)nread;
		pr_err("file_io_demo: kernel_read failed: %d\n", ret);
		goto out_free;
	}

	read_buf[nread] = '\0';
	pr_info("file_io_demo: read %zd bytes: \"%s\"\n", nread, read_buf);

	/* 5) SEEK again, this time to an interior offset */
	pos = vfs_llseek(filp, 6, SEEK_SET);
	if (pos < 0) {
		ret = (int)pos;
		pr_err("file_io_demo: vfs_llseek(6, SEEK_SET) failed: %d\n", ret);
		goto out_free;
	}

	memset(read_buf, 0, READ_BUF_SZ);

	/* 6) READ again from the new offset */
	nread = kernel_read(filp, read_buf, READ_BUF_SZ - 1, &pos);
	if (nread < 0) {
		ret = (int)nread;
		pr_err("file_io_demo: second kernel_read failed: %d\n", ret);
		goto out_free;
	}

	read_buf[nread] = '\0';
	pr_info("file_io_demo: read from offset 6: \"%s\"\n", read_buf);

out_free:
	kfree(read_buf);

out_close:
	/* 7) CLOSE */
	filp_close(filp, NULL);
	pr_info("file_io_demo: closed '%s'\n", filename);

	return ret;
}

static int __init file_io_demo_init(void)
{
	pr_info("file_io_demo: module loaded\n");
	return run_file_demo(path);
}

static void __exit file_io_demo_exit(void)
{
	pr_info("file_io_demo: module unloaded\n");
}

module_init(file_io_demo_init);
module_exit(file_io_demo_exit);
