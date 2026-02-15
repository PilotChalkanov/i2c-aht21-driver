# Makefile for I2C AHT21 Driver

# Debugging flag
DEBUG = y
# Module name
obj-m += aht21.o

ifeq ($(DEBUG),y)
  DEBFLAGS = -O -g -DAHT_DEBUG # "-O" is needed to expand inlines
else
  DEBFLAGS = -O2
endif

# Kernel source directory
KDIR ?= /lib/modules/$(shell uname -r)/build

# Current working directory
PWD := $(shell pwd)

# Compiler flags
EXTRA_CFLAGS += -Wall -Wextra $(DEBFLAGS)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

install:
	$(MAKE) -C $(KDIR) M=$(PWD) modules_install

help:
	$(MAKE) -C $(KDIR) M=$(PWD) help

.PHONY: all clean install help
