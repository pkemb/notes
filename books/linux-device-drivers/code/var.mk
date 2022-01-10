# second run, in kernel dir
obj-m := $(KO_NAME).o
$(KO_NAME)-objs := $(MODULE_NAME).o

EXTRA_CFLAGS += -DMODULE_NAME=\"$(MODULE_NAME)\"
EXTRA_CFLAGS += -DDEBUG