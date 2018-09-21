#
# By default, the build is done against the running linux kernel source.
# To build against a different kernel source tree, set SYSSRC:
#
#    make SYSSRC=/path/to/kernel/source

ifdef SYSSRC
 KERNEL_SOURCES	 = $(SYSSRC) $(SYSSRC)/src
else
 KERNEL_UNAME	:= $(shell uname -r)
 KERNEL_SOURCES	 = /lib/modules/$(KERNEL_UNAME)/build
endif


default: modules
.PHONY: default
install: modules_install
	cp processor_container-blacklist.conf /etc/modprobe.d
#	cp 80-processor_container.rules /etc/udev/rules.d
	mkdir -p /usr/local/include/processor_container/
	cp include/processor_container.h /usr/local/include/processor_container/
	update-initramfs -u
#	cp processor_container.ko /lib/modules/$(KERNEL_UNAME)/

.PHONY: install


.PHONY:


%::
	$(MAKE) -C $(KERNEL_SOURCES) \
        M=$$PWD $@
