// Making a "ring" data structure from the STL
// Adapted (and fixed) from Bruce Eckel's Thinking in C++, 2nd Ed
// http://www.codeguru.com/cpp/tic/tic0234.shtml

#include <list>

template<class T>
class Ring {

  std::list<T> lst;

public:
  // Declaration necessary so the following 
  // 'friend' statement sees this 'iterator' 
  // instead of std::iterator:
  class iterator;
  friend class iterator;

  class iterator : public std::iterator<std::bidirectional_iterator_tag, T, std::ptrdiff_t>{

    typename std::list<T>* r;
    typename std::list<T>::iterator it;

  public:

    iterator(){}

    // "typename" necessary to resolve nesting:
    iterator(typename std::list<T>& lst, const typename std::list<T>::iterator& i)
      : r(&lst),
        it(i) {}

    bool operator==(const iterator& x) const { return it == x.it; }

    bool operator!=(const iterator& x) const { return !(*this == x); }

    T& operator*() const { return *it; }

    iterator& operator++() {
      ++it;
      if(it == r->end()) it = r->begin();
      return *this;
    }

    iterator operator++(int) {
      iterator tmp = *this;
      ++*this;
      return tmp;
    }

    iterator& operator--() {
      if(it == r->begin()) it = r->end();
      --it;
      return *this;
    }

    iterator operator--(int) {
      iterator tmp = *this;
      --*this; 
      return tmp;
    }

    iterator insert(const T& x){ return iterator(*r, r->insert(it, x)); }

    iterator erase() { return iterator(*r, r->erase(it)); }

  };

  void push_back(const T& x) { lst.push_back(x);}

  iterator begin() { return iterator(lst, lst.begin()); }

  size_t size() const { return lst.size(); }
};

///////////////////
// Example and test
///////////////////

// #include <iostream>
// #include <string>

// int main( int argc, char** argv ) {
//   Ring<std::string> rs;
//   rs.push_back("one");
//   rs.push_back("two");
//   rs.push_back("three");
//   rs.push_back("four");
//   rs.push_back("five");
//   Ring<std::string>::iterator it = rs.begin();
//   it++; it++;
//   it.insert("six");
//   it = rs.begin();
//   //Twice around the ring:
//   for(int i = 0; i < rs.size() * 2; i++)
//     std::cout << *it++ << std::endl;
//   return 0;
// }
