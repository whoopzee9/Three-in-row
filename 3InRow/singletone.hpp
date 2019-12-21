#ifndef SINGLETONE_HPP
#define SINGLETONE_HPP

#include <vector>

class Node;

class Singletone {
public:
  static std::vector< Node * > * instance();
};

#endif
