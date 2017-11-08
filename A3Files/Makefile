obj-m := sbd.o
KDIR := /scratch/fall2017/11/group11repo/linux
PWD_C := $(shell pwd)

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD_C) modules
clean:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD_C) clean

