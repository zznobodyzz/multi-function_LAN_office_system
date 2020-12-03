#include"server_head.h"

void Print_server(void)
{
		system("clear");
		printf("****************************\n");
		printf("管理员选项:                *\n");
		printf("1.用户管理                 *\n");
		printf("2.文档管理                 *\n");
		printf("3.清空服务器缓存           *\n");
		printf("4.修改推送内容             *\n");
		printf("5.向所有在线用户发送广播   *\n");
		printf("6.统计用户评分             *\n");
		printf("                           *\n");
		printf("                           *\n");
		printf("9.服务器配置               *\n");
		printf("0.关闭服务器               *\n");
		printf("           designed by zdc *\n");
		printf("****************************\n");
}

void Print_client(void)
{
		system("clear");
		printf("****************************\n");
		printf("1.查看当前服务器在线列表   *\n");
		printf("2.查看聊天室在线列表       *\n");
		printf("3.将用户踢出聊天室         *\n");
		printf("4.修改用户等级             *\n");
		printf("5.查看用户搜索缓存         *\n");
		printf("6.查看用户聊天概要         *\n");
		printf("7.查看用户登录登出信息     *\n");
		printf("0.返回                     *\n");
		printf("****************************\n");
}

void Print_client_next(void)
{
		system("clear");
		printf("****************************\n");
		printf("1.所有用户                 *\n");
		printf("2.所有在线用户             *\n");
		printf("0.返回                     *\n");
		printf("****************************\n");
}

void Print_client_next_next(void)
{
		system("clear");
		printf("****************************\n");
		printf("1.查看用户名列表           *\n");
		printf("2.查看详细信息             *\n");
		printf("0.返回                     *\n");
		printf("****************************\n");
}

void Print_client_historySS(void)
{
		system("clear");
		printf("****************************\n");
		printf("1.按用户名查看             *\n");
		printf("2.按单词查看               *\n");
		printf("3.按时间段查看             *\n");
		printf("4.查看总体                 *\n");
		printf("0.返回                     *\n");
		printf("****************************\n");
}

void Print_client_history(void)
{
		system("clear");
		printf("****************************\n");
		printf("1.按用户名查看             *\n");
		printf("2.按时间段查看             *\n");
		printf("3.查看总体                 *\n");
		printf("0.返回                     *\n");
		printf("****************************\n");
}

void Print_document(void)
{
		system("clear");
		printf("****************************\n");
		printf("1.文档浏览                 *\n");
		printf("2.搜索文档                 *\n");
		printf("3.删除文档                 *\n");
		printf("0.返回                     *\n");
		printf("****************************\n");
}

void Print_document_search(void)
{
		system("clear");
		printf("****************************\n");
		printf("1.根据文档名搜索           *\n");
		printf("2.根据作者搜索             *\n");
		printf("3.根据上传时间搜索         *\n");
		printf("0.返回                     *\n");
		printf("****************************\n");
}

void Print_search_doc(void)
{
		system("clear");
		printf("****************************\n");
		printf("1.精确搜素                 *\n");
		printf("2.模糊搜索                 *\n");
		printf("0.返回                     *\n");
		printf("****************************\n");
}

void Print_server_config(void)
{
		system("clear");
		printf("****************************\n");
		printf("1.修改默认文档路径         *\n");
		printf("0.返回                     *\n");
		printf("****************************\n");
}