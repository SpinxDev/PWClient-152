
#ifndef __GNET_RPCDEFS_H
#define __GNET_RPCDEFS_H

#include <vector>

#include "gnmarshal.h"
#include "gnrpc.h"

typedef __int64 int64_t;

using namespace GNET;

namespace GNET
{

	class RpcRetcode : public Rpc::Data
	{
	public:
		int retcode;

		RpcRetcode (int l_retcode = -1) : retcode(l_retcode) { }
		RpcRetcode(const RpcRetcode &rhs) : retcode(rhs.retcode) { }
		Rpc::Data *Clone() const { return new RpcRetcode(*this); }

		Rpc::Data& operator = (const Rpc::Data &rhs)
		{
			const RpcRetcode *r = dynamic_cast<const RpcRetcode *>(&rhs);
			if (r && r != this)
			{
				retcode = r->retcode;
			}
			return *this;
		}

		OctetsStream& marshal(OctetsStream & os) const { return os << retcode; }
		const OctetsStream& unmarshal(const OctetsStream &os) { return os >> retcode; }
	};

	class IntOctets : public Rpc::Data
	{
	public:
		int m_int;
		Octets m_octets;

		IntOctets (int l_int = -1, Octets l_octets = Octets(0)) : m_int(l_int), m_octets(l_octets) { }
		IntOctets(const IntOctets &rhs) : m_int(rhs.m_int), m_octets(rhs.m_octets) { }
		Rpc::Data *Clone() const { return new IntOctets(*this); }

		Rpc::Data& operator = (const Rpc::Data &rhs)
		{
			const IntOctets *r = dynamic_cast<const IntOctets *>(&rhs);
			if (r && r != this)
			{
				m_int = r->m_int;
				m_octets = r->m_octets;
			}
			return *this;
		}

		IntOctets& operator = (const IntOctets &rhs)
		{
			m_int = rhs.m_int;
			m_octets = rhs.m_octets;
			return *this;
		}

		OctetsStream& marshal(OctetsStream & os) const { return os << m_int << m_octets; }
		const OctetsStream& unmarshal(const OctetsStream &os) { return os >> m_int >> m_octets; }
	};

	template < class T >
	class RpcDataVector : public Rpc::Data
	{
		std::vector<T>	m_data;
	public:
		typedef typename std::vector<T>::iterator       iterator;
		typedef typename std::vector<T>::const_iterator const_iterator;
		
		RpcDataVector ( ) { }
		RpcDataVector(const RpcDataVector &rhs) : m_data(rhs.m_data)
		{ }

		void resize( size_t __new_size ) { m_data.resize(__new_size); }
		void add( const T & __arg ) { m_data.insert(m_data.end(),__arg); }
		size_t size() { return m_data.size(); }
		T & operator [] ( size_t n ) { return m_data[n]; }

		Rpc::Data *Clone() const { return new RpcDataVector(*this); }

		Rpc::Data& operator = (const Rpc::Data &rhs)
		{
			const RpcDataVector *r = dynamic_cast<const RpcDataVector *>(&rhs);
			if (r && r != this)
			{
				m_data = r->m_data;
			}
			return *this;
		}

		RpcDataVector& operator = (const RpcDataVector &rhs)
		{
			m_data = rhs.m_data;
			return *this;
		}

		OctetsStream& marshal(OctetsStream & os) const
		{
			return os << MarshalContainer(m_data);
		}

		const OctetsStream& unmarshal(const OctetsStream &os)
		{
			return os >> MarshalContainer(m_data);
		}
		const_iterator begin() const { return m_data.begin(); }
		iterator begin() { return m_data.begin(); }
		const_iterator end() const { return m_data.end(); }
		iterator end() { return m_data.end(); }
		iterator erase(iterator __position) { return m_data.erase(__position); }
		iterator insert(iterator __position, const T& __x) { return m_data.insert(__position, __x); }
		void push_back(const T & __x) { m_data.push_back(__x);}
		
		std::vector<T>& GetVector() { return m_data; }
	};

	typedef RpcDataVector<unsigned char>	ByteVector;
	typedef RpcDataVector<int>			IntVector;
	typedef RpcDataVector<char>			CharVector;
	typedef RpcDataVector<Octets>		OctetsVector;
	typedef RpcDataVector<IntOctets>	IntOctetsVector;

	class OctetsTree : public Rpc::Data
	{
		Octets m_self;
		std::vector<OctetsTree>	m_children;
	public:
		OctetsTree ( ) { }
		OctetsTree ( Octets self ) : m_self(self) { }
		OctetsTree(const OctetsTree &rhs) : m_self(rhs.m_self), m_children(rhs.m_children)
		{ }

		Rpc::Data *Clone() const { return new OctetsTree(*this); }

		void SetSelf( Octets self ) { m_self = self; }
		Octets & GetSelf( ) { return m_self; }

		Rpc::Data& operator = (const Rpc::Data &rhs)
		{
			const OctetsTree *r = dynamic_cast<const OctetsTree *>(&rhs);
			if (r && r != this)
			{
				m_self = r->m_self;
				m_children = r->m_children;
			}
			return *this;
		}

		OctetsTree& operator = (const OctetsTree &rhs)
		{
			m_self = rhs.m_self;
			m_children = rhs.m_children;
			return *this;
		}

		OctetsStream& marshal(OctetsStream & os) const
		{
			os << m_self;
			return os << MarshalContainer(m_children);
		}

		const OctetsStream& unmarshal(const OctetsStream &os)
		{
			os >> m_self;
			return os >> MarshalContainer(m_children);
		}

	};

};

#endif

