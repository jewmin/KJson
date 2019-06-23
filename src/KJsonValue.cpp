#include <stdlib.h>
#include <sstream>
#include <math.h>
#include "../include/KJsonValue.h"



namespace KJson
{
	//---------------------------------------------------------------
	//KJsonValueBase
	
	
	ValueType KJsonValueBase::GetValueType() const
	{
		return m_valuetype;
	}
	
	
	void KJsonValueBase::SetValueType(ValueType vt)
	{
		m_valuetype = vt;
	};
	
	
	//---------------------------------------------------------------
	//KJsonInteger
	
	
	KJsonValueBase* KJsonInteger::create(const std::string& _value)
	{
		return static_cast<KJsonValueBase*>(new KJsonInteger(_value));
	}
	
	
	std::string KJsonInteger::strGetJsonString() const
	{
		std::stringstream ss;
		std::string ret;
		ss<<m_base;
		if( 0 != m_index)
		{
			//以科学计数法表示
			ss<<"e";
			ss<<m_index;
		}
		ss>>ret;
		return ret;
	}
	
	
	int KJsonInteger::nGetInteger() const
	{
		return m_base * pow(10, m_index);
	}
	
	
	int KJsonInteger::nGetBase() const
	{
		return m_base;
	}
	
	
	int KJsonInteger::nGetIndex() const
	{
		return m_index;
	}
	
	
	KJsonInteger::KJsonInteger(const std::string& integer)
	{
		SetValueType(Value_Ingeter);
		int ePos = integer.find('e');
		if(-1 == ePos)
		{
			//Not Found 'e'
			m_base = atoi(integer.c_str());
			m_index = 0;
		}
		else
		{
			//Found 'e'
			m_base = atoi(integer.substr(0,ePos).c_str());
			m_index = atoi(integer.substr(ePos + 1).c_str());
		}
	}
	
	
	//---------------------------------------------------------------
	//KJsonBoolean
	
	
	KJsonValueBase* KJsonBoolean::create(const std::string& _value)
	{
		return static_cast<KJsonValueBase*>(new KJsonBoolean(_value));
	}
	
	std::string KJsonBoolean::strGetJsonString() const
	{
		std::string ret;
		if(m_boolean)
		{
			ret = "true";
		}
		else
		{
			ret = "false";
		}
		return ret;
	}
	
	bool KJsonBoolean::bGetBoolean() const
	{
		return m_boolean;
	}
	
	
	int KJsonBoolean::nGetBoolean() const
	{
		return (int)m_boolean; 
	}
	
	KJsonBoolean::KJsonBoolean(const std::string& boolean)
	{
		SetValueType(Value_Boolean);
		if( 't' == boolean[0])
		{
			m_boolean = true;
		}
		else
		{
			m_boolean = false;
		}
	}
	
	
	//---------------------------------------------------------------
	//KJsonString
	
	
	KJsonValueBase* KJsonString::create(const std::string& _value)
	{
		return static_cast<KJsonValueBase*>(new KJsonString(_value));
	}
	
	
	std::string KJsonString::strGetJsonString() const
	{
		std::string ret;
		ret = ret + "\"" + m_string + "\"";
		return ret;
	}
	
	
	std::string KJsonString::strGetString() const
	{
		return m_string;
	}
	
	
	KJsonString::KJsonString(const std::string& string)
	{
		SetValueType(Value_String);
		m_string.assign(string);
	}
	
	
	//---------------------------------------------------------------
	//KJsonNull
	
	
	KJsonValueBase* KJsonNull::create()
	{
		return static_cast<KJsonValueBase*>(new KJsonNull());
	}
	
	std::string KJsonNull::strGetJsonString() const
	{
		return "null";
	}
	
	
	int KJsonNull::nGetNull() const
	{
		return 0;
	}
	
	
	std::string KJsonNull::strGetNull() const
	{
		return "null";
	}
		
	
	KJsonNull::KJsonNull()
	{
		SetValueType(Value_Null);
	}
	
	
	//---------------------------------------------------------------
	//KJsonObject
	
	
	KJsonValueBase* KJsonObject::create()
	{
		return static_cast<KJsonValueBase*>(new KJsonObject());
	}
	
	
	std::string KJsonObject::strGetJsonString() const
	{
		std::string ret;
		ret += "{";
		std::map<std::string, KJsonValueBase*>::const_iterator it;
		for( it = m_child.begin() ; it != m_child.end() ; it++)
		{
			ret = ret + (*it).first.c_str() + ":" + (*it).second->strGetJsonString() + ",";
		}
		if( 0 != nGetSize())
		{
			ret.erase(ret.length() - 1);
		}
		ret += "}";
		return ret;
	}
	
	
	KJsonObject::~KJsonObject()
	{
		CleanAll();
	}
	
	
	int KJsonObject::nGetSize() const
	{
		return m_child.size();
	}
	
	
	bool KJsonObject::insert(const std::string& key, KJsonValueBase* value)
	{
		std::pair<std::map<std::string, KJsonValueBase*>::iterator, bool> ret =
			m_child.insert(
				std::map<std::string, KJsonValueBase*>::value_type(key, value)
				);
		return ret.second;
	}
	
	
	bool KJsonObject::erase(const std::string& key)
	{
		std::map<std::string, KJsonValueBase*>::iterator it;
		it = m_child.find(key);
		if(m_child.end() == it)
		{
			return false;
		}
		else
		{
			m_child.erase(it);
			return true;
		}
	}
	
	
	bool KJsonObject::erase(int nPos)
	{
		if( nGetSize() <= nPos || 0 > nPos)
		{
			return false;
		}
		else
		{
			std::map<std::string, KJsonValueBase*>::iterator it = m_child.begin();
			while(0 != nPos)
			{
				it++;
				nPos--;
			}
			// m_child.erase(it + nPos);
			m_child.erase(it);
			return true;
		}
	}
	
	
	KJsonValueBase* KJsonObject::operator [](const std::string& key)
	{
		return static_cast<KJsonValueBase*>(m_child[key]);
	}
	
	
	KJsonValueBase* KJsonObject::operator [](int nPos)
	{
		if( nGetSize() <= nPos || 0 > nPos)
		{
			return NULL;
		}
		else
		{
			std::map<std::string, KJsonValueBase*>::iterator it = m_child.begin();
			while(0 != nPos)
			{
				it++;
				nPos--;
			}
			return static_cast<KJsonValueBase*>(it->second);
		}
	}
	
	
	bool KJsonObject::bHasValue(const std::string& key)
	{
		if( 0 != m_child[key])
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	
	
	std::string KJsonObject::strGetKey(int nPos)
	{
		if( nGetSize() <= nPos || 0 > nPos)
		{
			return "";
		}
		else
		{
			std::map<std::string, KJsonValueBase*>::iterator it = m_child.begin();
			while(0 != nPos)
			{
				it++;
				nPos--;
			}
			return static_cast<std::string>(it->first);
		}
	}
	
	
	KJsonValueBase* KJsonObject::strGetValue(int nPos)
	{
		if( nGetSize() <= nPos || 0 > nPos)
		{
			return NULL;
		}
		else
		{
			std::map<std::string, KJsonValueBase*>::iterator it = m_child.begin();
			while(0 != nPos)
			{
				it++;
				nPos--;
			}
			return static_cast<KJsonValueBase*>(it->second);
		}
	}
	
	
	void KJsonObject::CleanAll()
	{
		std::map<std::string, KJsonValueBase*>::iterator it;
		for( it = m_child.begin() ; it != m_child.end() ; it++)
		{
			delete ((*it).second);
		}
		m_child.clear();
	}
	
	
	KJsonObject::KJsonObject()
	{
		SetValueType(Value_Object);
	}
	
	
	//---------------------------------------------------------------
	//KJsonArray
	KJsonValueBase* KJsonArray::create()
	{
		return static_cast<KJsonValueBase*>(new KJsonArray());
	}
	
	
	std::string KJsonArray::strGetJsonString() const
	{
		std::string ret;
		ret += "[";
		std::vector<KJsonValueBase*>::const_iterator it;
		for( it = m_child.begin() ; it != m_child.end() ; it++)
		{
			ret = ret + (*it)->strGetJsonString() + ",";
		}
		if( 0 != nGetSize())
		{
			ret.erase(ret.length() - 1);
		}
		ret += "]";
		return ret;
	}
	
	
	KJsonArray::~KJsonArray()
	{
		CleanAll();
	}
	
	
	int KJsonArray::nGetSize() const
	{
		return m_child.size();
	}
	
	
	bool KJsonArray::insert(KJsonValueBase* value)
	{
		m_child.push_back(value);
		return true;
	}
	
	
	bool KJsonArray::erase(int nPos)
	{
		std::vector<KJsonValueBase*>::iterator it = m_child.begin();
		if( nGetSize() <= nPos || 0 > nPos)
		{
			return false;
		}
		else
		{
			while(0 != nPos)
			{
				it++;
				nPos--;
			}
			// m_child.erase(it + nPos);
			m_child.erase(it);
			return true;
		}
	}
	
	
	KJsonValueBase* KJsonArray::operator [](int nPos)
	{
		return static_cast<KJsonValueBase*>(m_child[nPos]);
	}
	
	
	void KJsonArray::CleanAll()
	{
		std::vector<KJsonValueBase*>::iterator it;
		for( it = m_child.begin() ; it != m_child.end() ; it++)
		{
			delete (*it);
		}
		m_child.clear();
	}
		
	
	KJsonArray::KJsonArray()
	{
		SetValueType(Value_Array);
	}
}