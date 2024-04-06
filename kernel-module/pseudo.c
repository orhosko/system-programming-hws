#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/string.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Orhan Berkay Yılmaz");

static int pseudo_major = 0;
static int pseudo_minor = 0;

struct cdev pseudo_cdev;
struct semaphore pseudo_sem;

static int pseudo_capacity = 50;
module_param(pseudo_capacity, int, S_IRUGO);

static char *pseudo_data = NULL;

int pseudo_open(struct inode *inode, struct file *filp) {
  printk(KERN_DEBUG "pseudo: opening device\n");
  return 0;
}

int pseudo_release(struct inode *inode, struct file *filp) {
  printk(KERN_DEBUG "pseudo: closing device\n");
  return 0;
}

ssize_t pseudo_read(struct file *fp, char __user *buf, size_t count,
                    loff_t *f_pos) {
  int i = 0;
  int err = 0;

  if (down_interruptible(&pseudo_sem) != 0) {
    return -ERESTARTSYS;
  }

  // printk(KERN_DEBUG "pseudo: count: %d\n", count);
  // printk(KERN_DEBUG "pseudo: fpos: %d\n", *f_pos);

  if (*f_pos >= pseudo_capacity) {
    *f_pos = 0; // or % pseudo_capacity;

    // up(&pseudo_sem);
    // return 0;
  }

  // if (*f_pos + count > pseudo_capacity) {
  //   count = pseudo_capacity - *f_pos;
  // }

  for (i = 0; i < count; i++) {
    if (*f_pos >= pseudo_capacity) {
      *f_pos = 0;
    }

    err = copy_to_user(buf + i, pseudo_data + *f_pos, 1);

    if (err != 0) {
      up(&pseudo_sem);
      return -EFAULT;
    }

    (*f_pos)++;
  }

  up(&pseudo_sem);
  return count;
}

ssize_t pseudo_write(struct file *fp, const char *buf, size_t count,
                     loff_t *f_pos) {
  int err = 0;

  if (down_interruptible(&pseudo_sem) != 0) {
    return -ERESTARTSYS;
  }

  if (*f_pos >= pseudo_capacity) {
    up(&pseudo_sem);
    return 0;
  }

  printk(KERN_DEBUG "pseudo: count: %d\n", count);
  printk(KERN_DEBUG "pseudo: fpos: %d\n", *f_pos);

  if (*f_pos + count > pseudo_capacity) {
    count = pseudo_capacity - *f_pos;
  }

  err = copy_from_user(pseudo_data + *f_pos, buf, count);

  if (err != 0) {
    up(&pseudo_sem);
    return -EFAULT;
  }

  *f_pos += count;

  // for (int i = 0; i < count; i++) {
  //
  //   strncpy(pseudo_data + *f_pos, buf + i, 1);
  //
  //   if (err != 0) {
  //     up(&pseudo_sem);
  //     return -EFAULT;
  //   }
  //   (*f_pos)++;
  // }

  up(&pseudo_sem);
  return count;

  return 0;
}

loff_t pseudo_llseek(struct file *filp, loff_t off, int whence) {
  loff_t newpos;

  switch (whence) {
  case 0: /* SEEK_SET */
    newpos = off;
    break;

  case 1: /* SEEK_CUR */
    newpos = filp->f_pos + off;
    break;

  case 2: /* SEEK_END */
    newpos = pseudo_capacity + off;
    break;

  default: /* can't happen */
    return -EINVAL;
  }

  if (newpos < 0)
    return -EINVAL;

  filp->f_pos = newpos;
  return newpos;
}

long pseudo_ioctl(struct file *file, unsigned int cmd, unsigned long argp) {
  switch (cmd) {
  case 0: // SHIFT
    pr_info("Shift\n");

    int shift = 0;

    shift = (int)argp;

    int *temp = kmalloc(shift, GFP_KERNEL);
    for (int i = 0; i < shift; i++) {
      temp[i] = pseudo_data[i];
    }

    for (int i = 0; i < pseudo_capacity - shift; i++) {
      pseudo_data[i] = pseudo_data[i + shift];
    }

    for (int i = 0; i < shift; i++) {
      pseudo_data[pseudo_capacity - shift + i] = temp[i];
    }

    file->f_pos = 0;

    break;

  default:
    pr_info("Default, %d\n", cmd);
    break;
  }
  return 0;
}

struct file_operations pseudo_fops = {
    .owner = THIS_MODULE,
    .open = pseudo_open,
    .release = pseudo_release,
    .read = pseudo_read,
    .write = pseudo_write,
    .llseek = pseudo_llseek,
    .unlocked_ioctl = pseudo_ioctl,
};

/*
** This function will be called when we open the procfs file
*/
static int open_proc(struct inode *inode, struct file *file) {
  pr_info("proc file opend.....\t");
  return 0;
}
/*
** This function will be called when we close the procfs file
*/
static int release_proc(struct inode *inode, struct file *file) {
  pr_info("proc file released.....\n");
  return 0;
}
/*
** This function will be called when we read the procfs file
*/
static ssize_t read_proc(struct file *filp, char __user *buffer, size_t length,
                         loff_t *offset) {
  pr_info("proc file read.....\n");

  // if (copy_to_user(buffer, etx_array, 20)) {
  //   pr_err("Data Send : Err!\n");
  // }

  return length;
  ;
}
/*
** This function will be called when we write the procfs file
*/
static ssize_t write_proc(struct file *filp, const char *buff, size_t len,
                          loff_t *off) {
  pr_info("proc file wrote.....\n");

  // if (copy_from_user(etx_array, buff, len)) {
  //   pr_err("Data Write : Err!\n");
  // }

  return len;
}

/*
** procfs operation sturcture
*/
static struct proc_ops proc_fops = {.proc_open = open_proc,
                                    .proc_read = read_proc,
                                    .proc_write = write_proc,
                                    .proc_release = release_proc};

void pseudo_fill(void) {
  int i = 0;

  for (i = 0; i < pseudo_capacity; i++) {
    pseudo_data[i] = i;
  }
}

static int pseudo_init(void) {
  dev_t devno = 0;
  int result = -1;
  int err = 0;

  pseudo_data = kmalloc(pseudo_capacity, GFP_KERNEL);
  if (pseudo_data == NULL) {
    return -ENOMEM;
  }
  pseudo_fill();

  if (pseudo_major == 0) {
    result = alloc_chrdev_region(&devno, pseudo_minor, 1, "pseudo");
    pseudo_major = MAJOR(devno);
  } else {
    devno = MKDEV(pseudo_major, pseudo_minor);
    result = register_chrdev_region(devno, 1, "pseudo");
  }
  if (result < 0) {
    printk(KERN_WARNING "pseudo: can't get major %d\n", pseudo_major);
    return result;
  }

  sema_init(&pseudo_sem, 1);

  cdev_init(&pseudo_cdev, &pseudo_fops);
  pseudo_cdev.owner = THIS_MODULE;
  pseudo_cdev.ops = &pseudo_fops;
  err = cdev_add(&pseudo_cdev, devno, 1);
  if (err != 0) {
    printk(KERN_NOTICE "Error %d adding pseudo device", err);
  }
  proc_create("pseudo_proc", 0666, NULL, &proc_fops);
  return 0;
}

static void pseudo_exit(void) {
  dev_t devno = 0;

  cdev_del(&pseudo_cdev);
  devno = MKDEV(pseudo_major, pseudo_minor);
  unregister_chrdev_region(devno, 1);
  kfree(pseudo_data);
  remove_proc_entry("pseudo_proc", NULL);
}

module_init(pseudo_init);
module_exit(pseudo_exit);
