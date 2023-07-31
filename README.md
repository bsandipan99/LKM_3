## Introduction
LKMs (Loadable Kernel Modules) are pieces of code that can be dynamically loaded into the Linux kernel at runtime without the need to restart the system. They allow developers to extend the functionality of the kernel without having to recompile the entire kernel or reboot the system. When an LKM is loaded, it becomes part of the running kernel and can execute privileged operations.

## Project Description
<p>
This project implements a simple priority_queue data structure in a LKM that supports various ioctl calls. The ioctl calls are used for setting and retrieving various configurations and internal information. Upon insertion of this LKM, it will create a proc file at the path "/proc/".  The description of the commands is as below:
</p>

* PB2_SET_CAPACITY : Initializes the LKM with the maximum priority-queue capacity, for the current process
* PB2_INSERT_INT : Inserting integer in the LKM for the current process. Followed by a "PB2_INSERT_PRIO" by the program in userspace.
* PB2_INSERT_PRIO : Inserting priority into LKM for the data inserted before this command.
* PB2_GET_INFO : Returns information about the state of the LKM
* PB2_GET_MIN : Getting element with minimum priority from the LKM.
* PB2_GET_MAX : Getting element with maximum priority from the LKM.

<p>
Interaction with the LKM is done by a userspace process. LKM is capable of handling concurrency and separate data from multiple processes. Also, no userspace program can open the file more than once simultaneously. However, the userspace process can reset the LKM (for the said process) by re-opening the file.
</p>

## Setup
In the **lkm** directory, execute the makefile
```Bash
make
```
Insert the LKM into kernel
```Bash
sudo insmod lkm.ko
```
Check whether module inserted into kernel
```Bash
lsmod
```
Interact with the LKM using the userspace program in **test** directory 
```Bash
gcc userspace.c && ./a.out
```
Remove the LKM from kernel using the commmand 
```Bash
sudo rmmod lkm
```


    
