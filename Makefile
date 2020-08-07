#includes

include config/config.mk

include $(ARCH_DIR)/$(ARCH)/Makefile
include $(KERNEL_DIR)/Makefile
include $(INIT_DIR)/Makefile

#files

OBJS := $(OBJS_ARCH) $(OBJS_INIT)

#rules
all: $(NAME)

install:
	sh $(SCRIPTS_DIR)/img-create.sh
	sh $(SCRIPTS_DIR)/kvm-start.sh

$(NAME): $(OBJS)
	$(LD) $(LDFLAGS) -o $(NAME) $(OBJS)

clean:
	rm -rf $(OBJS)

fclean:
	rm -rf $(OBJS)
	rm -f $(NAME)
#	rm -rf $(DEST_DIR)

re: fclean all

.PHONY: clean fclean re all install