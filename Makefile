## [M1: point 1]
#  Explain following in here
#  ...
# this defines the name of the module we're building
MODULE	 = ex4

## [M2: point 1]
#  Explain following in here
#  ...
# this adds the new source code to the  
obj-m += $(MODULE).o

## [M3: point 1]
#  Explain following in here
#  ...
# this creates a path for the kernel directory if there alread isn't one
KERNELDIR ?= /lib/modules/$(shell uname -r)/build

## [M4: point 1]
#  Explain following in here
#  ...
# this creates a variable of the current directory
PWD := $(shell pwd)

## [M5: point 1]
#  Explain following in here
#  ...
# target to compile entire program
all: $(MODULE)


## [M6: point 1]
#  Explain following in here
#  ...
#This compiles the source code
%.o: %.c
	@echo "  CC      $<"
	@$(CC) -c $< -o $@

## [M7: point 1]
#  Explain following in here
#  ...
# This compiles the module
$(MODULE):
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

## [M8: point 1]
#  Explain following in here
#  ...
#remove old build files
clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
