INCLUDE_FLAGS += -I$(O_DIR)/_public
MAINS += ft_ping
OBJ_DIR_TREE += $(O_DIR)/srcs/main/ $(O_DIR)/srcs/ $(O_DIR)/
O_FILES += $(O_DIR)/srcs/main/main.o
PUBLIC_LINKS +=

ft_ping: $(O_FILES)

# module ft_ping
$(O_DIR)/srcs/main/main.o: srcs/main/main.c

# public links
