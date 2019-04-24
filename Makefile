ifndef ($(KERNELRELEASE)，)
	obj-m=pram.o
else
	KDIR=/
	CURRENT_DIR=$(shell pwd)

default:
	$(make) -C $(KDIR) M=$(CURRENT_DIR) modules
clean:
	$(make) -C $(KDIR) M=$(CURRENT_DIR) clean 

endif
