//
//  bank.h
//
//

#ifndef __BANK_H__
#define __BANK_H__

#include "customer.h"


class Bank {
public:
  Bank() = default;
  Bank(const ext_vector<int>& available) : avail(available), customers() { }
  
  ext_vector<int> get_avail() const { return avail; }
  bool is_avail(const ext_vector<int>& req) const { return req < avail; }
  
  // TODO: determine if alloc is safe
  bool is_safe(int id, const ext_vector<int>& req){ 
    // if we were to approve this request, could we at LEAST satisfy one future request?
    bool atleast = false;

    // create copy of bank available resources
    ext_vector<int> bank_copy = this->avail;
    // pseudo-approve request
    bank_copy -= req;

    // can we satisfy another request?
    for(size_t x = 0; x < this->avail.size(); ++x){
      if(x == id) continue; // don't count pseudo-approved request
      if(bank_copy >= this->customers[x]->create_req()){
         atleast = true;
      }
    }

    return atleast;
  }   

  bool req_approved(int id, const ext_vector<int>& req) {
    if (req > avail) { return false; }

    const Customer* c = customers[id];
    if (c->needs_exceeded(req)) { return false; }

    return is_safe(id, req);
  }
  
  void add_customer(Customer* c) {
    customers.push_back(c);
    avail -= c->allocated();
  }
  
  void withdraw_resources(const ext_vector<int>& req) {
    if (!is_avail(req)) {
      pthread_mutex_lock(&mutex_);
      std::cerr << "WARNING: req: " << req << " is not available for withdrawing\n";
      pthread_mutex_unlock(&mutex_);
      return;
    }
    if (is_avail(req)) { avail -= req; }
  }
  void deposit_resources(const ext_vector<int>& req) { avail += req; }


  ext_vector<Customer*> get_customers() const { return customers; }
  
  void show() const {
    pthread_mutex_lock(&mutex_);
    std::cout << "avail: [" << avail << "]\n";
    pthread_mutex_unlock(&mutex_);
    
    for (Customer* c : customers) {
      c->show();
    }
    std::cout << "\n";
  }
  
  friend std::ostream& operator<<(std::ostream& os, const Bank& be) {
    be.show();
    return os;
  }

private:
  ext_vector<int> avail;
  ext_vector<Customer*> customers;
};

#endif /* Bank_h */