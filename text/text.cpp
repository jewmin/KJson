#include <iostream>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include "../include/KJson.h"
using namespace std;



void TextKJson();
void TextKJsonObject();

int main(int argc, char** argv)
{
	cout<<"---------------------------------------------------------"<<endl;
	TextKJson();
	TextKJsonObject();
	return 0;
}


void TextKJson()
{
	KJson::CKJsonControl json;
	int nErrorNum = 0, nTextNum = 0;
	DIR* dir_fail, *dir_pass;
	struct dirent* ptr_fail, *ptr_pass;
	
	//错误Json测试
	string fail_path("./text_json_file/text_json_fail");
	dir_fail = opendir(fail_path.c_str());
	while ((ptr_fail = readdir(dir_fail)) != NULL)
	{
		if (0 == strcmp(ptr_fail->d_name, ".") || 0 == strcmp(ptr_fail->d_name, ".."))
			continue;
		nTextNum++;
		if( 0 == json.LoadFile(fail_path + "/" + ptr_fail->d_name))
		{
			cout<<"文件 \""<<ptr_fail->d_name<<"\" 测试错误"<<endl;
			nErrorNum++;
		}
		else
		{
			cout<<"文件 \""<<ptr_fail->d_name<<"\" 测试通过"<<endl;
		}
	}
	closedir(dir_fail);
	
	
	//正确json测试	
	string pass_path("./text_json_file/text_json_pass");
	dir_pass = opendir(pass_path.c_str());
	while ((ptr_pass = readdir(dir_pass)) != NULL)
	{
		if (0 == strcmp(ptr_pass->d_name, ".") || 0 == strcmp(ptr_pass->d_name, ".."))
			continue;
		nTextNum++;
		if( 0 != json.LoadFile(pass_path + "/" + ptr_pass->d_name))
		{
			cout<<"文件 \""<<ptr_pass->d_name<<"\" 测试错误"<<endl;
			json.outputtoken();
			nErrorNum++;
		}
		else
		{
			cout<<"文件 \""<<ptr_pass->d_name<<"\" 测试通过"<<endl;
		}
	}
	closedir(dir_pass);
	
	
	//测试完毕
	if(0 == nErrorNum)
	{
		cout<<"K_Json测试全部通过，共测试了"<<nTextNum<<"个文件。"<<endl;
	}
	else
	{
		cout<<"K_Json测试未能通过，共测试了"<<nTextNum<<"个文件，其中有"<<nErrorNum<<"个文件测试错误。"<<endl;
	}
	
	cout<<"---------------------------------------------------------"<<endl;
}


void TextKJsonObject()
{
	KJson::CKJsonControl json;
	int result = json.LoadFile("./text_json_file/text_json_pass/pass4_object_text.json");
	if( 0 != result)
	{
		cout<<"pass4_object_text对象测试失败,json解析出错"<<endl;
	}
	else
	{
		KJson::KJsonObject* obj = static_cast<KJson::KJsonObject*>(json.GetKJsonValueBase());
		if(NULL == obj)
		{
			cout<<"pass4_object_text对象测试失败,导出对象为空"<<endl;
		}
		else
		{
			obj->strGetJsonString();
			cout<<"pass4_object_text对象测试通过"<<endl;
			delete obj;
		}
	}
	
	cout<<"---------------------------------------------------------"<<endl;
}
