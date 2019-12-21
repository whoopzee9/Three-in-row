#ifndef FIND_DESTROYABLE_HPP
#define FIND_DESTROYABLE_HPP

class Node;

class find_destroyable {
public:
  find_destroyable();
  void operator ()(Node *);
};

#endif
