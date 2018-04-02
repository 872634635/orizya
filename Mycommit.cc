


#include <strings.h>
#include "th_assert.h"
#include "Message_tags.h"
#include "Digest.h"
#include "Mycommit.h"
#include "Node.h"
#include "Principal.h"

#include "Statistics.h"	
	
Mycommit::Mycommit(Mycommit_rep *myc) : Message(myc) {}

//构造mycommit函数  mycommit消息中包含requests 跟对应的OR消息所包含的是一致的。
Mycommit::Mycommit(View view, Seqno seq_num, int bsize, int replica, Digest &rh_d) :
	 Message(Mycommit_tag, Max_message_size)
{
			 
    rep().v = view; 
    rep().seq_num = seq_num;
	rep().b_size=bsize;
    rep().rep_id = replica;
    rep().rh_digest = rh_d;
	
	//INCR_OP(mycommit_auth);
    //START_CC(mycommit_auth_cycles);
    //p->gen_mac_out(contents(), sizeof(Mycommit_rep), contents()+sizeof(Mycommit_rep));
    //STOP_CC(reply_auth_cycles);
    //set_size(sizeof(Mycommit_rep)+MAC_size);
	 // Append batched requests information
	 
	/*char *nexte = contents()+sizeof(Order_request_rep);
	int cur = 0;
	int i = seq_num;
	Digest	reqd;
	bool	stat = rh_log.request_digest(i, &reqd);
	
	Request *	req	= NULL;
	

	if (stat)
	{
		
		for (Request *	req= rqueue.lookup(reqd); req != 0 && cur < bsize; req= rqueue.lookup(reqd))
		{
			req	= rqueue.lookup(reqd);
			int cid = req->client_id();
			Request_id rid = req->request_id();
			Digest &d = req->digest();

			memcpy(nexte,(char*)&cid,sizeof(int));
			nexte += sizeof(int);
			memcpy(nexte,(char *)&rid,sizeof(Request_id));
			nexte += sizeof(Request_id);
			memcpy(nexte,(char *)&d,sizeof(Digest)); 
			nexte += sizeof(Digest);
			cur++;
			i++;
			
		}
	
	int old_size =  sizeof(Order_request_rep)+
    bsize*(sizeof(int)+sizeof(Request_id)+sizeof(Digest))+nd_size;
  
	set_size(old_size);*/
}

void Mycommit::authenticate(bool use_auth ) //对应加密
{
	//printf("test\n");
   th_assert((unsigned) node->auth_size() < msize()-sizeof(Mycommit_rep),
	    "Insufficient memory");

  int old_size = sizeof(Mycommit_rep);
  
  // Use authenticator
if(use_auth){ 
  node->gen_auth_out(contents(), old_size, contents()+old_size);//到底是该in 还是out
}
  else{
    // Use signature
	//rep().extra |= 2;
    node->gen_signature(contents(), old_size, contents()+old_size);
}
   //trim(); 
}

//其实这个并没有用到
/*void Mycommit::re_authenticate(Principal *p, bool mac)
{
   // TO DO : Re-authenticate a principal using the new key
  if (p) {
    int old_size = size();
    
    if (mac) {
      th_assert((unsigned) MAC_size < (unsigned )(msize()-old_size),
	    "Insufficient memory");
      p->gen_mac_out(contents(),sizeof(Mycommit),contents()+old_size);
      set_size(old_size+MAC_size);
    }
    else {
      th_assert((unsigned) node->auth_size() < (unsigned) (msize()-old_size),
		"Insufficient memory");
      node->gen_auth_out(contents(), sizeof(Mycommit), contents()+sizeof(Mycommit));
      set_size(old_size+node->auth_size()); 
    }
    trim();
  }
}*/

//感觉需要两个verify()函数 
bool Mycommit::verify() 
{
return true;	
	if (!node->is_replica(id())) 
    return false;

	const int replica_id = node->id();//当前replica的id
	const int n_id = rep_id();//收到的mycommit消息对应的replica id
	const int old_size = sizeof(Mycommit_rep);
	Principal* p = node->i_to_p(replica_id);


	if (p != 0) 
	{
		// 1.0 First check is properly authenticated      
		if (/*replica_id != n_id  &&*/ size()-old_size >= node->auth_size()) //有疑问
		{//size():nassage size;
			if (!node->verify_auth_in(n_id, contents(), sizeof(Mycommit_rep),
					 contents()+old_size)) 
			{
				return false;
			}
		} 
		else 
		{
		  // Message is signed.
			if (size() - old_size >= p->sig_size()) 
			{
				if ( p->verify_signature(contents(), sizeof(Mycommit_rep),
					contents()+old_size, true)) 
				{
					return false;
				}
		  }
		}
	}
	
	
  return true;
}


 bool Mycommit::convert(Message *m1, Mycommit *&m2) {
  if (!m1->has_tag(Mycommit_tag, sizeof(Mycommit_rep)))
    return false;
  
  m1->trim();
  m2 = (Mycommit*)m1;
  return true;
}

//Mycommit.cc结束

