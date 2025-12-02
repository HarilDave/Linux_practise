#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/ioctl.h>
#include <linux/mutex.h>
#include <linux/file.h>
#include <linux/fcntl.h>

#define DEVICE "vblock"
#define SIZE 4096
#define REGION 512
#define REGIONS 8

/* ioctl commands */
#define MAGIC 'V'
#define LOCK   _IOW(MAGIC, 1, int)
#define UNLOCK _IOW(MAGIC, 2, int)

/* storage */
static char disk[SIZE];
static char mirror[SIZE];
static int locked[REGIONS];

/* module params */
static int user_keys[4];
static int key_count;
module_param_array(user_keys, int, &key_count, 0644);

static int mirror_enable;
module_param(mirror_enable, int, 0644);

/* device stuff */
static dev_t dev;
static struct cdev cdev;
static struct class *cls;
static DEFINE_MUTEX(vlock);

/* prototype */
int vblock_backup_to_file(const char *path);

/* helper */
static int key_ok(int k)
{
	int i;
	for (i = 0; i < key_count; i++)
		if (user_keys[i] == k)
			return 1;
	return 0;
}

/* exported backup */
int vblock_backup_to_file(const char *path)
{
	struct file *f;
	loff_t pos = 0;

	f = filp_open(path, O_CREAT | O_WRONLY | O_TRUNC, 0644);
	if (IS_ERR(f))
		return -1;

	kernel_write(f, disk, SIZE, &pos);
	filp_close(f, NULL);
	return 0;
}
EXPORT_SYMBOL(vblock_backup_to_file);

/* read (FIXED) */
static ssize_t v_read(struct file *f, char __user *u,
		      size_t l, loff_t *off)
{
	if (*off >= SIZE)
		return 0;

	if (*off + l > SIZE)
		l = SIZE - *off;

	if (copy_to_user(u, disk + *off, l))
		return -EFAULT;

	*off += l;
	return l;
}

/* write */
static ssize_t v_write(struct file *f, const char __user *u,
		       size_t l, loff_t *off)
{
	char buf[64];
	int key, pos, reg;
	char payload[32];

	if (l >= sizeof(buf))
		l = sizeof(buf) - 1;

	if (copy_from_user(buf, u, l))
		return -EFAULT;
	buf[l] = 0;

	/* format: key:offset:data */
	sscanf(buf, "%d:%d:%s", &key, &pos, payload);

	if (pos < 0 || pos >= SIZE)
		return -EINVAL;

	reg = pos / REGION;

	mutex_lock(&vlock);

	if (locked[reg] && !key_ok(key)) {
		mutex_unlock(&vlock);
		return -EACCES;
	}

	strcpy(disk + pos, payload);
	if (mirror_enable)
		strcpy(mirror + pos, payload);

	mutex_unlock(&vlock);
	return l;
}

/* ioctl */
static long v_ioctl(struct file *f,
		    unsigned int cmd, unsigned long arg)
{
	int r;

	if (copy_from_user(&r, (int __user *)arg, sizeof(int)))
		return -EFAULT;

	if (r < 0 || r >= REGIONS)
		return -EINVAL;

	mutex_lock(&vlock);
	if (cmd == LOCK)
		locked[r] = 1;
	else if (cmd == UNLOCK)
		locked[r] = 0;
	mutex_unlock(&vlock);

	return 0;
}

static struct file_operations ops = {
	.owner = THIS_MODULE,
	.read = v_read,
	.write = v_write,
	.unlocked_ioctl = v_ioctl,
};

static int __init v_init(void)
{
	alloc_chrdev_region(&dev, 0, 1, DEVICE);
	cdev_init(&cdev, &ops);
	cdev_add(&cdev, dev, 1);
	cls = class_create(DEVICE);
	device_create(cls, NULL, dev, NULL, DEVICE);

	pr_info("vblock loaded\n");
	return 0;
}

static void __exit v_exit(void)
{
	device_destroy(cls, dev);
	class_destroy(cls);
	cdev_del(&cdev);
	unregister_chrdev_region(dev, 1);
	pr_info("vblock unloaded\n");
}

module_init(v_init);
module_exit(v_exit);

MODULE_LICENSE("GPL");

