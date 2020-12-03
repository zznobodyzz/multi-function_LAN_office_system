OBJS=main.c menu.c database.c network.c login_out.c receive.c diction_func.c\
		file_func.c chat_func.c game_func.c client_func.c vip_func.c\
		score.c workqueue.c snake.c wuziqi.c tetris.c 
all:
	@$(CC) -I./include -o client $(OBJS) $(CFLAGS)	
	@mv client ../out
	@echo
	@echo "!!!!!client is ready in dir <out>!!!!!"
	@echo
