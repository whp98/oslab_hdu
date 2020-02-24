obj-m := show_task_family.o
KDIR := /home/whp/桌面/linux-4.20.17
PWD := $(shell pwd)
default:
	make -C $(KDIR) M=$(PWD) modules
clean:
	make -C $(KDIR) M=$(PWD) clean
