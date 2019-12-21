#include "find-destroyable.hpp"

#include "scene-singletone.hpp"
#include "singletone.hpp"
#include "nodes.hpp"

find_destroyable::find_destroyable()
{
}

void find_destroyable::operator ()(Node * node)
{
  if (!node) {
    throw std::runtime_error("Unexpected error");
  }

  if (node->getDestroyFlag()) {
    size_t index = node->getIndex();

    if (index + 1 != ((index / parameters::FIELD_SIZE) + 1) * parameters::FIELD_SIZE
        && Singletone::instance()->at(index + 1)->getDestroyFlag()) {

      return;
    }

    size_t count = 0;
    int i = static_cast< int >(index);
    while (i + 1 != ((index / parameters::FIELD_SIZE)) * parameters::FIELD_SIZE
        && Singletone::instance()->at(i)->getDestroyFlag()) {

      ++count;
      --i;
    }

    for (int i = static_cast< int >(index - count); i > static_cast< int >(index - index % parameters::FIELD_SIZE - 1); --i) {
      Singletone::instance()->at(i)->moveToPos(Singletone::instance()->at(i + count)->getHomePos());
      Nodes::swap(i, i + count);
    }

    index = (index / parameters::FIELD_SIZE) * parameters::FIELD_SIZE;

    Nodes::addToScore(count);

    while(count) {
      --count;
      SceneSingletone::instance()->removeItem(Singletone::instance()->at(index + count));
    }
  }
}
