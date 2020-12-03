OBJS=main.c menu.c database.c init.c network.c admin_main.c admin_config.c admin_cache.c \
		admin_score.c admin_exit.c admin_file.c admin_user.c admin_sendmsg.c login_out.c \
		service.c diction_func.c document_func.c chat_func.c client_func.c game_func.c \
		wuziqi_pk.c vip_func.c 
all:
	@$(CC) -I./include -o server $(OBJS) $(CFLAGS)
	@mv server ../out
	@echo
	@echo "!!!!!server is ready in dir <out>!!!!!"
	@echo
