obj-m += exb_core.o
obj-m += exb_driver.o
obj-m += exb_device.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
