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

static DEFINE_MUTEX(access);

typedef struct container_node container_node;
typedef struct thread_node thread_node;

struct thread_node{
	int tid;
	thread_node* next;
};

struct container_node{
	int cid;
	container_node* next;
	thread_node* head_thread;
	thread_node* tail_thread;
	int num_of_thread;
};

//can't kmalloc in file-level scope
//current is kernel global variable, so name current can't use as variable name
//container_node* curr = NULL;
container_node* head = NULL;
container_node* tail = NULL;

int create_container(int cid){
	container_node* new_node = (container_node*)kmalloc(sizeof(container_node), GFP_KERNEL);//);
	
	new_node->cid = cid;
	new_node->head_thread = NULL;
	new_node->tail_thread = NULL;
	new_node->num_of_thread = 0;
	if (head == NULL)
	{
		head = new_node;
		head->next = head;
		tail = head;
		printk("add first container %d\n",head->cid);
	}
	else
	{
		tail->next = new_node;
		new_node->next = head;
		printk("after container %d ",tail->cid);
		tail = new_node;
		printk("add container %d\n",tail->cid);
	} 
	// printk("bug after this 12 ");
	return 0;
}

int delete_container(int cid){
    container_node* curr = head;
    container_node* prev = tail;
    //if list is empty
    if(head == NULL)
        return 0;
    //if list not empty but only one node
    if(head->cid == cid && tail->cid == cid){
        head = NULL;
        tail = NULL;
        return 0;
    }
    //if more than one 
    while(curr->cid != cid){
        prev = curr;
        curr = curr->next;
    }
    //tackle when deleting head
    if(curr == head)
    	head = curr->next;
    //tackle when deleting tail
    if(curr->next == head)
    	tail = prev;
    prev->next = curr->next;
    printk("delete a container %d\n", curr->cid);
    return 0;
}

container_node* find_container(int cid){
	container_node* curr = head;
	// //list has only one node 
	// if(curr == head && curr->cid == cid){
	// 	// printk("find container %d", curr->cid);
	// 	return curr;
	// }
	if(curr == NULL){
		printk("container %d doestn't exist, because there isn't even a container list\n",cid);
		return NULL;
	}
	while(curr != tail){
		// printk("-0-\n");
		if(curr->cid == cid){
			// printk("find at container %d \n", curr->cid );
			// printk("find container %d", curr->cid);
			// printk("-1-\n");
			printk("find container %d\n", curr->cid);
			return curr;
		}
		curr = curr->next;		
	}
	// printk("-2-\n");
	if(curr->cid == cid){
		// printk("find at container %d ", curr->cid );
		// printk("-3-\n");
		printk("find container %d\n", curr->cid);
		return curr;
	}
	printk("container %d doesn't exist\n",cid );
	return NULL;
}

int bind_thread(container_node* container, int tid){
	thread_node* new_thread = (thread_node*)kmalloc(sizeof(thread_node),GFP_KERNEL);
	new_thread->tid = tid;
	//printk("kmalloc for new thread_node success\n");
	// thread_node* head = container->head_thread;
	// thread_node* tail = container->tail_thread;
	//container haven't bind any thread
	if(container->head_thread == NULL){
		//printk("container %d 's head_thread is NULL\n",container->cid);
		// head = new_thread;
		// head->next = head;
		// tail = head;
		container->head_thread = new_thread;
		container->head_thread->next = container->head_thread;
		container->tail_thread = container->head_thread;
		printk("bind first thread(task) %d to container %d\n",tid,container->cid);
	}
	else{
		//printk("container %d already has a task \n",container->cid);
		// tail->next = new_thread;
		// new_thread->next = head;
		// printk("after thread %d ",tail->tid);
		// tail = new_thread;
		// printk("add thread %d\n",new_thread->tid);
		container->tail_thread->next = new_thread;
		new_thread->next = container->head_thread;
		printk("after thread %d ", container->tail_thread->tid );
		container->tail_thread = new_thread;
		printk("add thread %d\n ", new_thread->tid );
	}
	container->num_of_thread++;
	// printk("bug after this 2 ");
	return 0;
}

int unbind_thread(container_node* container, int tid){
	// thread_node* head = container->head_thread;
	// thread_node* tail = container->tail_thread;
	// thread_node* curr = head;
	// thread_node* prev = tail;
	// if(head == NULL)
	// 	//potential bug
	// 	return 0;
	// if(head->tid == tid && tail->tid == tid){
	// 	head = NULL;
	// 	tail = NULL;
	// 	return 0;
	// }
	// while(curr->tid != tid ){
	// 	prev = curr;
	// 	curr = curr->next;
	// }
	// if(curr == head)
	// 	head = curr->next;
	// if(curr->next == head)
	// 	tail = prev;
	// prev->next = curr->next;
	// return 0;
	thread_node* curr = container->head_thread;
	thread_node* prev = container->tail_thread;
	if(container->head_thread == NULL)
		return 0;
	if(container->head_thread->tid == tid && container->tail_thread->tid == tid){
		container->head_thread = NULL;
		container->tail_thread = NULL;
		printk("unbind last thread(task) %d in container %d\n",tid,container->cid);
		container->num_of_thread--;
		return 0;
	}
	while(curr->tid != tid){
		prev = curr;
		curr = curr->next;	
	}
	if(curr == container->head_thread)
		container->head_thread = curr->next; 
	if(curr->next == container->head_thread)
		container->tail_thread = prev;
	prev->next = curr->next;
	container->num_of_thread--;
	printk("delete thread %d in container %d\n", curr->tid,container->cid);
	return 0;
}

int print_thread_list(container_node* container){
	thread_node* head = container->head_thread;
	thread_node* curr = head;
	// thread_node* tail = container->tail_thread;
	if(curr == NULL){
		// printk("thread list of c%d is empty",container->cid);
		printk("empty");
		return 0;
	}
	printk("%d->", curr->tid);
	curr = curr->next;
	while(curr != head){
		printk("%d->", curr->tid);
		curr = curr->next;
	}
	printk("%d", curr->tid);
	return 0;
}

int print_container_list(void){
    container_node* curr = head;
    /**
     *	print list in a circular way: start from head end at head
     *  also check empty case
    **/
    if(curr == NULL){
    	printk("list is empty\n");
    	return 0;
    }
    printk("%d(", curr->cid);
    print_thread_list(find_container(curr->cid));
    printk(")->");
    curr = curr->next;
    while(curr != head){
    	printk("%d(",curr->cid);
    	print_thread_list(find_container(curr->cid));
    	printk(")->");
        curr = curr->next;
    }
    printk("%d(",curr->cid);
    print_thread_list(find_container(curr->cid));
    printk(")\n");
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
	struct processor_container_cmd cmd;
	struct task_struct* task = current;
	int tid = task->pid;
	copy_from_user(&cmd, user_cmd, sizeof(cmd));
	printk("entering lock\n");
	mutex_lock(&access);
	container_node* delete_node = find_container(cmd.cid);
	//mutex_unlock(&access);
	printk("inside lock\n");
	printk("Deleting a container ");
	printk("%llu\n",cmd.cid);
	//currently assume we only create one task for each container
	//mutex_lock(&access);
	unbind_thread(delete_node,tid);
	if(delete_node->num_of_thread == 0)
		delete_container(cmd.cid);
	mutex_unlock(&access);
	printk("just exit lock\n");
    return 0;
}

/**
 * Create a task in the corresponding container.
 * external functions needed
 * copy_from_user(), mutex_lock(), mutex_unlock(), set_current_state(), schedule()
 * 
 * external variables needed:
 * struct task_struct* current  
 */
int processor_container_create(struct processor_container_cmd __user *user_cmd)
{	struct processor_container_cmd cmd;
	struct task_struct* task = current;
	int tid = task->pid;
	
	// printk("whose cid is  %llu\n",user_cmd->cid);
	// int cid = (int)user_cmd->cid;
	//cause every time a threadbody create would call this function
	//we dont know whether this thread is first in the containre of not
	printk("Creating a container ");
	copy_from_user(&cmd, user_cmd, sizeof(cmd));
	printk("%llu\n",cmd.cid);
	printk("entering lock\n");
	mutex_lock(&access);
	printk("inside lock\n");
	if(find_container(cmd.cid) == NULL){
		create_container(cmd.cid);
		//printk("this line should be seen cause it's before calling find_container");
		container_node* new_node = find_container(cmd.cid);
		bind_thread(new_node,tid);
	}
	else{
		container_node* exist_node = find_container(cmd.cid);
		bind_thread(exist_node,tid);
	}
	mutex_unlock(&access);
	printk("just exit lock\n");
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
