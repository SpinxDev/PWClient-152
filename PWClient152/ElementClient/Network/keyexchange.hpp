
#ifndef __GNET_KEYEXCHANGE_HPP
#define __GNET_KEYEXCHANGE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class KeyExchange : public GNET::Protocol
{
#include "keyexchange"
	void Process(Manager *manager, Manager::Session::ID sid)
	{
	}
	
	Octets& GenerateKey(Octets &identity, Octets &password, Octets &nonce, Octets& key )
	{   
		HMAC_MD5Hash hash;
		hash.SetParameter(identity);
		hash.Update(password);
		hash.Update(nonce);
		return hash.Final(key);
	}
	
	void Setup(Manager* mgr, Manager::Session::ID sid, const char* name, int kick, Octets& oSecurity, Octets& iSecurity)
	{
		NetClient* m = (NetClient*)mgr;
		blkickuser = kick;
		GNET::Octets id = GNET::Octets(name, strlen(name));
		mgr->SetOSecurity(sid,ARCFOURSECURITY,GenerateKey(id, m->nonce, nonce, oSecurity));
		
		Random r;
		r.Update(nonce.resize(16));
		mgr->SetISecurity(sid,DECOMPRESSARCFOURSECURITY,GenerateKey(id, m->nonce, nonce, iSecurity));
	}
};

};

#endif
