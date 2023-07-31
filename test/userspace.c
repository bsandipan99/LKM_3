#include<stdio.h>
#include<sys/types.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/ioctl.h>

#define PB2_SET_CAPACITY _IOW(0x10, 0x31, int32_t*)
#define PB2_INSERT_INT _IOW(0x10, 0x32, int32_t*)
#define PB2_INSERT_PRIO _IOW(0x10, 0x33, int32_t*)
#define PB2_GET_INFO _IOR(0x10, 0x34, int32_t*)
#define PB2_GET_MIN _IOR(0x10, 0x35, int32_t*)
#define PB2_GET_MAX _IOR(0x10, 0x36, int32_t*)

struct obj_info{
	int32_t prio_que_size;
	int32_t capacity;
};


int main()
{
	int j,fd,i,check;
	int32_t n,m;
	char ch;
	int32_t pq[22]={0};
	struct obj_info temp;
		
	fork();			//creating child processes
	fork();
	//fork();
	//fork();
	//fork();
	
	
	fd=open("/proc/LKM", O_RDWR);
	if(fd<0) 
	{
		printf("Could not open File!!!\n");
		return 1;
	}
	
	n=10;				//intialising value of n
	ch=n;				//storing it in a char variable
	
	check=ioctl(fd,PB2_GET_INFO,&temp);		//checking info
	printf("capacity = %d, prio_que_size = %d\n",temp.capacity,temp.prio_que_size);
	
	check=ioctl(fd,PB2_SET_CAPACITY,&n);		//setting capacity
	if(check<0)
	{
		//setting capacity failed
		printf("setting capacity failed\n");
		close(fd);
		return 0;
	}
	
	//odd index --> data, even index --> priority
	pq[0]=34;
	pq[1]=15;		
	pq[2]=-2;
	pq[3]=42;
	pq[4]=25;
	pq[5]=1;
	pq[6]=3;
	pq[7] = 43;
	pq[8] = 9;
	pq[9] = 6;
	pq[10] = 13;
	pq[11] = 15;
	pq[12]=8;
	pq[13] = 16;
	pq[14]=7;
	pq[15] = 16;
	pq[16]=31;
	pq[17] = 42;
	pq[18]=24;
	pq[19]=1;
	
	//inserting values one at a time
	for(i=0;i<20;i+=2)
	{
		//data
		check=ioctl(fd,PB2_INSERT_INT,&pq[i]);
		sleep(0.5);
		if(check<0)
		{
			//write operation failed
			printf("Writing integer data failed\n");
			close(fd);
			return 0;
		}
		
		//priority
		check=ioctl(fd,PB2_INSERT_PRIO,&pq[i+1]);
		sleep(0.5);
		if(check<0)
		{
			//write operation failed
			printf("Writing integer data failed\n");
			close(fd);
			return 0;
		}
		
		if(i==9)
		{
			//checking info 
			check=ioctl(fd,PB2_GET_INFO,&temp);	
			printf("capacity = %d, prio_que_size = %d\n",temp.capacity,temp.prio_que_size);
		
		}
	}
	
	
	////trying to write beyond capacity
	check=ioctl(fd,PB2_INSERT_INT,&pq[20]);
	sleep(0.5);
	if(check<0)
	{
		printf("Capacity Exceeded!!!\n");
	}
	 

	
	//reading values by max/min priority
	for(i=0;i<10;i+=2)
	{
		//min priority
		check=ioctl(fd,PB2_GET_MIN,&m);
		sleep(0.5);
		if(check!=0)
		{
			//read operation failed
			printf("Reading integer data failed\n");
			close(fd);
			return 0;
		}
		printf("Min Priority Element = %d\n",(int32_t)m); 
	
		//max priority
		check=ioctl(fd,PB2_GET_MAX,&m);
		sleep(0.5);
		if(check!=0)
		{
			//read operation failed
			printf("Reading integer data failed\n");
			close(fd);
			return 0;
		}
		printf("Max Priority Element = %d\n",(int32_t)m); 
	
	}	


	////reading from empty priority queue
	check=ioctl(fd,PB2_GET_MIN,&m);;
	sleep(0.5);
	if(check!=0)
		printf("Reading from Empty priority queue!!!\n");	
		

	close(fd);
	return 0;
}
