#ifndef _KJSON_VALUE_H_
#define _KJSON_VALUE_H_



#include <map>
#include <string>
#include <vector>

namespace KJson
{
	enum ValueType
	{
		Value_Null = 0,		// 'null'  value
		Value_Ingeter,		// Number  value
		Value_Object,		// Object  value
		Value_Array,		// Array   value
		Value_Boolean,		// Boolean value
		Value_String,		// String  value
	};
	
	class KJsonValueBase
	{
	public:
		virtual std::string strGetJsonString() const = 0;
		ValueType GetValueType() const;
	protected:
		void SetValueType(ValueType vt);
	private:
		ValueType m_valuetype;
	};
	
	
	class KJsonInteger : public KJsonValueBase
	{
	public:
		static KJsonValueBase* create(const std::string& _value);
		virtual std::string strGetJsonString() const;
		int nGetInteger() const;
		int nGetBase() const;
		int nGetIndex() const;
		
	private:
		KJsonInteger(const std::string& integer);
		int m_base;
		int m_index;
	};
	
	
	class KJsonBoolean : public KJsonValueBase
	{
	public:
		static KJsonValueBase* create(const std::string& _value);
		virtual std::string strGetJsonString() const;
		bool bGetBoolean() const;
		int nGetBoolean() const;
		
	private:
		KJsonBoolean(const std::string& boolean);
		bool m_boolean;
	};
	
	
	class KJsonString : public KJsonValueBase
	{
	public:
		static KJsonValueBase* create(const std::string& _value);
		virtual std::string strGetJsonString() const;
		std::string strGetString() const;
		
	private:
		KJsonString(const std::string& string);
		std::string m_string;
	};
	
	
	class KJsonNull : public KJsonValueBase
	{
	public:
		static KJsonValueBase* create();
		virtual std::string strGetJsonString() const;
		int nGetNull() const;
		std::string strGetNull() const;
		
	private:
		KJsonNull();
	};
	
	
	class KJsonObject : public KJsonValueBase
	{
	public:
		static KJsonValueBase* create();
		virtual std::string strGetJsonString() const;
		~KJsonObject();
		int nGetSize() const;
		bool insert(const std::string& key, KJsonValueBase* value);
		bool erase(const std::string& key);
		bool erase(int nPos);
		KJsonValueBase* operator [](const std::string& key);
		KJsonValueBase* operator [](int nPos);
		bool bHasValue(const std::string& key);
		std::string strGetKey(int nPos); 
		KJsonValueBase* strGetValue(int nPos);
		void CleanAll();
			
	private:
		KJsonObject();
		std::map<std::string, KJsonValueBase*> m_child;
	};
	
	
	class KJsonArray : public KJsonValueBase
	{
	public:
		static KJsonValueBase* create();
		virtual std::string strGetJsonString() const;
		~KJsonArray();
		int nGetSize() const;
		bool insert(KJsonValueBase* value);
		bool erase(int nPos);
		KJsonValueBase* operator [](int nPos);
		void CleanAll();
		
	private:
		KJsonArray();
		std::vector<KJsonValueBase*> m_child;
	};
}

#endif // ! _KJSON_VALUE_H_