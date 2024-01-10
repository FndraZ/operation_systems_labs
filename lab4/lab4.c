
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/version.h>
#include <linux/time.h>
#include <linux/timekeeping.h>

#define procfs_name "tsu"
static struct proc_dir_entry *our_proc_file = NULL;

static ssize_t procfile_read(struct file *file_pointer, char __user *buffer, size_t buffer_length, loff_t *offset)
{
    struct timespec64 now;
    ktime_get_real_ts64(&now);
    struct tm tm_now;
    time64_to_tm(now.tv_sec, 0, &tm_now);

    int x = (365 - tm_now.tm_yday - 1)/(tm_now.tm_yday + 1);

    ssize_t ret;
    char s[128];

    ret = sprintf(s, "until the new Year / from the new Year = %d\n", x);
    if (*offset >= ret || copy_to_user(buffer, s, ret))
    {
        ret = 0;
    }
    else
    {
        pr_info("procfile read %s\n", file_pointer->f_path.dentry->d_name.name);
        *offset += ret;
    }

    return ret;
}
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
static const struct proc_ops proc_file_fops = {
    .proc_read = procfile_read,
};
#else
static const struct file_operations proc_file_fops = {
    .read = procfile_read,
};
#endif

static int __init procfs1_init(void)
{
    our_proc_file = proc_create(procfs_name, 0644, NULL, &proc_file_fops);
    pr_info("/proc/%s created\n", procfs_name);
    return 0;
}

static void __exit procfs1_exit(void)
{
    proc_remove(our_proc_file);
    pr_info("/proc/%s removed\n", procfs_name);
}

module_init(procfs1_init);
module_exit(procfs1_exit);
MODULE_LICENSE("GPL");

/*
#include <time.h>
#include <stdio.h>
int main(){
    time_t t = time(NULL);
    struct tm tmm = *gmtime(&t);
    float x = ((float)365 - (float)tmm.tm_yday - 1)/(float)(tmm.tm_yday + 1);
    char snum[128];

    sprintf(snum,"%d",x);

    // Print our string
    printf("%s\n", snum);
}
*/
