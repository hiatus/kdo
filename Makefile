MODULE := kdo.ko
SRCDIR := src
KDIR   ?= /lib/modules/$(shell uname -r)/build


all: $(MODULE)

$(MODULE): $(addprefix $(SRCDIR)/, main.c cdev.c include/cdev.h include/exec.h)
	@make --no-print-directory -C $(KDIR) M=$(shell pwd)/$(SRCDIR)
	@cp $(SRCDIR)/$(MODULE) $(MODULE)

clean:
	@echo [rm] $(MODULE)
	@rm $(MODULE)
	@make --no-print-directory -C $(KDIR) M=$(shell pwd)/$(SRCDIR) clean


.PHONY: clean
