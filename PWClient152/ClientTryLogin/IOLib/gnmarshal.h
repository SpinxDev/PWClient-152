#ifndef __MARSHAL_H
#define __MARSHAL_H

#include <algorithm>
#include <string>
#include <vector>
#include "gnbyteorder.h"
#include "gnoctets.h"

namespace GNET
{

template<typename T>
inline T& remove_const(const T &t) { return const_cast<T&>(t); }

class Marshal
{
public:
	class Exception { };
	enum Transaction { Begin, Commit, Rollback };

	class OctetsStream : public Octets
	{
		enum { MAXSPARE = 16384};
		mutable unsigned int pos;
		mutable unsigned int tranpos;
		template<typename T> OctetsStream& push_byte(T t)
		{
			insert(end(), &t, sizeof(t));
			return *this;
		}
		template<typename T> void pop_byte(T &t) const
		{
			if (pos + sizeof(t) > size()) throw Marshal::Exception();
			t = *(T *)( (char*)begin() + pos );
			pos += sizeof(t);
		}
		unsigned char pop_byte_8() const
		{
			unsigned char c;
			pop_byte(c);
			return c;
		}
		unsigned short pop_byte_16() const
		{
			unsigned short s;
			pop_byte(s);
			return byteorder_16(s);
		}
		unsigned long pop_byte_32() const
		{
			unsigned long l;
			pop_byte(l);
			return byteorder_32(l);
		}
		unsigned __int64 pop_byte_64() const
		{
			unsigned __int64 ll;
			pop_byte(ll);
			return byteorder_64(ll);
		}

		friend class CompactUINT;
		friend class CompactSINT;

		OctetsStream& compact_uint32(unsigned int x)
		{
			if (x < 0x80) return push_byte((unsigned char)x);
			else if (x < 0x4000) return push_byte(byteorder_16(x|0x8000));
			else if (x < 0x20000000) return push_byte(byteorder_32(x|0xc0000000));
			push_byte((unsigned char)0xe0);
			return push_byte(byteorder_32(x));
		}
		OctetsStream& compact_sint32(int x)
		{
			if (x >= 0)
			{
				if (x < 0x40) return push_byte((unsigned char)x);
				else if (x < 0x2000) return push_byte(byteorder_16(x|0x8000));
				else if (x < 0x10000000) return push_byte(byteorder_32(x|0xc0000000));
				push_byte((unsigned char)0xe0);
				return push_byte(byteorder_32(x));
			}
			if (-x > 0)
			{
				x = -x;
				if (x < 0x40) return push_byte((unsigned char)x|0x40);
				else if (x < 0x2000) return push_byte(byteorder_16(x|0xa000));
				else if (x < 0x10000000) return push_byte(byteorder_32(x|0xd0000000));
				push_byte((unsigned char)0xf0);
				return push_byte(byteorder_32(x));
			}
			push_byte((unsigned char)0xf0);
			return push_byte(byteorder_32(x));
		}
		const OctetsStream& uncompact_uint32(const unsigned int &x) const
		{
			if (pos == size()) throw Marshal::Exception();
			switch ( *((unsigned char *)begin()+pos) & 0xe0)
			{
			case 0xe0:
				pop_byte_8();
				remove_const(x) = pop_byte_32();
				return *this;
			case 0xc0:
				remove_const(x) = pop_byte_32() & ~0xc0000000;
				return *this;
			case 0xa0:
			case 0x80:
				remove_const(x) = pop_byte_16() & ~0x8000;
				return *this;
			}
			remove_const(x) = pop_byte_8();
			return *this;
		}
		const OctetsStream& uncompact_sint32(const int &x) const
		{
			if (pos == size()) throw Marshal::Exception();
			switch ( *((unsigned char *)begin()+pos) & 0xf0)
			{
			case 0xf0:
				pop_byte_8();
				remove_const(x) = -(int)pop_byte_32();
				return *this;
			case 0xe0:
				pop_byte_8();
				remove_const(x) = pop_byte_32();
				return *this;
			case 0xd0:
				remove_const(x) = -(int)(pop_byte_32() & ~0xd0000000);
				return *this;
			case 0xc0:
				remove_const(x) = pop_byte_32() & ~0xc0000000;
				return *this;
			case 0xb0:
			case 0xa0:
				remove_const(x) = -(pop_byte_16() & ~0xa000);
				return *this;
			case 0x90:
			case 0x80:
				remove_const(x) = pop_byte_16() & ~0x8000;
				return *this;
			case 0x70:
			case 0x60:
			case 0x50:
			case 0x40:
				remove_const(x) = -(pop_byte_8() & ~0x40);
				return *this;
			}
			remove_const(x) = pop_byte_8();
			return *this;
		}
	public:
		OctetsStream() : pos(0) {}
		OctetsStream(const Octets &o) : Octets(o), pos(0) {}
		OctetsStream(const OctetsStream &os) : Octets(os), pos(0) {}

		OctetsStream& operator = (const OctetsStream &os) 
		{
			if (&os != this)
			{
				pos = os.pos;
				replace(os.begin(), os.size());
			}
			return *this;
		}

		OctetsStream& operator << (char x)               { return push_byte(x); }
		OctetsStream& operator << (unsigned char x)      { return push_byte(x); }
		OctetsStream& operator << (short x)              { return push_byte(byteorder_16(x)); }
		OctetsStream& operator << (unsigned short x)     { return push_byte(byteorder_16(x)); }
		OctetsStream& operator << (int x)                { return push_byte(byteorder_32(x)); }
		OctetsStream& operator << (unsigned int x)       { return push_byte(byteorder_32(x)); }
		OctetsStream& operator << (long x)               { return push_byte(byteorder_32(x)); }
		OctetsStream& operator << (unsigned long x)      { return push_byte(byteorder_32(x)); }
		OctetsStream& operator << (__int64 x)          { return push_byte(byteorder_64(x)); }
		OctetsStream& operator << (unsigned __int64 x) { return push_byte(byteorder_64(x)); }
		OctetsStream& operator << (float x)              { return push_byte(byteorder_32(*(unsigned long*)&x)); }
		OctetsStream& operator << (double x)             { return push_byte(byteorder_64(*(unsigned __int64*)&x)); }
		OctetsStream& operator << (const Marshal &x)     { return x.marshal(*this); }
		OctetsStream& operator << (const Octets &x)  
		{
			compact_uint32(x.size());
			insert(end(), x.begin(), x.end());
			return *this;
		}
		template<typename T>
		OctetsStream& operator << (const std::basic_string<T> &x)
		{
			compact_uint32(x.length());
			insert(end(), (void*)x.c_str(), x.length()*sizeof(T) );
			return *this;
		}
		OctetsStream& push_byte(const char *x, size_t len)
		{
			insert(end(), x, len);
			return *this;
		}
		template<typename T1, typename T2>
		OctetsStream& operator << (const std::pair<T1, T2> &x)
		{
			return *this << x.first << x.second;
		}

		bool eos() const { return pos == size(); }
		const OctetsStream& operator >> (Transaction trans) const
		{
			switch (trans)
			{
			case Marshal::Begin:
				tranpos = pos;
				break;
			case Marshal::Rollback:
				pos = tranpos;
				break;
			case Marshal::Commit:
				if (pos >= MAXSPARE)
				{
					remove_const(*this).erase((char*)begin(), (char*)begin()+pos);	
					pos = 0;
				}
			}
			return *this;
		}
		const OctetsStream& operator >> (const char &x) const
		{
			remove_const(x) = pop_byte_8();
			return *this;
		}
		const OctetsStream& operator >> (const unsigned char &x) const
		{
			remove_const(x) = pop_byte_8();
			return *this;
		}
		const OctetsStream& operator >> (const short &x) const
		{
			remove_const(x) = pop_byte_16();
			return *this;
		}
		const OctetsStream& operator >> (const unsigned short &x) const
		{
			remove_const(x) = pop_byte_16();
			return *this;
		}
		const OctetsStream& operator >> (const int &x) const
		{
			remove_const(x) = pop_byte_32();
			return *this;
		}
		const OctetsStream& operator >> (const unsigned int &x) const
		{
			remove_const(x) = pop_byte_32();
			return *this;
		}
		const OctetsStream& operator >> (const long &x) const
		{
			remove_const(x) = pop_byte_32();
			return *this;
		}
		const OctetsStream& operator >> (const unsigned long &x) const
		{
			remove_const(x) = pop_byte_32();
			return *this;
		}
		const OctetsStream& operator >> (const __int64 &x) const
		{
			remove_const(x) = pop_byte_64();
			return *this;
		}
		const OctetsStream& operator >> (const unsigned __int64 &x) const
		{
			remove_const(x) = pop_byte_64();
			return *this;
		}
		const OctetsStream& operator >> (const float &x) const
		{
			unsigned long l = pop_byte_32();
			remove_const(x) = *(float*)&l;
			return *this;
		}
		const OctetsStream& operator >> (const double &x) const
		{
			unsigned __int64 ll = pop_byte_64();
			remove_const(x) = *(double *)&ll;
			return *this;
		}
		const OctetsStream& operator >> (const Marshal &x) const
		{
			return remove_const(x).unmarshal(*this);
		}
		const OctetsStream& operator >> (const Octets &x) const
		{
			size_t len;
			uncompact_uint32(len);
			if (pos + len > size()) throw Marshal::Exception();
			remove_const(x).replace((char*)begin()+pos, len);
			pos += len;
	
			return *this;
		}
		template<typename T>
		const OctetsStream& operator >> (const std::basic_string<T> &x) const
		{
			size_t len;
			uncompact_uint32(len);
			if (pos + len*sizeof(T) > size()) throw Marshal::Exception();
			remove_const(x).assign((T*)((char*)begin()+pos), len);
			pos += len*sizeof(T);
			return *this;
		}
		void pop_byte(char *x, size_t len) const
		{
			if (pos + len > size()) throw Marshal::Exception();
			memcpy(x, (char*)begin()+pos, len);
			pos += len;
		}
		template<typename T>
		OctetsStream& operator << (const std::vector<T> &x)
		{
			return *this <<( MarshalContainer(x));
		}
		template<typename T>
		const OctetsStream& operator >> (const std::vector<T> &x) const
		{
			return *this >>( MarshalContainer(x));
		}
		
		template<typename T1, typename T2>
		const OctetsStream& operator >> (const std::pair<T1, T2> &x) const
		{
			return *this >> remove_const(x.first) >> remove_const(x.second);
		}
		Octets& clear() 
		{ 
			pos = 0;
			return Octets::clear();
		}
		Octets& replace(const void *data, size_t size)
		{
			pos = 0;
			return Octets::replace(data,size);
		}
	};

	virtual OctetsStream& marshal(OctetsStream &) const = 0;
	virtual const OctetsStream& unmarshal(const OctetsStream &) = 0;
	virtual ~Marshal() { }

};

template<typename Container>
class STLContainer : public Marshal
{
	Container *pc;
public:
	explicit STLContainer(Container &c) : pc(&c) { }
	OctetsStream& marshal(OctetsStream &os) const
	{
		os << CompactUINT(pc->size());
		for (typename Container::const_iterator i = pc->begin(); i != pc->end(); ++i)
			os << *i;
		return os;
	}
	const OctetsStream& unmarshal(const OctetsStream &os)
	{
		pc->clear();
		size_t size;
		for (os >> CompactUINT(size); size > 0; --size)
		{
			typename Container::value_type tmp;
			os >> tmp;
			pc->insert(pc->end(), tmp);
		}
		return os;
	}
};

template<typename Container>
inline STLContainer<Container> MarshalContainer(const Container &c)
{
	return STLContainer<Container> (remove_const(c));
}

class CompactUINT : public Marshal
{
	unsigned int *pi;
public:
	explicit CompactUINT(const unsigned int &i): pi(&remove_const(i)) { }

	OctetsStream& marshal(OctetsStream &os) const
	{
		return os.compact_uint32(*pi);
	}
	const OctetsStream& unmarshal(const OctetsStream &os)
	{
		return os.uncompact_uint32(*pi);
	}
};

class CompactSINT : public Marshal
{
	int *pi;
public:
	explicit CompactSINT(const int &i): pi(&remove_const(i)) { }

	OctetsStream& marshal(OctetsStream &os) const
	{
		return os.compact_sint32(*pi);
	}
	const OctetsStream& unmarshal(const OctetsStream &os)
	{
		return os.uncompact_sint32(*pi);
	}
};

};

#endif
