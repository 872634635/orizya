#include "Certificate.h"
#include "Node.h"

template <class T>
 
Certificate<T>::Certificate(int comp, int corr, int maxs) : bmap(Max_num_replicas) {
  max_size = (maxs == 0) ? node->f()*3+1 : maxs;
  vals = new Message_val[max_size];
  cur_size = 0;
  correct = (corr == 0) ? node->f()*2+1 : corr;
  complete = (comp == 0) ? node->f()*2+1 : comp;
  c = 0;
  mym = 0; 
}

template <class T> 
Certificate<T>::~Certificate() {
  delete [] vals;
}

template <class T> 
bool Certificate<T>::add(T *m) {
  const int id = m->id();
	printf("msg id: %d\n",id);
  if (node->is_replica(id) && !bmap.test(id)) {
    // "m" was sent by a replica that does not have a message in
    // the certificate
	printf("node->is_replica(id)\n");
    if ((c == 0 || (c->count < complete && c->m->match(m))) && m->verify()) {
      // add "m" to the certificate	
      // th_assert(id != node->id(), "verify should return false for messages from self");
      printf("c==0 || match vervify\n");
      bmap.set(id);	
      if (c) {
		printf("if(c)\n");
	c->count++;
	if (!c->m->full() && m->full()) {
	  // if c->m is not full and m is, replace c->m
		printf("!c->m->full()\n");	 
 delete c->m;
	  //fprintf(stderr," Certificate full : deleting prev m : %x \n",c->m); 	
	   c->m = m;	  
	} else {
	  //fprintf(stderr," Certificate full : deleting cur m : %x \n",m); 
	  delete m;
	}
	return true;
      } 
      
      // Check if there is a value that matches "m"
      int i;
      for (i=0; i < cur_size; i++) {
	Message_val &val = vals[i];
	if (val.m->match(m)) {
	  val.count++;
	  fprintf(stderr," Certificate matching values %d \n",val.count); 			
	  if (val.count >= correct) {
	    c = vals+i;
	    fprintf(stderr," Certificate complete with matching= %d \n",val.count);	
          }

	  if (!val.m->full() && m->full()) {
	    // if val.m is not full and m is, replace val.m
	    delete val.m;
	     //fprintf(stderr," Certificate not full : deleting prev m : %x \n",val.m); 		
	    val.m = m;
	  } else {
	    //fprintf(stderr," Certificate not full : deleting cur m : %x \n",m); 
	    delete m;
	  }
	  return true;
	}
      }
      
      // "m" has a new value.
      if (cur_size < max_size) {
	vals[cur_size].m = m;
	vals[cur_size++].count = 1;
	printf("cur_size<max_size update values\n");
	return true;
      } else {
	// Should only happen for replies to read-only requests.
	fprintf(stderr, "More than f+1 distinct values in certificate");
	clear();
      } 

    } else {
      // fprintf(stderr, "About to delete count : %d complete : %d",c->count,complete);
      if (m->verify()) bmap.set(id);
    }
  }
  //fprintf(stderr, "About to delete m: %x",m);
  delete m;
  return false;
}
    

template <class T> 
bool Certificate<T>::add_mine(T *m) {

  // ZYZZYVA: Add my order request message after adding message
  // from the primary. Since we use the same message as the one 
  // received from the primary but with my authenticator. Hence, this message
  // cannot be deleted.
	
  const int id = m->id();
  if (node->is_replica(id) && !bmap.test(id)) {
    // "m" was sent by a replica that does not have a message in
    // the certificate
    if ((c == 0 || (c->count < complete && c->m->match(m))) && m->verify()) {
      // add "m" to the certificate	
      // th_assert(id != node->id(), "verify should return false for messages from self");
      
      bmap.set(id);	
      if (c) {
	c->count++;
    printf(" add_mine successed!\n");
	return true;
      } 
      
      // Check if there is a value that matches "m"
      int i;
      for (i=0; i < cur_size; i++) {
	Message_val &val = vals[i];
	if (val.m->match(m)) {
	  val.count++;
	  //fprintf(stderr," Certificate matching values %d \n",val.count); 			
	  if (val.count >= correct) {
	    c = vals+i;
	    //fprintf(stderr," Certificate complete with matching= %d \n",val.count);	
          }
	printf("add_mine successed!\n");
	 return true;
	}
      }
      
      // "m" cannot be a new value.
      th_assert(0,"Invalid call to add_mine.");

    } else {
      if (m->verify()) bmap.set(id);
    }
  }
  
  return false;
}

template <class T> 
bool Certificate<T>::add_mine_commit(T *m) {

  // ZYZZYVA: Add my order request message after adding message
  // from the primary. Since we use the same message as the one 
  // received from the primary but with my authenticator. Hence, this message
  // cannot be deleted.
	
  const int id = m->id();
  if (node->is_replica(id) && !bmap.test(id)) {
    // "m" was sent by a replica that does not have a message in
    // the certificate
    if ((c == 0 || (c->count < complete && c->m->match(m))) && m->verify()) {
      // add "m" to the certificate	
      // th_assert(id != node->id(), "verify should return false for messages from self");
      
      bmap.set(id);	
      if (c) {
	c->count++;
    printf(" add_mine successed!\n");
	return true;
      } 
      
      // Check if there is a value that matches "m"
      int i;
      for (i=0; i < cur_size; i++) {
	Message_val &val = vals[i];
	if (val.m->match(m)) {
	  val.count++;
	  //fprintf(stderr," Certificate matching values %d \n",val.count); 			
	  if (val.count >= correct) {
	    c = vals+i;
	    //fprintf(stderr," Certificate complete with matching= %d \n",val.count);	
          }
	printf("add_mine successed!\n");
	 return true;
	}
      }
      
      // "m" cannot be a new value.
      //th_assert(0,"Invalid call to add_mine.");

      if (cur_size < max_size) {
	vals[cur_size].m = m;
	vals[cur_size++].count = 1;
	printf("cur_size<max_size update values\n");
	return true;
      } else {
	// Should only happen for replies to read-only requests.
	fprintf(stderr, "More than f+1 distinct values in certificate");
	clear();
      } 
    } else {
      if (m->verify()) bmap.set(id);
    }
  }
 return false;
} 


template <class T> 
void Certificate<T>::mark_stale() {
  if (!is_complete()) {
    int i = 0;
    int old_cur_size = cur_size;
    if (mym) {
      th_assert(mym == c->m, "Broken invariant");
      c->m = 0;
      c->count = 0;
      c = vals;
      c->m = mym;
      c->count = 1;
      i = 1;
    } else {
      c = 0;
    }
    cur_size = i;    

    for (; i < old_cur_size; i++) vals[i].clear();
    bmap.clear();
  }
}
  
template <class T> 
T * Certificate<T>::cvalue_clear() {
  if (c == 0) {
    return 0;
  }

  T *ret = c->m;
  c->m = 0;
  for (int i=0; i < cur_size; i++) {
    if (vals[i].m == ret)
      vals[i].m = 0;
  }
  clear();

  return ret;
}


template <class T> 
bool Certificate<T>::encode(FILE* o) {
  bool ret = bmap.encode(o);

  size_t sz = fwrite(&max_size, sizeof(int), 1, o);
  sz += fwrite(&cur_size, sizeof(int), 1, o);
  for (int i=0; i < cur_size; i++) {
    int vcount = vals[i].count;
    sz += fwrite(&vcount, sizeof(int), 1, o);
    if (vcount) {
      ret &= vals[i].m->encode(o);
    }
  }

  sz += fwrite(&complete, sizeof(int), 1, o);

  int cindex = (c != 0) ? c-vals : -1;
  sz += fwrite(&cindex, sizeof(int), 1, o);

  bool hmym = mym != 0;
  sz += fwrite(&hmym, sizeof(bool), 1, o);
  
  return ret & (sz == 5U+cur_size);
}


template <class T> 
bool Certificate<T>::decode(FILE* in) {
  bool ret = bmap.decode(in);

  size_t sz = fread(&max_size, sizeof(int), 1, in);
  delete [] vals;

  vals = new Message_val[max_size];
  
  sz += fread(&cur_size, sizeof(int), 1, in);
  if (cur_size < 0 || cur_size >= max_size)
    return false;

  for (int i=0; i < cur_size; i++) {
    sz += fread(&vals[i].count, sizeof(int), 1, in);
    if (vals[i].count < 0 || vals[i].count > node->n())
      return false;

    if (vals[i].count) {
      vals[i].m = (T*)new Message;
      ret &= vals[i].m->decode(in);
    }
  }

  sz += fread(&complete, sizeof(int), 1, in);
  correct = node->f()+1;

  int cindex;
  sz += fread(&cindex, sizeof(int), 1, in);

  bool hmym;
  sz += fread(&hmym, sizeof(bool), 1, in);
  
  if (cindex == -1) {
    c = 0;
    mym = 0;
  } else {
    if (cindex < 0 || cindex > cur_size) 
      return false;
    c = vals+cindex;

    if (hmym)
      mym = c->m;
  }
  
  t_sent = zeroTime();

  return ret & (sz == 5U+cur_size);
}
