obj-m += char_driver_4.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
app1:user_app1.c
	cc -o app1 user_app1.c
app2:user_app2.c
	cc -o app2 user_app2.c
