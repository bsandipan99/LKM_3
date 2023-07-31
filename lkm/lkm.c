#include <linux/proc_fs.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");

#define PB2_SET_CAPACITY _IOW(0x10, 0x31, int32_t*)
#define PB2_INSERT_INT _IOW(0x10, 0x32, int32_t*)
#define PB2_INSERT_PRIO _IOW(0x10, 0x33, int32_t*)
#define PB2_GET_INFO _IOR(0x10, 0x34, int32_t*)
#define PB2_GET_MIN _IOR(0x10, 0x35, int32_t*)
#define PB2_GET_MAX _IOR(0x10, 0x36, int32_t*)

static long ioctl(struct file *, unsigned int , unsigned long );
static int device_release(struct inode *, struct file *);
static int device_open(struct inode *, struct file *);

struct obj_info {
	int32_t prio_que_size; 	// current number of elements in priority-queue
	int32_t capacity;		// maximum capacity of priority-queue
};

static struct node
{
	int32_t data,priority;
};

static struct prio_que_ForEachProcess 
{
	int task_id;
	int length;
	struct node values[101];
	struct prio_que_ForEachProcess *nextTask;
	struct obj_info info;
//	struct node *head, *tail;
} *start ;			  
 

static struct proc_ops file_ops = 
{
	.proc_ioctl=ioctl,
	.proc_open = device_open,
	.proc_release = device_release
};




static int hello_init(void)
{
	struct proc_dir_entry *entry = proc_create("LKM", 0666, NULL, &file_ops);

	if(!entry) return -ENOENT;
	
	start=NULL;
	
	printk(KERN_ALERT"Intialising\n");
	return 0;
}


static void hello_exit(void)
{
	remove_proc_entry("A2_TA", NULL);
	printk(KERN_ALERT"Exiting\n");
}

static long ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int32_t num,index,i,min,max;
	struct prio_que_ForEachProcess *temp=start;
	struct node x;
	struct obj_info obj1;
	switch(cmd)
	{
		case PB2_SET_CAPACITY:
			//inaccessible pointer checking
			if(copy_from_user(&num,(int32_t*)arg,sizeof(num)))
				return -EINVAL;
			
			//invalid value checking
			if(num<1||num>100)
				return -EINVAL;
			
			
			printk(KERN_ALERT"Setting Capacity\n");
			
			//finding the task entry
			while(temp->task_id!=get_current()->pid)
				temp=temp->nextTask;
			
			//Checking if setting capacity for first time
			if(temp->info.capacity==0)
				temp->info.capacity=num;
			
			//else reset the LKM for this task
			else
			{
				temp->length = 0;
				temp->info.capacity=num;
			}
			return 0;
			
			
		case PB2_INSERT_INT:
			//inaccessible pointer checking
			if(copy_from_user(&num,(int32_t*)arg,sizeof(num)))
				return -EINVAL;
			
			//finding the task entry
			while(temp->task_id!=get_current()->pid)
				temp=temp->nextTask;
			
			//checking if priority_queue full
			if(temp->length == temp->info.capacity)
				return -EACCES;
			
			//checking if priority_queue capacity is 0
			if(temp->info.capacity==0)
				return -EACCES;
			
			//adding integer
			index = temp->length;
			temp->values[index].data = num;
		
			printk(KERN_ALERT"Inserted integer successfully\n");
			
			return 0;
		
		
		case PB2_INSERT_PRIO:
			//inaccessible pointer checking
			if(copy_from_user(&num,(int32_t*)arg,sizeof(num)))
				return -EINVAL;
			
			//finding the task entry
			while(temp->task_id!=get_current()->pid)
				temp=temp->nextTask;
			
			//checking if priority_queue full
			if(temp->length == temp->info.capacity)
				return -EACCES;
			
			//checking if priority_queue capacity is 0
			if(temp->info.capacity==0)
				return -EACCES;
			
			//adding priority
			index = temp->length;
			temp->values[index].priority = num;
			temp->length++;
			
			printk(KERN_ALERT"Inserted priority successfully\n");
			
			return 0;
			
			
		case PB2_GET_MIN:
			//finding the task entry
			while(temp->task_id!=get_current()->pid)
				temp=temp->nextTask;
			
			//checking if priority_queue capacity is not yet set
			if(temp->info.capacity==0)
				return -EACCES;
			
			//checking if priority_queue is currently empty
			if(temp->length==0)
				return -EACCES;
			
			//getting min priority element
			min = 0;
			
			index = temp->length;
			for(i=1;i<index;i++)
				if(temp->values[i].priority < temp->values[min].priority)
					min = i; 
			
			num = temp->values[min].data;
			x = temp->values[min];
			temp->values[min] = temp->values[index-1];
			temp->values[index-1] = x;
			
			if(copy_to_user((int32_t*) arg, &num,sizeof(num)))
				return -EINVAL;
			
			temp->length--;
				
			return 0;
			
		
		case PB2_GET_MAX:
			//finding the task entry
			while(temp->task_id!=get_current()->pid)
				temp=temp->nextTask;
			
			//checking if priority_queue capacity is not yet set
			if(temp->info.capacity==0)
				return -EACCES;
			
			//checking if priority_queue is currently empty
			if(temp->length == 0)
				return -EACCES;
			
			
			//getting max priority element
			max = 0;
			
			index = temp->length;
			for(i=1;i<index;i++)
				if(temp->values[i].priority > temp->values[max].priority)
					max = i; 
			
			num = temp->values[max].data;
			x = temp->values[max];
			temp->values[max] = temp->values[index-1];
			temp->values[index-1] = x;
			
			if(copy_to_user((int32_t*) arg, &num,sizeof(num)))
				return -EINVAL;
			
			temp->length--;
			
			return 0;

		case PB2_GET_INFO: 
			
			//finding the task entry
			while(temp->task_id!=get_current()->pid)
				temp=temp->nextTask;
			
			//storing data in 'obj'
			obj1.prio_que_size=temp->length;
			obj1.capacity=temp->info.capacity;
			
			//returning to userspace
			if(copy_to_user((struct obj_info*) arg, &obj1,sizeof(obj1)))
				return -EINVAL;
			
			return 0;
			
		default : return -EINVAL;
	
	
	}

}


static int device_release(struct inode *in, struct file *fi)
{
   
   struct prio_que_ForEachProcess *temp, *p;
   
   //searching node corresponding to process
   temp=start;
   while(temp->task_id!=get_current()->pid)
   {
   	p=temp;
	temp=temp->nextTask;   	
   }

   //Deleting node for that particular process
   if(temp==start)
   {
   	start=start->nextTask;	
   	kfree(temp);
  
   }
   else
   {
   	p->nextTask=temp->nextTask;
   	kfree(temp);
   }
   
   
   
   printk(KERN_ALERT"Device is released by process %d\n",get_current()->pid);
   return 0;
}


static int device_open(struct inode *in, struct file *fi)
{
   struct prio_que_ForEachProcess *temp,*p;
   
   if(start==NULL)		//first process 
   {
   	//creating node for process
   	start=(struct prio_que_ForEachProcess *)kmalloc(sizeof(struct prio_que_ForEachProcess), GFP_KERNEL);
   	
   	start->nextTask=NULL;
 	start->info.capacity=0;
 	start->length = 0;
    start->task_id=get_current()->pid;

	printk(KERN_ALERT"New Process!!\n");

   	return 0;
   }
   
   
   temp=start;
   while(temp->nextTask!=NULL)
   {
   	if(temp->task_id==get_current()->pid)
   		break;
   	else
   		temp=temp->nextTask;
   
   }
   
   if(temp->nextTask==NULL)
   {
   	if(temp->task_id==get_current()->pid)
   	{
	  	//process id already exists
	  	printk(KERN_ALERT"Trying to open file before closing previous instance!\n");
	  	return -EACCES;
   		
   	}
   	else
   	{
   		//create node for new task
   		p=(struct prio_que_ForEachProcess *)kmalloc(sizeof(struct prio_que_ForEachProcess), GFP_KERNEL);
   		
   		p->nextTask=NULL;
   		p->length = 0;
   		p->info.capacity=0;
   		p->task_id=get_current()->pid;
   		temp->nextTask=p;
   		
   		printk(KERN_ALERT"New Process!!\n");
   		
   	}
   
   }

   else
   {
  	//process id already exists
  	printk(KERN_ALERT"Trying to open file before closing previous instance!\n");
  	return -EACCES;
   	
   }
 
   return 0;
}


module_init(hello_init);
module_exit(hello_exit);
