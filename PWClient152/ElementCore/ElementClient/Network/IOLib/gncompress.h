#ifndef __COMPRESS_H
#define __COMPRESS_H

#include <zlib.h>

#include "gnproto.h"
#include "gnmppc.h"

namespace GNET
{

#define PROTOCOL_COMPRESSBINDER	16

class CompressBinder : public Protocol
{
	typedef std::vector<Protocol *> ProtocolVector;
	ProtocolVector pv;
	mutable Thread::RWLock locker_vector;
	
	Marshal::OctetsStream os_src;

	Protocol *Clone() const { return new CompressBinder(*this); }
public:
	~CompressBinder()
	{
		Thread::RWLock::WRScoped l(locker_vector);
		pv.clear();
	}
	explicit CompressBinder(Type type=PROTOCOL_COMPRESSBINDER) : Protocol(type)
	{
	}
	CompressBinder(const CompressBinder &rhs) : Protocol(rhs)
	{
		Thread::RWLock::RDScoped l(rhs.locker_vector);
		for( ProtocolVector::const_iterator it=rhs.pv.begin(); it != rhs.pv.end(); ++it )
			pv.push_back((*it)->Clone());
	}
	OctetsStream& marshal(OctetsStream &os) const
	{
		Octets os_com;

#ifdef USE_ZLIB
		os_com.reserve( compressBound(os_src.size()) );
		uLong len_com = os_com.capacity();
		if ( Z_OK == compress((Bytef*)os_com.begin(),&len_com,(Bytef*)os_src.begin(),os_src.size())
			&& len_com<os_src.size() )
		{
			os_com.resize(len_com);
			os << CompactUINT(os_src.size()) << CompactUINT(os_com.size());
			os.insert(os.end(), os_com.begin(), os_com.end());
			return os;
		}
#else
		os_com.reserve( mppc::compressBound(os_src.size()) );
		size_t len_com = os_com.capacity();
		bool success;
		if( os_src.size() <= 8192 )
		{
			success = ( 0 == mppc::compress((unsigned char*)os_com.begin(),(int*)&len_com,
									(const unsigned char*)os_src.begin(),os_src.size())
				&& len_com<os_src.size() );
		}
		else
		{
			success = ( 0 == mppc::compress2((unsigned char*)os_com.begin(),(int*)&len_com,
									(const unsigned char*)os_src.begin(),os_src.size())
				&& len_com<os_src.size() );
		}
		if( success )
		{
			os_com.resize(len_com);
			os << CompactUINT(os_src.size()) << CompactUINT(os_com.size());
			os.insert(os.end(), os_com.begin(), os_com.end());
			return os;
		}
#endif

		// no compress
		os << CompactUINT(os_src.size()) << CompactUINT(os_src.size());
		os.insert(os.end(), os_src.begin(), os_src.end());
		return os;
	}
	const OctetsStream& unmarshal(const OctetsStream &os)
	{
		Manager::Session::Stream &is = (Manager::Session::Stream &)(os);

		size_t	len_src, len_com;
		is >> CompactUINT(len_src) >> Marshal::Begin >> CompactUINT(len_com);
		if( len_com < len_src )
		{
			is >> Marshal::Rollback;

			Marshal::OctetsStream	os_com;
			Manager::Session::Stream is_src(is.session);

			os_com.reserve(len_com+8);
			is >> os_com;
			((Octets&)is_src).reserve( len_src );
#ifdef USE_ZLIB
			if (Z_OK == uncompress((Bytef*)is_src.begin(),(uLongf*)&len_src,
								(const Bytef*)os_com.begin(),os_com.size()))
#else
			if ( (len_src<=8192 && 0 == mppc::uncompress((unsigned char*)is_src.begin(),(int*)&len_src,
								(const unsigned char*)os_com.begin(),os_com.size()))
				|| (len_src>8192 && 0 == mppc::uncompress2((unsigned char*)is_src.begin(),(int*)&len_src,
								(const unsigned char*)os_com.begin(),os_com.size())) )
#endif
			{
				((Octets&)is_src).resize( len_src );
				Thread::RWLock::WRScoped l(locker_vector);
				Protocol * p;
				while( !is_src.eos() )
				{
					p = Protocol::Decode(is_src);
					if( NULL == p  )
						break;
					pv.push_back(p);
				}
			}
			else
			{
				printf("CompressBinder::unmarshal uncompress failed.len_src=%u,len_com=%u,ossize=%u",
						len_src, len_com, os.size() );
			}
		}
		else
		{
			is >> Marshal::Commit;

			Thread::RWLock::WRScoped l(locker_vector);
			Protocol * p;
			while( !is.eos() )
			{
				p = Protocol::Decode(is);
				if( NULL == p  )
					break;
				pv.push_back(p);
			}
		}

		return os;
	}
	void Process(Manager *manager, Manager::Session::ID sid)
	{
		Thread::RWLock::RDScoped l(locker_vector);
		for (ProtocolVector::const_iterator it = pv.begin(); it != pv.end(); ++it)
		{
			try
			{
				(*it)->Process(manager,sid);
			}
			catch (Protocol::Exception &)
			{
				manager->Close(sid);
			}
		}
	}
	void Notify(Manager *mgr, Manager::Session::ID sid)
	{
		{
			Thread::RWLock::RDScoped l(locker_vector);
			for (ProtocolVector::const_iterator it=pv.begin(); it!=pv.end();++it)
			{
				try
				{
					mgr->OnRecvProtocol(sid,(*it));
				}
				catch (Protocol::Exception &) {
					mgr->Close(sid);
				}
			}
		}
		Destroy();
	}
	CompressBinder& bind(const Protocol *p)
	{
		Thread::RWLock::WRScoped l(locker_vector);
		Marshal::OctetsStream oct;
		p->Encode(oct);
		os_src.push_byte( (const char*)oct.begin(), oct.size() );
		return *this;
	}
	CompressBinder& bind(const Protocol &p) { return bind(&p); }
	int  PriorPolicy() const { return 1; }
	bool SizePolicy(size_t size) const { return true; }
};

};

#endif
