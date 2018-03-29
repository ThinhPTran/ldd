This code demostrate how to use completion variable to resolve synchronization problem
The scenario is: user process 1 writes data to kernel buffer, user process 2 reads data from kernel buffer.
We force that: process 2 can only read data if process 1 have written data before. To do that, we use
completion variable solution.

first, you enter "make" to build a kernel module. then, you enter "make app1 app2" to build 2 user applications.
after that, you run "./app2 &" to launch user applicaiton 2. You can check state of the process by "ps aux | grep app"
then, you run "./app1" to launch user application 1. Now, you can see process 2 shows its message.

