//////////////////////////////////////////////////////////////////////
//                      North Carolina State University
//
//
//
//                             Copyright 2016
//
////////////////////////////////////////////////////////////////////////
//
// This program is free software; you can redistribute it and/or modify it
// under the terms and conditions of the GNU General Public License,
// version 2, as published by the Free Software Foundation.
//
// This program is distributed in the hope it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
// more details.
//
// You should have received a copy of the GNU General Public License along with
// this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
//
////////////////////////////////////////////////////////////////////////
//
//   Author:  Hung-Wei Tseng, Yu-Chia Liu
//
//   Description:
//     Interface of Kernel Module
//
////////////////////////////////////////////////////////////////////////

#include "processor_container.h"

#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/poll.h>
#include <linux/mutex.h>

extern long processor_container_lock(struct processor_container_cmd __user *user_cmd);
extern long processor_container_unlock(struct processor_container_cmd __user *user_cmd);
extern long processor_container_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);
extern int processor_container_init(void);
extern void processor_container_exit(void);

static const struct file_operations processor_container_fops = {
    .owner                = THIS_MODULE,
    .unlocked_ioctl       = processor_container_ioctl,
};

struct miscdevice processor_container_dev = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "pcontainer",
    .fops = &processor_container_fops,
};

static int __init processor_container_module_init(void)
{
    return processor_container_init();
}

static void __exit processor_container_module_exit(void)
{
    processor_container_exit();
}


MODULE_AUTHOR("Hung-Wei Tseng <htseng3@ncsu.edu>");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
module_init(processor_container_module_init);
module_exit(processor_container_module_exit);
