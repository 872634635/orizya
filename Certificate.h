#ifndef _Certificate_h
#define _Certificate_h 1

#include <sys/time.h>
#include "types.h"
#include "Time.h"
#include "parameters.h"
#include "Bitmap.h"

template <class T> class Certificate {
  //
  // A certificate is a set of "matching" messages from different
  // replicas. 
  //
  // T must have the following methods:
  // bool match(T*);
  // // Effects: Returns true iff the messages match
  //
  // int id();
  // // Effects: Returns the identifier of the principal that
  // // sent the message.
  //
  // bool verify();
  // // Effects: Returns true iff the message is properly authenticated
  // // and statically correct.
  //
  // bool full();
  // // Effects: Returns true iff the message is full
  //
  // bool encode(FILE* o);
  // bool decode(FILE* i);
  // Effects: Encodes and decodes object state from stream. Return
  // true if successful and false otherwise.
  
public:

  Certificate(int complete=0, int correct=0, int maxs=0);
  // Requires: "complete" (2*f()+1) and "correct" (2*f()+1) to be specified 
  // Effects: Creates an empty certificate. The certificate is
  // complete when it contains at least "complete" matching messages
  // from different replicas. If the complete argument is omitted (or
  // 0) it is taken to be 2f+1. The certificate is correct if it received 
  // matching "correct" messages. If the complete argument is omitted (or
  // 0) it is taken to be 2f+1. If the correct is ommitted it is taken 
  // to be 2*f()+1. "maxs" is the total number of messages that can be 
  // received and it is set to 3*f()+1 if not specified.
  // 

  ~Certificate();
  // Effects: Deletes certificate and all the messages it contains.
 
  bool add(T *m);
  // Effects: Adds "m" to the certificate and returns true provided
  // "m" satisfies:
  // 1. there is no message from "m.id()" in the certificate
  // 2. "m->verify() == true"
  // 3. if "cvalue() != 0", "cvalue()->match(m)";
  // otherwise, it has no effect on this and returns false.  This
  // becomes the owner of "m" (i.e., no other code should delete "m"
  // or retain pointers to "m".)

  bool add_mine(T *m);
  // Requires: The identifier of the calling principal is "m->id()"
  // and "mine()==0" and m is full.
  // Effects: If "cvalue() != 0" and "!cvalue()->match(m)", it has no
  // effect and returns false. Otherwise, adds "m" to the certificate
  // and returns. This becomes the owner of "m"

  bool add_mine_commit(T *m); // this code is writen by ZHANG JINGJING

  T *mine(Time **t=0);
  // Effects: Returns caller's message in certificate or 0 if there is
  // no such message. If "t" is not null, sets it to point to the time
  // at which I last sent my message.

  T *cvalue() const;
  // Effects: Returns the correct message value for this certificate
  // or 0 if this value is not known. Note that the certificate
  // retains ownership over the returned value (e.g., if clear or
  // mark_stale are called the value may be deleted.)

  T *cvalue_clear();
  // Effects: Returns the correct message value for this certificate
  // or 0 if this value is not known. If it returns the correct value,
  // it removes the message from the certificate and clears the
  // certificate (that is the caller gains ownership over the returned
  // value.)

  int num_recv();
  // Number of valid messages recvd

  int num_correct() const;
  // Effects: Returns the number of messages with the correct value
  // in this.

  bool is_complete() const;
  // Is certificate complete 

  bool is_fully_correct() const;
  // Does certificate consist of matching enties from all replicas

  bool is_done() const;
  // Have we received messages from all replicas

  bool is_correct() const;
  // Have we received a correct certificate 

  void make_complete();
  // Effects: If cvalue() is not null, makes the certificate
  // complete.
    
  void mark_stale();
  // Effects: Discards all messages in certificate except mine. 

  void clear(); 
  // Effects: Discards all messages in certificate

  bool is_empty() const;
  // Effects: Returns true iff the certificate is empty

  inline bool read(T*& m, int index) const;

  inline bool read_rep(T*& m, int replica_id) const;
  // Read the message reecived from the replica with id == replica_id;

  // Return the entry in index of vals[]

  class Val_iter {
    // An iterator for yielding all the distinct values in a
    // certificate and the number of messages matching each value. The
    // certificate cannot be modified while it is being iterated on.
  public:
    Val_iter(Certificate<T>* c);
    // Effects: Return an iterator for the values in "c"
	
    bool get(T*& m, int& count);
    // Effects: Updates "m" to point to the next value in the
    // certificate and count to contain the number of messages
    // matching this value and returns true. If there are no more
    // values, it returns false.

  private:
    Certificate<T>* cert; 
    int next;
  };
  friend  class Val_iter;

  bool encode(FILE* o);
  bool decode(FILE* i);
  // Effects: Encodes and decodes object state from stream. Return
  // true if successful and false otherwise.

private:
  Bitmap bmap; // bitmap with replicas whose message is in this.

  class Message_val {
  public:
    T *m;
    int count; 
    
    inline Message_val() { m = 0; count = 0; }
    inline void clear() { 
      //fprintf(stderr," Certificate clear : deleting cur m : %x \n",m); 
      delete m; 
      m = 0;
      count = 0;
    }
    inline ~Message_val() { clear(); }
    
  };
  Message_val *vals;    // vector with all distinct message values in this
  int max_size;         // maximum number of elements in vals, f+1
  int cur_size;         // current number of elements in vals

  int correct;          // value is correct if it appears in at least "correct" messages
  Message_val *c;       // correct certificate value or 0 if unknown.

  int complete;         // certificate is complete if "num_correct() >= complete"

  T *mym; // my message in this or null if I have no message in this 
  Time t_sent; // time at which mym was last sent

  // The implementation assumes:
  // correct > 0 and complete > correct 
};

template <class T> 
inline T *Certificate<T>::mine(Time **t=0) { 
  if (t && mym) *t = &t_sent;
  return mym;
}

template <class T> 
inline T *Certificate<T>::cvalue() const { return (c) ? c->m : 0; }

template <class T> 
inline int Certificate<T>::num_correct() const {  return (c) ? c->count : 0; }

template <class T> 
inline int Certificate<T>::num_recv() {  return bmap.total_set();}

template <class T>
inline bool Certificate<T>::is_complete() const { return num_correct() >= complete; }

template <class T>
inline bool Certificate<T>::is_correct() const { return (c) ? c->count : 0; }

// is done is used when all the messages are received from different replicas
template <class T>
inline bool Certificate<T>::is_done() const { return (c) ? (c->count+cur_size == max_size) : (cur_size == max_size);}

template <class T>
inline bool Certificate<T>::is_fully_correct() const { return (c) ? (c->count == max_size) : (false); }

template <class T>
inline  void Certificate<T>::make_complete() {
  if (c) {
    c->count = complete;
  }
}


template <class T>
inline bool Certificate<T>::is_empty() const {
  return bmap.all_zero();
}

// TO DO: Include id of the sender of the message m
//  Check in the bitmap for the sender id before returning
// We use this to read the order request message sent by the current primary 
template <class T>
inline bool Certificate<T>::read(T*& m, int index) const {
 if (cur_size < index || is_empty()) {
    fprintf(stderr," Failed to read OR : %d %d \n", cur_size, index); 
    return false;
 }
 else {
   // fprintf(stderr," Read OR : %d %d count : %d \n", cur_size, index, vals[index].count); 
   m = vals[index].m;
   return true;
 } 
} 

template <class T>
inline bool Certificate<T>::read_rep(T*& m, int replica_id) const {
  int index = 0;
  if (is_empty()) {
    fprintf(stderr," Failed to read OR : %d %d \n", cur_size, index); 
    return false;
  }
  
  while (index < cur_size) {
    // fprintf(stderr," Read OR : %d %d count : %d \n", cur_size, index, vals[index].count); 
    if (vals[index].m->id() == replica_id) {
      m = vals[index].m;
      return true;
    }
    index++;
  } 

  return false;
} 

template <class T>
inline void Certificate<T>::clear() {
  for (int i=0; i < cur_size; i++) vals[i].clear(); 
  bmap.clear();
  c = 0;
  mym = 0;
  t_sent = 0;
  cur_size = 0;
}


template <class T>
inline Certificate<T>::Val_iter::Val_iter(Certificate<T>* c) {
  cert = c;
  next = 0;
}
	

template <class T>
inline bool Certificate<T>::Val_iter::get(T*& m, int& count) {
  if (next < cert->cur_size) {
    m = cert->vals[next].m;
    count = cert->vals[next].count;
    next++;
    return true;
  } 
  return false;
}

#endif // Certificate_h
  










