#ifndef _CKJSON_CONTROL_H_
#define _CKJSON_CONTROL_H_



#include "KJsonValue.h"
#include <vector>

namespace KJson
{	
	class CKJsonControl
	{
	private:
		enum keywork
		{
			//匹配符
			L_braces,						//左大括号
			R_braces,						//右大括号
			L_bracket,						//左中括号
			R_bracket,						//右中括号
			//分隔符
			Comma,							//逗号
			Colon,							//冒号
			//Json内容
			String,							//字符串
			Integer,						//数字
			Boolean,						//逻辑值
			Null,							//Null
			Object,							//对象
			Array,                          //数组
			//错误标识
			Error,							//错误单词
		};
		
		class CToken
		{
		public:
			CToken(keywork vartype, std::string var, int line);
			keywork nGetVarType() const;
			void ChangeVarType(keywork vartype);
			const std::string& strGetVar() const;
			int nGetLine() const;
		private:
			keywork m_vartype;
			std::string m_var;
			int m_line;
		};
		
		class CErrorToken
		{
		public:
			CErrorToken(std::string strvar, int line);
			std::string strGetVar() const;
			int GetLine() const;
		private:
			std::string m_strvar;
			int m_line;
		};
	
	public:
		CKJsonControl();
		~CKJsonControl();
		int Load(const std::string strJsonValue);
		int LoadFile(const std::string strJsonFile);
		bool HasCache() const;
		void ClearCache();
		KJsonValueBase* GetKJsonValueBase();
		void outputtoken();
		
	private:
		bool m_hasCache;
		void DealWithLine(const std::string& strLine, int nLine);
		void SyntaxAnalyze();
		std::vector<CToken*> m_Token;
		std::vector<CErrorToken*> m_ErrorList;
	};
}

#endif // !_CKJSON_CONTROL_H_