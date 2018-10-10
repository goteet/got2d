#pragma once
#include <functional>
#include <memory>
#include <exception>
#include <sstream>
#include <cassert>
#define SR(x)  if(x) { x->Release(); x=nullptr; }
#define SD(x)  if(x) { delete x; x=nullptr; }
#define SDA(x) if(x) { delete[] x; x=nullptr; }

#ifdef _DEBUG
#define ENSURE(b) assert(b);
#define FAIL(info) assert(false && info);
#else
class ensure_exception : public std::exception
{
	std::string expression;
public:
	ensure_exception(const char* what, const char* filename, unsigned line)
	{
		std::stringstream ss;
		ss << "fail:" << what
			<< "\nfile:" << filename
			<< "\nline:" << line;
		expression = ss.str();
	}
	template<class T>
	ensure_exception& operator<<(std::pair<const char*, T> values)
	{
		std::stringstream ss;
		ss << "\n" << values.first << ":" << values.second;
		expression += ss.str();
		return *this;
	}
	ensure_exception& operator<<(int) { return *this; }
	virtual ~ensure_exception() throw() { }
	virtual const char* what() const override { return expression.c_str(); }
};
static int ENSURE_NEXT_A = 0;
static int ENSURE_NEXT_B = 0;
#define ENSURE_NEXT_A(v) ENSURE_LINK(v, ENSURE_NEXT_B)
#define ENSURE_NEXT_B(v) ENSURE_LINK(v, ENSURE_NEXT_A)
#define ENSURE_LINK(v, NEXT) std::make_pair(#v,v) <<NEXT
#define ENSURE(b) if (b); else throw ensure_exception(#b, __FILE__, __LINE__) <<ENSURE_NEXT_A
#define FAIL(info) throw ensure_exception(#info, __FILE__, __LINE__) <<ENSURE_NEXT_A
#endif