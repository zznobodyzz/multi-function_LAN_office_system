#include"server_head.h"

void * Server(void * arg)
{
		int choose;
		int flag=0;
		while(1)
		{
				flag=0;
				Print_server();
				scanf("%d",&choose);
				getchar();
				Print_server();
				switch(choose)
				{
						case 1: while(1)
								{
										Print_client();
										scanf("%d",&choose);
										getchar();
										switch(choose)
										{
												case 1: Print_online_client(1);
														break;
												case 2: Print_online_client(2);
														break;
												case 3: Kick_chat();
														break;
												case 4: Change_client_mod();
														break;
												case 5: View_search_history();
														break;
												case 6: View_history(1);
														break;
												case 7: View_history(2);
														break;
												case 0: flag=1;
														break;
												default:printf("请输入有效数字\n");
														break;
										}
										if(flag==1)
												break;
										printf("请按回车键继续\n");
										getchar();
								}
								break;
						case 2: while(1)
								{
										scanf("%d",&choose);
										getchar();
										Print_document();
										switch(choose)
										{
												case 1: View_document();
														break;
												case 2: Search_document(1);
														break;
												case 3: Search_document(2);
														break;
												case 0: flag=1;
														break;
												default:printf("请输入有效数字\n");
														break;
										}
										if(flag==1)
												break;
								}
								break;
						case 3: Delete_cache();
								break;
						case 4: Change_adv();
								break;
						case 5: Broadcast();
								break;
						case 6: Sco_count();
								break;
						case 9:	while(1)
								{
										Print_server_config();
										scanf("%d",&choose);
										getchar();
										switch(choose)
										{
												case 1: Change_docpath();
														break;
												case 0: flag=1;
														break;
												default:printf("请输入有效数字\n");
														break;
										}
										if(flag==1)
												break;
										printf("请按回车键继续1\n");
										getchar();
								}
								break;
						case 0: Exit_server();
								break;
						default:printf("请输入有效数字\n");
								break;
				}
				if(flag!=1)
				{
						printf("请按回车键继续\n");
						getchar();
				}
		}
}

void * Fun(void * arg)
{
		pthread_detach(pthread_self());
		printf("请按回车键打开管理员菜单\n");
		getchar();
		pthread_t tser;
		pthread_create(&tser,NULL,Server,NULL);
		pthread_exit(NULL);
}