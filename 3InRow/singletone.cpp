#include "singletone.hpp"

#include <memory>

static std::unique_ptr< std::vector< Node * > > nodes;

std::vector< Node * > * Singletone::instance()
{
  if (!nodes) {
    nodes.reset(new std::vector< Node * >);
  }

  return &(*nodes);
}
