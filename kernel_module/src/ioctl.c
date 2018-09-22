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
//     Core of Kernel Module for Processor Container
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
#include <linux/sched.h>
#include <linux/kthread.h>
typedef struct container_node container_node;
typedef struct thread_node thread_node;

struct thread_node{
	int tid;
	thread_node* next;
};

struct container_node{
	int cid;
	container_node* next;
	thread_node* current_thread;
};
//can't malloc in file-level scope
//current is kernel global variable, so name current can't use as variable name
//container_node* curr = NULL;
container_node* head = NULL;
container_node* tail = NULL;

int create_container(int cid){
	container_node* new_node = (container_node*)kmalloc(sizeof(container_node),GFP_KERNEL);
	//new_node->next = NULL;
	new_node->cid = cid;
	new_node->current_thread = NULL;
	if(head == NULL){
		head = new_node;
		head->next = head;
		tail = head;
	}else{
		tail->next = new_node;
		new_node->next = head;
		printk("after container %d ",tail->cid);
		tail = new_node;
		printk("add container %d\n",tail->cid);
	}		
	/**
	*original singly list
	if(curr == NULL){
		curr = new_node;
	}
	else{
		container_node* temp = curr;
		while(temp->next != NULL){
			temp = temp->next;
		}
		temp->next = new_node;
		printk("add new container %d after %d\n", new_node->cid, temp->cid);
	}
	**/
	return 0;
}

/**
 * Delete the task in the container.
 * 
 * external functions needed:
 * mutex_lock(), mutex_unlock(), wake_up_process(), 
 */
int processor_container_delete(struct processor_container_cmd __user *user_cmd)
{
	printk("Deleting a container\n");
    return 0;
}

/**
 * Create a task in the corresponding container.
 * external functions needed:
 * copy_from_user(), mutex_lock(), mutex_unlock(), set_current_state(), schedule()
 * 
 * external variables needed:
 * struct task_struct* current  
 */
int processor_container_create(struct processor_container_cmd __user *user_cmd)
{
	printk("Creating a container");
	printk("whose cid is  %llu\n",user_cmd->cid);
	create_container((int)user_cmd->cid);
    return 0;
}

/**
 * switch to the next task within the same container
 * 
 * external functions needed:
 * mutex_lock(), mutex_unlock(), wake_up_process(), set_current_state(), schedule()
 */
int processor_container_switch(struct processor_container_cmd __user *user_cmd)
{
	printk("Switching between container\n");
    return 0;
}

/**
 * control function that receive the command in user space and pass arguments to
 * corresponding functions.
 */
int processor_container_ioctl(struct file *filp, unsigned int cmd,
                              unsigned long arg)
{
    switch (cmd)
    {
    case PCONTAINER_IOCTL_CSWITCH:
        return processor_container_switch((void __user *)arg);
    case PCONTAINER_IOCTL_CREATE:
        return processor_container_create((void __user *)arg);
    case PCONTAINER_IOCTL_DELETE:
        return processor_container_delete((void __user *)arg);
    default:
        return -ENOTTY;
    }
}
