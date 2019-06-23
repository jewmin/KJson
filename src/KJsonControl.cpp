#include "../include/KJsonControl.h"
#include <sstream>
#include <fstream>
#include <stack>
#include <iostream>


namespace KJson
{
	CKJsonControl::CToken::CToken(keywork vartype, std::string var, int line) : 
		m_vartype(vartype),
		m_var(var),
		m_line(line)
	{
	}
	
	
	CKJsonControl::keywork CKJsonControl::CToken::nGetVarType() const 
	{  
		return m_vartype;
	}
	
	
	void CKJsonControl::CToken::ChangeVarType(keywork vartype)
	{
		m_vartype = vartype;
	}
	
	
	const std::string& CKJsonControl::CToken::strGetVar() const
	{
		return m_var;
	}
	
	
	int CKJsonControl::CToken::nGetLine() const
	{
		return m_line;
	}
	
	
	CKJsonControl::CErrorToken::CErrorToken(std::string strvar, int line) : 
		m_strvar(strvar),
		m_line(line)
	{
	}
	
	
	std::string CKJsonControl::CErrorToken::strGetVar() const
	{
		return m_strvar;
	}
	
	
	int CKJsonControl::CErrorToken::GetLine() const
	{
		return m_line;
	}
	
	
	CKJsonControl::CKJsonControl()
	{
		ClearCache();
	}
	
	
	CKJsonControl::~CKJsonControl()
	{
		ClearCache();
	}
	
	
	int CKJsonControl::Load(const std::string strJsonValue)
	{
		if(HasCache())
		{
			ClearCache();
		}
		std::string buff;
		std::istringstream _LoadJson(strJsonValue);
		int line = 1;
		while(getline(_LoadJson, buff))
		{
			DealWithLine(buff + '\n', line);
			buff.clear();
			line++;
		}
		int _es = m_ErrorList.size();
		if( 0 == _es)
		{
			SyntaxAnalyze();
			_es = m_ErrorList.size();
		}
		return _es;
	}
	
	
	int CKJsonControl::LoadFile(const std::string strJsonFile)
	{
		if(HasCache())
		{
			ClearCache();
		}
		std::string buff;
		std::ifstream _LoadJsonFile(strJsonFile.c_str());
		int line = 1;
		while(getline(_LoadJsonFile, buff))
		{
			DealWithLine(buff + '\n', line);
			buff.clear();
			line++;
		}
		int _es = m_ErrorList.size();
		if( 0 == _es)
		{
			SyntaxAnalyze();
			_es = m_ErrorList.size();
		}
		return _es;
	}
	
	
	bool CKJsonControl::HasCache() const
	{
		return m_hasCache;
	}
	
	
	void CKJsonControl::ClearCache()
	{
		int length = m_Token.size();
		for(int i = 0 ; i < length ; i++)
		{
			delete m_Token[i];
		}
		length = m_ErrorList.size();
		for(int i = 0 ; i < length ; i++)
		{
			delete m_ErrorList[i];
		}
		m_Token.clear();
		m_ErrorList.clear();
		m_hasCache = false;
	}
	
	
	void CKJsonControl::DealWithLine(const std::string& strLine, int nLine)
	{
		m_hasCache = true;
		int length = strLine.size();
		int pos = 0;
		CToken *token;
		std::string _temp;
		bool _temp_over = false;
		while(pos < length)
		{
			switch(strLine[pos])
			{
			case '{' :
				token = new CToken(L_braces, "{", nLine);
				_temp_over = true;
				pos++;
				break;
			case '}' :
				token = new CToken(R_braces, "}", nLine);
				_temp_over = true;
				pos++;
				break;
			case '[' :
				token = new CToken(L_bracket, "[", nLine);
				_temp_over = true;
				pos++;
				break;
			case ']' :
				token = new CToken(R_bracket, "]", nLine);
				_temp_over = true;
				pos++;
				break;
			case ',':
				token = new CToken(Comma, ",", nLine);
				_temp_over = true;
				pos++;
				break;
			case ':' :
				token = new CToken(Colon, ":", nLine);
				_temp_over = true;
				pos++;
				break;
			case '"' : 
				if(!_temp.empty())
				{
					//缓存字符不为空
					//先跳出处理清空字符_temp
					//pos不自增即可
					_temp_over = true;
					break;
				}
				else
				{
					pos++;
					while(pos < length)
					{
						if('"' == strLine[pos])
						{
							//读取到正常结束
							token = new CToken(String, _temp, nLine);
							break;
						}
						else if( '\\' == strLine[pos])
						{
							//如果读到转义符'\\'，则识别下一位转义
							//同时在转义状态下不能再次转义
							pos++;
							if(
								'b' == strLine[pos] || 
								'f' == strLine[pos] ||
								'n' == strLine[pos] ||
								'r' == strLine[pos] ||
								't' == strLine[pos] ||
								'"' == strLine[pos] ||
								'/' == strLine[pos] ||
								'|' == strLine[pos] ||
								'\\' == strLine[pos]
								)
							{
								//允许的转义
								_temp.push_back(strLine[pos]);
								pos++;
							}
							else if(
								'u' == strLine[pos]
							)
							{
								//允许的转义
								_temp.push_back('\\');
								_temp.push_back(strLine[pos]);
								pos++;
							}
							else
							{
								break;
							}
						}
						else if(
							'\b' == strLine[pos] || 
							'\f' == strLine[pos] ||
							'\n' == strLine[pos] ||
							'\r' == strLine[pos] ||
							'\t' == strLine[pos] ||
							'\"' == strLine[pos] ||
							'\\' == strLine[pos]
							)
						{
							//不能出现直接以此形式出现的控制转义符
							break;
						}
						else
						{
							_temp.push_back(strLine[pos]);
							pos++;
						}
					}
					//遍历完还未读取到结束符
					//则视为错误
					//字符串不允许跨行
					if(NULL == token)
					{
						// token = new CToken(Error, _temp, nLine);
						m_ErrorList.push_back(new CErrorToken(strLine, nLine));
					}
					_temp.clear();
					_temp_over = true;
					pos++;
					break;
				}
			case ' ':
			case '\n':
			case '\t':
			case '\r':
				//处理空格、回车、制表符和换行符
				_temp_over = true;
				pos++;
				break;
			default:
				//处理0-32的控制符
				// if( 0 <= strLine[pos] && 32 >= strLine[pos])
				// {
				// 	_temp_over = true;
				// 	pos++;
				// 	break;
				// }
				//剩下Boolean或者Number或者Error或者Null
				//开启缓存模式
				_temp_over = false;
				_temp.push_back(strLine[pos]);
				pos++;
				break;
			}
			//处理temp字符串
			if(!_temp.empty() && _temp_over)
			{
				//分析_temp并解析分类Boolean或者Number或者Error或者Null
				int _temp_length = _temp.size();
				if( ( '0' <= _temp[0] && '9' >= _temp[0] ) || '-' == _temp[0])
				{
					//若为数字开头(除了0)或者是"-"号开头
					//需要检测往后是否全为数字或者为科学计数
					//若否则为Error
					bool isNum = true;
					bool has_E = false;
					bool has_pos = false;
					int n_pos = 0;
					//判断第一个
					if(( '0' < _temp[n_pos] && '9' >= _temp[n_pos] ))
					{
						//第一位为数字且不为0
						n_pos++;
					}
					else if( '0' == _temp[n_pos])
					{
						//第一位为数字且为0
						n_pos++;
						if( n_pos < _temp_length && '.' != _temp[n_pos])
						{
							//第二位不为"."
							isNum = false;
						}
						n_pos++;
					}
					else
					{
						//第一位为符号位"-"
						n_pos++;
						if(( '0' < _temp[n_pos] && '9' >= _temp[n_pos] ))
						{
							//第二位非0
							n_pos++;
						}
						else if( '0' == _temp[n_pos])
						{
							//第二位为0
							n_pos++;
							if( n_pos < _temp_length && '.' != _temp[n_pos])
							{
								//第三位不为"."
								isNum = false;
							}
							n_pos++;
						}
						else
						{
							isNum = false;
						}
					}
					
					while(isNum && n_pos < _temp_length)
					{
						//判断全数字或科学计数
						if('e' == _temp[n_pos] || 'E' == _temp[n_pos])
						{
							//解析到e或E
							//不管大小写，转成小写
							_temp[n_pos] = 'e';
							if( !has_E)
							{
								//如果从未出现过e，则视为第一次出现
								has_E = true;
								//解析后面一位
								//可以为数字或者符号，但必须存在
								n_pos++;
								if(n_pos < _temp_length)
								{
									if( '+' == _temp[n_pos] || '-' == _temp[n_pos])
									{
										//符合，后接符号
									}
									else if('0' <= _temp[n_pos] && '9' >=_temp[n_pos])
									{
										//符合，后接0-9的数字
									}
									else
									{
										isNum = false;
									}
								}
								else
								{
									isNum = false;
								}
							}
							else
							{
								isNum = false;
							}
						}
						else if ('.' == _temp[n_pos])
						{
							//解析到"."
							//判断是否已存在"."
							//若已存在则记错，否则通过。
							//不必考虑e后面，因为后面不允许为小数
							if(has_pos)
							{
								//若存在且再次匹配到，则记错
								isNum = false;
							}
						}
						else if(! ('0' <= _temp[n_pos] && '9' >=_temp[n_pos]))
						{
							//若除了"."和"e"和"E"外还匹配不到数字，则记错
							isNum = false;
						}
						n_pos++;
					}
					if(isNum)
					{
						//若为true，则是数字
						m_Token.push_back(new CToken(Integer, _temp, nLine));
					}
					else
					{
						//若不是，则为Error
						// m_Token.push_back(new CToken(Error, _temp, nLine));
						m_ErrorList.push_back(new CErrorToken(strLine, nLine));
					}
				}
				else if( 4 == _temp_length)
				{
					//可能为null或者true
					if('n' == _temp[0])
					{
						if(
						'u' == _temp[1] &&
						'l' == _temp[2] &&
						'l' == _temp[3]
						)
						{
							m_Token.push_back(new CToken(Null, _temp, nLine));
						}
					}
					else if('t' == _temp[0])
					{
						if(
						'r' == _temp[1] &&
						'u' == _temp[2] &&
						'e' == _temp[3]
						)
						{
							m_Token.push_back(new CToken(Boolean, _temp, nLine));
						}
					}
					else
					{
						//两者都不是
						//则为Error
						// m_Token.push_back(new CToken(Error, _temp, nLine));
						m_ErrorList.push_back(new CErrorToken(strLine, nLine));
					}
				}
				else if(5 == _temp_length)
				{
					//可能为false
					if(
						'f' == _temp[0] && 
						'a' == _temp[1] && 
						'l' == _temp[2] && 
						's' == _temp[3] && 
						'e' == _temp[4]
						)
					{
						m_Token.push_back(new CToken(Boolean, _temp, nLine));
					}
					else
					{
						//若不是则为Error
						// m_Token.push_back(new CToken(Error, _temp, nLine));
						m_ErrorList.push_back(new CErrorToken(strLine, nLine));
					}
				}
				else
				{
					//非数字或符号"-"开头且长度不为特殊值的
					//必为Error
					// m_Token.push_back(new CToken(Error, _temp, nLine));
					m_ErrorList.push_back(new CErrorToken(strLine, nLine));
				}
				
				_temp_over = false;
				_temp.clear();
			}
			if(NULL != token)
			{
				m_Token.push_back(token);
				token = NULL;
			}
		}
	}
	
	
	void CKJsonControl::SyntaxAnalyze()
	{	
		std::vector<CToken*>::iterator vector_it;
		std::stack<CToken*> stack, stack_temp;
		bool _hasVector = false;
		bool _overVector = false;
		CToken* VectorBegin = NULL;
		
		for(vector_it = m_Token.begin() ; vector_it != m_Token.end(); vector_it++)
		{
			if(_overVector)
			{
				//若主集合已结束
				//说明均为多余Token
				//直接压error
				stack.push(*vector_it);
				(*vector_it)->ChangeVarType(Error);
				m_ErrorList.push_back(
					new CErrorToken(
						(*vector_it)->strGetVar() + " : Surplus token outside the main object.",
						 (*vector_it)->nGetLine()
						 )
					);
			}
			//遍历Token列表并进行语法分析
			if(
				String == (*vector_it)->nGetVarType()    ||
				Integer == (*vector_it)->nGetVarType()   ||
				Boolean == (*vector_it)->nGetVarType()   ||
				Null == (*vector_it)->nGetVarType()
			)
			{
				//匹配到String, Integer, Boolean, Null
				//判断是否集合（对象、数组）
				//情况一、存在集合但在此之前找不到"{"标志，说明在主集合后，错误
				//       但此情况已在遍历时处理，故不需考虑
				//情况二、存在集合且在此前能找到"{"标志，说明字符合法直接压入
				//情况三、不存在集合，直接压入
				if(!_hasVector)
				{
					//第三种情况，不存在集合
					//直接压入但若后面还有数据，则整个CToken都为错误
					_overVector = true;
				}
				//第二种情况与第三种情况均直接入栈
				stack.push(*vector_it);
			}
			else if( 
				L_braces == (*vector_it)->nGetVarType()  ||
				L_bracket == (*vector_it)->nGetVarType()
				)
			{
				//匹配到左大括号，左中括号
				//直接压栈
				stack.push(*vector_it);
				//并置是否存在集合标志位为true
				if(!_hasVector)
				{
					//若在此前不存在集合
					//则设置标志位为true
					//并记录该位置以确定是否跳出
					_hasVector = true;
					VectorBegin = (*vector_it);
				}
			}
			else if(
				Comma == (*vector_it)->nGetVarType()     ||
				Colon == (*vector_it)->nGetVarType()
				)
			{
				//匹配到逗号，冒号
				//直接压栈
				stack.push(*vector_it);
			}
			else if( R_braces == (*vector_it)->nGetVarType())
			{
				//匹配"}"
				//往前查找最近的"{"并删除
				//由于为对象类型，若其中还存在词语，则进行语法分析
				//若找不到则压栈
				//先查找，若无再压栈，有就直接清除
				bool _isfind = false;
				CToken* temp = NULL;
				while(!stack.empty())
				{
					temp = stack.top();
					stack.pop();
					if(L_braces != temp->nGetVarType())
					{
						//若不是，则压入缓冲栈里
						stack_temp.push(temp);
					}
					else
					{
						stack.push(temp);
						//标志已找到
						//标示在跳出后不需要更改
						_isfind = true;
						//nt : 下一个位置需要匹配的内容
						//1-> key
						//2-> :
						//3-> value
						//4-> ,
						char nt = 1;
						if(VectorBegin == temp)
						{
							//若为主集合结束标志
							//标记主集合已结束
							_overVector = true;
						}
						//处理缓冲栈内容
						while(!stack_temp.empty())
						{
							temp = stack_temp.top();
							stack_temp.pop();
							if(1 == nt)
							{
								//需要匹配key
								if(String == temp->nGetVarType())
								{
									nt = 2;
									stack.push(temp);
								}
								else
								{
									//不符合key必须为string的规则
									//压回分析栈并标记为Error
									//且标记为从key开始匹配
									temp->ChangeVarType(Error);
									m_ErrorList.push_back(
										new CErrorToken(
											temp->strGetVar() + " : Error key type.",
											temp->nGetLine()
											)
										);
									stack.push(temp);
									nt = 1;
								}
							}
							else if( 2 == nt)
							{
								//需要匹配冒号
								if(Colon == temp->nGetVarType())
								{
									nt = 3;
									stack.push(temp);
								}
								else
								{
									//不符合分隔符必须为Colon的规则
									//压回分析栈并标记为Error
									//且标记为从key开始匹配
									temp->ChangeVarType(Error);
									m_ErrorList.push_back(
										new CErrorToken(
											temp->strGetVar() + " : Error Colon type.",
											temp->nGetLine()
											)
										);
									stack.push(temp);
									nt = 1;
								}
							}
							else if( 3 == nt)
							{
								//需要匹配value
								if(
									String == temp->nGetVarType() || 
									Integer == temp->nGetVarType() ||
									Boolean == temp->nGetVarType() ||
									Null == temp->nGetVarType() ||
									Object == temp->nGetVarType() ||
									Array == temp->nGetVarType()
									)
								{
									nt = 4;
									//把key和value弹出
									stack.pop();
									stack.pop();
									//如果为Object或者Array,需要手动释放空间
									//因为两者为标记作用，并不存在于m_Token中
									//不能通过CKJsonControl类析构时释放
									if(
										Object == temp->nGetVarType() ||
										Array == temp->nGetVarType()
										)
									{
										delete temp;
									}
								}
								else
								{
									//不符合value为String、Integer、Boolean、Null、
									//Object、Array的规则
									//压回分析栈并标记为Error
									//且标记为从key开始匹配
									temp->ChangeVarType(Error);
									m_ErrorList.push_back(
										new CErrorToken(
											temp->strGetVar() + " : Error value type.",
											temp->nGetLine()
											)
										);
									stack.push(temp);
									nt = 1;
								}
							}
							else if( 4 == nt)
							{
								//结束了一次匹配
								//然而却弹出了一个temp，
								//故该temp必须为Comma
								if(	Comma == temp->nGetVarType())
								{
									stack.push(temp);
									nt = 1;
								}
								else
								{
									//不符合分隔符为,的匹配规则
									//压回分析栈并标记为Error
									//且标记为从key开始匹配
									temp->ChangeVarType(Error);
									m_ErrorList.push_back(
										new CErrorToken(
											temp->strGetVar() + " : Error Comma type.",
											temp->nGetLine()
											)
										);
									stack.push(temp);
									nt = 1;
								}
							}
						}
						while(!stack.empty())
						{
							//把对象转换成CToken->Object
							//并处理多余的内容（即错误内容，标记为Error）
							temp = stack.top();
							stack.pop();
							if(L_braces == temp->nGetVarType())
							{
								//找到左大括号
								stack.push(new CToken(Object, "object", temp->nGetLine()));
								break;
							}
							else if(Comma != temp->nGetVarType())
							{
								//找到非左大括号以及非逗号的内容
								temp->ChangeVarType(Error);
								m_ErrorList.push_back(
									new CErrorToken(
										(*vector_it)->strGetVar() + " ： Surplus token in object var.",
										(*vector_it)->nGetLine()
										)
									);
							}
							else
							{
								if( 4 != nt)
								{
									//若不是恰好结束，则说明是以逗号结束，没有key
									//标记为多了一个逗号
									temp->ChangeVarType(Error);
									m_ErrorList.push_back(
										new CErrorToken(
											(*vector_it)->strGetVar() + " : More token in object var.",
											(*vector_it)->nGetLine()
											)
										);
									nt = 4;
								}
							}
						}
						break;
					}					
				}
				//若找不到
				if(!_isfind)
				{
					while(!stack_temp.empty())
					{
						//更改里面所有内容的类型为Error
						//并回压
						temp = stack_temp.top();
						stack_temp.pop();
						temp->ChangeVarType(Error);
						m_ErrorList.push_back(
							new CErrorToken(
								temp->strGetVar() + " : Surplus token in object var.",
								(*vector_it)->nGetLine()
								)
							);
						stack.push(temp);
					}
					//把匹配到的“}”压入并置为Error
					(*vector_it)->ChangeVarType(Error);
					stack.push(*vector_it);
					m_ErrorList.push_back(
							new CErrorToken(
								(*vector_it)->strGetVar() + " : Not found the matching braces.",
								(*vector_it)->nGetLine()
								)
							);
				}
			}
			else if( R_bracket == (*vector_it)->nGetVarType())
			{
				//匹配"]"
				//往前查找最近的"["并删除
				//若找不到则压栈
				//先查找，若无再压栈，有就直接清除
				bool _isfind = false;
				CToken* temp = NULL;
				while(!stack.empty())
				{
					temp = stack.top();
					stack.pop();
					if(L_bracket != temp->nGetVarType())
					{
						//若不是，则压入缓冲栈里
						stack_temp.push(temp);
					}
					else
					{
						stack.push(temp);
						//标志已找到
						//标示在跳出后不需要更改
						_isfind = true;
						//nt : 下一个位置需要匹配的内容
						//1-> var
						//2-> ,
						char nt = 1;
						if(VectorBegin == temp)
						{
							//若为主集合结束标志
							//标记主集合已结束
							_overVector = true;
						}
						//处理缓冲栈内容
						while(!stack_temp.empty())
						{
							temp = stack_temp.top();
							stack_temp.pop();
							if( 1 == nt)
							{
								//需要匹配var
								if(
									String == temp->nGetVarType() || 
									Integer == temp->nGetVarType() ||
									Boolean == temp->nGetVarType() ||
									Null == temp->nGetVarType() ||
									Object == temp->nGetVarType() ||
									Array == temp->nGetVarType()
									)
								{
									nt = 2;
									//如果为Object或者Array,需要手动释放空间
									//因为两者为标记作用，并不存在于m_Token中
									//不能通过CKJsonControl类析构时释放
									if(
										Object == temp->nGetVarType() ||
										Array == temp->nGetVarType()
										)
									{
										delete temp;
									}
								}
								else
								{
									//不符合value为String、Integer、Boolean、Null、
									//Object、Array的规则
									//压回分析栈并标记为Error
									//且标记为从key开始匹配
									temp->ChangeVarType(Error);
									m_ErrorList.push_back(
										new CErrorToken(
											temp->strGetVar() + " : Error value type.",
											temp->nGetLine()
											)
										);
									stack.push(temp);
									nt = 1;
								}
							}
							else if( 2 == nt)
							{
								//结束了一次匹配
								//然而却弹出了一个temp，
								//故该temp必须为Comma
								if(	Comma == temp->nGetVarType())
								{
									stack.push(temp);
									nt = 1;
								}
								else
								{
									//不符合分隔符为逗号的规则
									//压回分析栈并标记为Error
									//且标记为从key开始匹配
									temp->ChangeVarType(Error);
									m_ErrorList.push_back(
										new CErrorToken(
											temp->strGetVar() + " : Error Comma type.",
											temp->nGetLine()
											)
										);
									stack.push(temp);
									nt = 1;
								}
							}
						}
						while(!stack.empty())
						{
							//把对象转换成CToken->Object
							//并处理多余的内容（即错误内容，标记为Error）
							temp = stack.top();
							stack.pop();
							if(L_bracket == temp->nGetVarType())
							{
								//找到左中括号
								stack.push(new CToken(Array, "array", temp->nGetLine()));
								break;
							}
							else if(Comma != temp->nGetVarType())
							{
								//找到非左中括号以及非逗号的内容
								temp->ChangeVarType(Error);
								m_ErrorList.push_back(
									new CErrorToken(
										(*vector_it)->strGetVar() + " : Surplus token in array var.",
										(*vector_it)->nGetLine()
										)
									);
							}
							if( 2 != nt)
								{
									//若不是恰好结束，则说明是以逗号结束，没有key
									//标记为多了一个逗号
									temp->ChangeVarType(Error);
									m_ErrorList.push_back(
										new CErrorToken(
											(*vector_it)->strGetVar() + " : More token in array var.",
											(*vector_it)->nGetLine()
											)
										);
									nt = 2;
								}
						}
						break;
					}					
				}
				//若找不到
				if(!_isfind)
				{
					while(!stack_temp.empty())
					{
						//更改里面所有内容的类型为Error
						//并回压
						temp = stack_temp.top();
						stack_temp.pop();
						temp->ChangeVarType(Error);
						m_ErrorList.push_back(
							new CErrorToken(
								temp->strGetVar() + " : Bracket token in array var.",
								(*vector_it)->nGetLine()
								)
							);
						stack.push(temp);
					}
					//把匹配到的“]”压入并置为Error
					(*vector_it)->ChangeVarType(Error);
					stack.push(*vector_it);
					m_ErrorList.push_back(
							new CErrorToken(
								(*vector_it)->strGetVar() + " : Not found the matching Bracket.",
								(*vector_it)->nGetLine()
								)
							);
				}
			}
		}
		if(1 != stack.size())
		{
			//如果长度不为1
			m_ErrorList.push_back(
				new CErrorToken(
					"Json Format Error!",
					stack.top()->nGetLine()
					)
				);
		}
		//清空里面的Object对象和Array对象
		//因为此不受KJsonControl管理
		CToken* temp = NULL;
		while(0 != stack.size())
		{
			temp = stack.top();
			stack.pop();
			if(
				Object == temp->nGetVarType() ||
				Array == temp->nGetVarType()
				)
			{
				delete temp;
			}	
		}
		
	}
	
	
	KJsonValueBase* CKJsonControl::GetKJsonValueBase()
	{
		if( 0 != m_ErrorList.size())
		{
			//存在错误,返回空
			return NULL;
		}
		else
		{
			//如果完全正确,解析出对象
			KJsonValueBase *temp = NULL, *_ret = NULL, *_now = NULL;
			CToken *token = NULL, *key = NULL;
			std::stack<KJsonValueBase*> _stack;
			
			//处理token列表，按序生成kjosn对象
			for(std::vector<CToken*>::iterator it = m_Token.begin() ; 
				it != m_Token.end() ;it ++)
			{
				token = (*it);
				switch(token->nGetVarType())
				{
				case L_braces:
					_stack.push(_now);
					temp = KJsonObject::create();
					//加入上一层判断
					if( NULL != _stack.top())
					{
						if( Value_Object == _now->GetValueType())
						{
							//_now为对象
							(static_cast<KJsonObject*>(_now))
								->insert(key->strGetVar(), temp);
							key = NULL;
						}
						else
						{
							//_now为数组
							(static_cast<KJsonArray*>(_now))
								->insert(temp);
						}
					}
					else if(NULL == _ret)
					{
						//若_ret为空，则把这个obj指针给_ret;
						_ret = temp;
					}
					_now = temp;
					break;
				case R_braces:
					_now = _stack.top();
					_stack.pop();
					break;
				case L_bracket:
					_stack.push(_now);
					temp = KJsonArray::create();
					//加入上一层判断
					if( NULL != _stack.top())
					{
						if( Value_Object == _now->GetValueType())
						{
							//_now为对象
							(static_cast<KJsonObject*>(_now))
								->insert(key->strGetVar(), temp);
							key = NULL;
						}
						else
						{
							//_now为数组
							(static_cast<KJsonArray*>(_now))
								->insert(temp);
						}
					}
					else if(NULL == _ret)
					{
						//若_ret为空，则把这个obj指针给_ret;
						_ret = temp;
					}
					_now = temp;
					break;
				case R_bracket:
					_now = _stack.top();
					_stack.pop();
					break;
				case String:
					if(NULL == _ret)
					{
						_ret = KJsonString::create(token->strGetVar());
					}
					else if( Value_Object == _now->GetValueType())
					{
						if(NULL == key)
						{
							//_now为对象且不存在key
							key = token;
						}
						else
						{
							//_now为对象且存在Key
							(static_cast<KJsonObject*>(_now))
								->insert(
									key->strGetVar(), 
									KJsonString::create(token->strGetVar())
									);
							key = NULL;
						}
					}
					else
					{
						//_now为数组
						(static_cast<KJsonArray*>(_now))
							->insert(
								KJsonString::create(token->strGetVar())
								);
					}
					break;
				case Integer:
					if(NULL == _ret)
					{
						_ret = KJsonInteger::create(token->strGetVar());
					}
					else if( Value_Object == _now->GetValueType())
					{
						//_now为对象
						(static_cast<KJsonObject*>(_now))
							->insert(
								key->strGetVar(), 
								KJsonInteger::create(token->strGetVar())
								);
						key = NULL;
					}
					else
					{
						//_now为数组
						(static_cast<KJsonArray*>(_now))
							->insert(
								KJsonInteger::create(token->strGetVar())
								);
					}
					break;
				case Boolean:
					if(NULL == _ret)
					{
						KJsonBoolean::create(token->strGetVar());
						_ret = _now;
					}
					else if( Value_Object == _now->GetValueType())
					{
						//_now为对象
						(static_cast<KJsonObject*>(_now))
							->insert(
								key->strGetVar(), 
								KJsonBoolean::create(token->strGetVar())
								);
						key = NULL;
					}
					else
					{
						//_now为数组
						(static_cast<KJsonArray*>(_now))
							->insert(
								KJsonBoolean::create(token->strGetVar())
								);
					}
					break;
				case Null:
					if(NULL == _ret)
					{
						_ret = KJsonNull::create();
					}
					else if( Value_Object == _now->GetValueType())
					{
						//_now为对象
						(static_cast<KJsonObject*>(_now))
							->insert(
								key->strGetVar(), 
								KJsonNull::create()
								);
						key = NULL;
					}
					else
					{
						//_now为数组
						(static_cast<KJsonArray*>(_now))
							->insert(
								KJsonNull::create()
								);
					}
					break;
				default:
					break;
				}
			}
			//Token全部处理完
			//返回base对象
			return _ret;
		}
	}
	
	
	void CKJsonControl::outputtoken()
	{
		for(std::vector<CToken*>::iterator it = m_Token.begin() ; 
		it != m_Token.end() ;it ++)
		{
			int k = (*it)->nGetVarType();
			switch(k)
			{
			case L_braces:
				std::cout<<"左大括号---";
				break;
			case R_braces:
				std::cout<<"右大括号---";
				break;
			case L_bracket:
				std::cout<<"左中括号---";
				break;
			case R_bracket:
				std::cout<<"右中括号---";
				break;
			case Comma:
				std::cout<<"逗号------";
				break;
			case Colon:
				std::cout<<"冒号------";
				break;
			case String:
				std::cout<<"字符串-----";
				break;
			case Integer:
				std::cout<<"数字------";
				break;
			case Boolean:
				std::cout<<"逻辑值-----";
				break;
			case Null:
				std::cout<<"NULL------";
				break;
			}
			std::cout<<(*it)->strGetVar()<<std::endl;
		}
		std::cout<<"---------------Error-List--------------------------"<<std::endl;
		for(std::vector<CErrorToken*>::iterator it = m_ErrorList.begin() ; 
		it != m_ErrorList.end() ;it ++)
		{
			std::cout<<(*it)->strGetVar()<<std::endl;
		}
	}
}