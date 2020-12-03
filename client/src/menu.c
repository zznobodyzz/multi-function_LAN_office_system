#include"client_head.h"
void Title(void)
{
		system("clear");
		printf("     欢迎使用星尘办公软件\n");
		printf("Welcome to use STARDUST office\n");
		printf("注意:同一个ip只能启动一个客户端,请勿重复启动\n");
		printf("请保证网络通畅\n");
}
void Menu(void)
{
		system("clear");
		printf("------------------------------------------\n");
		printf("|注意：部分功能需要高级权限              |\n");
		printf("|1.单词功能                              |\n");
		printf("|2.文档功能                              |\n");
		printf("|3.聊天功能                              |\n");
		printf("|4.小游戏                                |\n");
		printf("|5.用户选项                              |\n");
		printf("|6.特权用户选项                          |\n");
		printf("|7.注销                                  |\n");
		printf("|8.给软件打分                            |\n");
		printf("|0.退出                                  |\n");
		printf("|                       更多功能敬请期待 |\n");
		printf("|                        designed by zdc |\n");
		printf("------------------------------------------\n");
}

void Dic_menu(void)
{
		system("clear");
		printf("------------------------------------------\n");
		printf("|1.单词查询                              |\n");
		printf("|2.历史记录查询                          |\n");
		printf("|0.返回主菜单                            |\n");
		printf("------------------------------------------\n");
}

void File_menu(void)
{
		system("clear");
		printf("------------------------------------------\n");
		printf("|1.文档一览                              |\n");
		printf("|2.搜索文档                              |\n");
		printf("|3.历史记录查询                          |\n");
		printf("|4.下载文档                              |\n");
		printf("|5.上传文档                              |\n");
		printf("|6.删除已下载文档                        |\n");
		printf("|7.取消已上传文档                        |\n");
		printf("|8.更改下载上传默认路径                  |\n");
		printf("|0.返回主菜单                            |\n");
		printf("------------------------------------------\n");
}

void Searchdoc_menu(void)
{
		system("clear");
		printf("------------------------------------------\n");
		printf("|1.按文档名搜索                          |\n");
		printf("|2.按上传者名搜索                        |\n");
		printf("|3.按上传时间搜索                        |\n");
		printf("|0.返回上一级                            |\n");
		printf("------------------------------------------\n");
}

void Print_searchdoc(void)
{
		system("clear");
		printf("------------------------------------------\n");
		printf("|1.精确搜素                              |\n");
		printf("|2.模糊搜索                              |\n");
		printf("|0.返回上一级                            |\n");
		printf("------------------------------------------\n");
}

void Downloaddoc_menu(void)
{
		system("clear");
		printf("------------------------------------------\n");
		printf("|1.通过文档名下载                        |\n");
		printf("|2.查看下载记录                          |\n");
		printf("|0.返回上一级                            |\n");
		printf("------------------------------------------\n");
}

void Uploaddoc_menu(void)
{
		system("clear");
		printf("------------------------------------------\n");
		printf("|1.通过文档名上传                        |\n");
		printf("|2.查看上传记录                          |\n");
		printf("|0.返回上一级                            |\n");
		printf("------------------------------------------\n");
}

void Chat_enter(void)
{
		system("clear");
		printf("------------------------------------------\n");
		printf("|1.进入聊天室                            |\n");
		printf("|2.退出聊天室                            |\n");
		printf("|0.返回主菜单                            |\n");
		printf("------------------------------------------\n");
}

void Chat_menu(void)
{
		system("clear");
		printf("------------------------------------------\n");
		printf("|             欢迎来到聊天室             |\n");
		printf("|1.查看聊天室用户列表                    |\n");
		printf("|2.群聊                                  |\n");
		printf("|3.私聊                                  |\n");
		printf("|4.聊天记录查询                          |\n");
		printf("|5.清空聊天记录                          |\n");
		printf("|6.找回聊天记录                          |\n");
		printf("|7.举报不良聊天行为                      |\n");
		printf("|8.直接返回主菜单(仍能收到消息)          |\n");
		printf("|0.退出聊天并返回主菜单                  |\n");
		printf("------------------------------------------\n");
}

void Print_chat_head(void)
{
		system("clear");
		printf("******************************************\n");
		printf("*您已开始聊天,请遵守网络道德规范,文明聊天*\n");
		printf("****           祝您聊天愉快           ****\n");
		printf("******************************************\n");
}

void Chat_history_choose(void)
{
		system("clear");
		printf("------------------------------------------\n");
		printf("|1.查看群聊记录                          |\n");
		printf("|2.查看私聊记录                          |\n");
		printf("|3.查看所有记录                          |\n");
		printf("|0.返回上一级                            |\n");
		printf("------------------------------------------\n");
}

void Game_menu(void)
{
		system("clear");
		printf("------------------------------------------\n");
		printf("|1.五子棋                                |\n");
		printf("|2.俄罗斯方块                            |\n");
		printf("|3.贪吃蛇                                |\n");
		printf("|0.返回上一级                            |\n");
		printf("------------------------------------------\n");
}

void Game_menu2(void)
{
		system("clear");
		printf("------------------------------------------\n");
		printf("|1.开始单人游戏                          |\n");
		printf("|2.查看排行榜                            |\n");
		printf("|3.好友PK                                |\n");
		printf("|0.返回上一级                            |\n");
		printf("------------------------------------------\n");
}

void Game_menu3(void)
{
		system("clear");
		printf("------------------------------------------\n");
		printf("|1.开始游戏                              |\n");
		printf("|2.查看排行榜                            |\n");
		printf("|0.返回上一级                            |\n");
		printf("------------------------------------------\n");
}

void Client_menu(void)
{
		system("clear");
		printf("------------------------------------------\n");
		printf("|1.修改密码                              |\n");
		printf("|2.查看用户列表                          |\n");
		printf("|3.打开/关闭管理员推送消息               |\n");
		printf("|4.降低/升高权限(需要管理员密钥)         |\n");
		printf("|0.返回主菜单                            |\n");
		printf("------------------------------------------\n");
}

void Super_menu(void)
{
		system("clear");
		printf("------------------------------------------\n");
		printf("|1.私人空间                              |\n");
		printf("|2.打开/关闭下载加速 	                 |\n");
		printf("|3.特别关心(上下线提醒)                  |\n");
		printf("|0.返回主菜单                            |\n");
		printf("------------------------------------------\n");
}

void Focus_menu(void)
{
		system("clear");
		printf("------------------------------------------\n");
		printf("|1.设置特别关心名单                      |\n");
		printf("|2.打开/关闭特别关心 	                 |\n");
		printf("|0.返回主菜单                            |\n");
		printf("------------------------------------------\n");
}

void Private_menu(void)
{
		system("clear");
		printf("------------------------------------------\n");
		printf("|1.查看私人空间                          |\n");
		printf("|2.下载私人空间文档                      |\n");
		printf("|3.上传文档至私人空间                    |\n");
		printf("|4.取消已上传的文档                      |\n");
		printf("|0.返回主菜单                            |\n");
		printf("------------------------------------------\n");
}
