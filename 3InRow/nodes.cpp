#include "nodes.hpp"

#include <QPainter>
#include <QObject>
#include <QGraphicsItemAnimation>

#include <algorithm>

#include "singletone.hpp"
#include "find-destroyable.hpp"
#include "scene-singletone.hpp"

bool Nodes::isDestroyable_ = false;
bool Nodes::isAnimationGoing_ = false;

size_t Nodes::score_ = 0;

QTimeLine * Nodes::timer_ = new QTimeLine(parameters::ANIMATION_PERIOD_MS);
QTimeLine * Nodes::replaceTimer_ = new QTimeLine(parameters::ANIMATION_PERIOD_MS);

void Nodes::add(Node * obj)
{
  if (!obj) {
    std::cerr << "OBJECT EMPTY\n";
    return;
  }
  if (Singletone::instance()->size() == parameters::FIELD_SIZE * parameters::FIELD_SIZE) {
    std::cerr << "FIELD IS FULL\n";
    return;
  }

  Singletone::instance()->push_back(obj);
}

void Nodes::swap(size_t index1, size_t index2)
{
  if (!Singletone::instance()) {
    return;
  }

  auto it1 = (*Singletone::instance()).begin();
  auto it2 = (*Singletone::instance()).begin();

  std::advance(it1, index1);
  std::advance(it2, index2);

  QPointF position = (*Singletone::instance())[index1]->getHomePos();

  Singletone::instance()->at(index1)->setHomePos(Singletone::instance()->at(index2)->getHomePos());
  Singletone::instance()->at(index2)->setHomePos(position);
  Singletone::instance()->at(index2)->moveToPos(Singletone::instance()->at(index2)->getHomePos());

  Singletone::instance()->at(index2)->setIndex(index1);
  Singletone::instance()->at(index1)->setIndex(index2);

  std::iter_swap(it1, it2);
}

void Nodes::process()
{
  std::for_each(Singletone::instance()->begin(), Singletone::instance()->end(), find_destroyable());

  setDestroyableFlag(false);
}

void Nodes::setDestroyableFlag(bool flag)
{
  Nodes::isDestroyable_ = flag;
}

void Nodes::setAnimationFlag(bool flag)
{
  Nodes::isAnimationGoing_ = flag;
}

void Nodes::addToScore(size_t addition)
{
  score_ += addition;
}

bool Nodes::getAnimationFlag()
{
  return Nodes::isAnimationGoing_;
}

size_t Nodes::getScore()
{
  return Nodes::score_;
}

bool Nodes::getDestroyableFlag()
{
  return Nodes::isDestroyable_;
}

void Nodes::resetScore()
{
  Nodes::score_ = 0;
}

bool Nodes::isDestroyable(size_t index, parameters::direction_t direction)
{
  if (index > parameters::FIELD_SIZE * parameters::FIELD_SIZE || direction == parameters::direction_t::NONE) {
    return false;
  }

  int verticalOffset = 0;
  int horizontalOffset = 0;

  const double deltaX = (*Singletone::instance())[index]->pos().x() - (*Singletone::instance())[index]->getHomePos().x();
  const double deltaY = (*Singletone::instance())[index]->pos().y() - (*Singletone::instance())[index]->getHomePos().y();

  size_t newIndex = index;

  if (direction == parameters::direction_t::VERTICAL) {
    horizontalOffset = parameters::FIELD_SIZE;

    if (deltaY < 0) {
      --newIndex;
      verticalOffset = -1;
    } else {
      ++newIndex;
      verticalOffset = 1;
    }

  } else if (direction == parameters::direction_t::HORIZONTAL) {
    verticalOffset = 1;

    if (deltaX < 0) {
      newIndex -= parameters::FIELD_SIZE;
      horizontalOffset = -parameters::FIELD_SIZE;
    } else {
      newIndex += parameters::FIELD_SIZE;
      horizontalOffset = parameters::FIELD_SIZE;
    }

  }

  const size_t column = newIndex / parameters::FIELD_SIZE;
  const size_t string = newIndex % parameters::FIELD_SIZE;

  if (direction == parameters::direction_t::VERTICAL) {

    switch (string) {
    case 0:
    case parameters::FIELD_SIZE - 1:
      {
        return isSpecialCaseDestroyable(index, newIndex, column, horizontalOffset)
            || isDefaultCaseDestroyable(newIndex, index, column, -verticalOffset, horizontalOffset) ;
      }
    case 1:
      {
        if (deltaY > 0) {
          return isDefaultCaseDestroyable(index, newIndex, column, verticalOffset, horizontalOffset)
              || isSpecialCaseDestroyable(newIndex, index, column, horizontalOffset);
        } else {
          return isSpecialCaseDestroyable(index, newIndex, column, horizontalOffset)
              || isDefaultCaseDestroyable(newIndex, index, column, -verticalOffset, horizontalOffset) ;
        }
      }
    case parameters::FIELD_SIZE - 2:
      {
        if (deltaY < 0) {
          return isDefaultCaseDestroyable(index, newIndex, column, verticalOffset, horizontalOffset)
              || isSpecialCaseDestroyable(newIndex, index, column, horizontalOffset);
        } else {
          return isSpecialCaseDestroyable(index, newIndex, column, horizontalOffset)
              || isDefaultCaseDestroyable(newIndex, index, column, -verticalOffset, horizontalOffset);
        }
      }
    default:
      {
        if ((deltaY > 0 && string == 2) || (deltaY < 0 && string == parameters::FIELD_SIZE - 3) ) {
          return isDefaultCaseDestroyable(index, newIndex, column, verticalOffset, horizontalOffset)
              || isSpecialCaseDestroyable(newIndex, index, column, horizontalOffset);
        }

        return isDefaultCaseDestroyable(index, newIndex, column, verticalOffset, horizontalOffset)
            || isDefaultCaseDestroyable(newIndex, index, column, -verticalOffset, horizontalOffset);
      }
    }

  } else {

    switch (column) {
    case 0:
    case parameters::FIELD_SIZE - 1:
      {
        return isSpecialCaseDestroyable(index, newIndex, string, verticalOffset)
            || isDefaultCaseDestroyable(newIndex, index, string, -horizontalOffset, verticalOffset) ;
      }
    case 1:
      {
        if (deltaX > 0) {
          return isDefaultCaseDestroyable(index, newIndex, string, horizontalOffset, verticalOffset)
              || isSpecialCaseDestroyable(newIndex, index, string, verticalOffset);
        } else {
          return isSpecialCaseDestroyable(index, newIndex, string, verticalOffset)
              || isDefaultCaseDestroyable(newIndex, index, string, -horizontalOffset, verticalOffset);
        }
      }
    case parameters::FIELD_SIZE - 2:
      {
        if (deltaX < 0) {
          return isDefaultCaseDestroyable(index, newIndex, string, horizontalOffset, verticalOffset)
              || isSpecialCaseDestroyable(newIndex, index, string, verticalOffset);
        } else {
          return isSpecialCaseDestroyable(index, newIndex, string, verticalOffset)
              || isDefaultCaseDestroyable(newIndex, index, string, -horizontalOffset, verticalOffset);
        }
      }
    default:
      {
        if ((deltaX > 0 && column == 2) || (deltaX < 0 && column == parameters::FIELD_SIZE - 3)) {
          return isDefaultCaseDestroyable(index, newIndex, string, horizontalOffset, verticalOffset)
              || isSpecialCaseDestroyable(newIndex, index, string, verticalOffset);
        }

        return isDefaultCaseDestroyable(index, newIndex, string, horizontalOffset, verticalOffset)
            || isDefaultCaseDestroyable(newIndex, index, string, -horizontalOffset, verticalOffset);
      }
    }

  }
}

void Nodes::setDestroyFlags(parameters::direction_t direction, size_t number)
{
  switch (direction) {
  case parameters::direction_t::HORIZONTAL:
    {
      size_t streak = 1;
      Qt::GlobalColor currentColor = Singletone::instance()->at(number)->getColor();

      for (size_t i = number + parameters::FIELD_SIZE; i < parameters::FIELD_SIZE * parameters::FIELD_SIZE; i += parameters::FIELD_SIZE ) {
        if (Singletone::instance()->at(i)->getColor() == currentColor) {
          streak++;

          if (streak >= 3) {
            if (streak == 3) {
              Singletone::instance()->at(i - parameters::FIELD_SIZE)->setDestroyFlag(true);
              Singletone::instance()->at(i - 2 * parameters::FIELD_SIZE)->setDestroyFlag(true);
            }
            Singletone::instance()->at(i)->setDestroyFlag(true);
            Nodes::setDestroyableFlag(true);
          }

        } else {
          streak = 1;
          currentColor = Singletone::instance()->at(i)->getColor();
        }
      }
      break;
    }
  case parameters::direction_t::VERTICAL:
    {
      size_t streak = 1;
      size_t index = number * parameters::FIELD_SIZE;
      Qt::GlobalColor currentColor = Singletone::instance()->at(index)->getColor();

      for (size_t i = index + 1; i < index + parameters::FIELD_SIZE; i++) {
        if (Singletone::instance()->at(i)->getColor() == currentColor) {
          streak++;

          if (streak >= 3) {
            if (streak == 3) {
              Singletone::instance()->at(i - 1)->setDestroyFlag(true);
              Singletone::instance()->at(i - 2)->setDestroyFlag(true);
            }
            Singletone::instance()->at(i)->setDestroyFlag(true);
            Nodes::setDestroyableFlag(true);
          }

        } else {
          streak = 1;
          currentColor = Singletone::instance()->at(i)->getColor();
        }
      }
    }
  }
}

Qt::GlobalColor Nodes::randomiseNodeColor()
{
  int i = rand() % 5;

  switch (i) {
  case 0:
    {
      return Qt::red;
    }
  case 1:
    {
      return Qt::green;
    }
  case 2:
    {
      return Qt::blue;
    }
  case 3:
    {
      return Qt::yellow;
    }
  case 4:
    {
      return Qt::magenta;
    }
  }
}

void Nodes::generateNewNodes()
{
  Qt::GlobalColor prevColor = Qt::black;
  for (size_t i = 0; i < parameters::FIELD_SIZE * parameters::FIELD_SIZE; i += parameters::FIELD_SIZE) {
    size_t index = i;

    while (Singletone::instance()->at(index)->getDestroyFlag()) {

      Qt::GlobalColor color = Nodes::randomiseNodeColor();
      while (color == prevColor) {
        color = Nodes::randomiseNodeColor();
      }
      prevColor = color;

      Singletone::instance()->at(index)->setColor(color);
      Singletone::instance()->at(index)->setDestroyFlag(false);
      Singletone::instance()->at(index)->setScale(0.1);

      SceneSingletone::instance()->addItem(Singletone::instance()->at(index));

      QGraphicsItemAnimation * animation = new QGraphicsItemAnimation(replaceTimer_);
      animation->setItem(Singletone::instance()->at(index));
      animation->setScaleAt(1, 10, 10);
      animation->setTimeLine(replaceTimer_);

      index++;
    }
  }
}


bool Nodes::isSideDestroyable(size_t index, size_t newIndex, int radius)
{
  return (Singletone::instance()->at(newIndex + radius)->getColor()
      == Singletone::instance()->at(newIndex + radius * 2)->getColor()
        && Singletone::instance()->at(index)->getColor()
        == Singletone::instance()->at(newIndex + radius)->getColor());
}

bool Nodes::isNeighborsDestroyable(size_t index, size_t newIndex, int radius)
{
  return (Singletone::instance()->at(newIndex + radius)->getColor() == Singletone::instance()->at(index)->getColor()
        && Singletone::instance()->at(index)->getColor() == Singletone::instance()->at(newIndex - radius)->getColor());
}

bool Nodes::isSpecialCaseDestroyable(size_t index, size_t newIndex, size_t coordPlace, int radius)
{
  switch (coordPlace) {
  case 0:
    {
      return isSideDestroyable(index, newIndex, radius);
    }
  case 1:
    {
      return (isNeighborsDestroyable(index, newIndex, radius) || isSideDestroyable(index, newIndex, radius));
    }
  case parameters::FIELD_SIZE - 2:
    {
      return (isNeighborsDestroyable(index, newIndex, radius) || isSideDestroyable(index, newIndex, -radius));
    }
  case parameters::FIELD_SIZE - 1:
    {
      return isSideDestroyable(index, newIndex, -radius);
    }
  default:
    {
      return (isSideDestroyable(index, newIndex, radius) || isSideDestroyable(index, newIndex, -radius)
              || isNeighborsDestroyable(index, newIndex, radius));
    }
  }
}

bool Nodes::isDefaultCaseDestroyable(size_t index, size_t newIndex, size_t coordPlace, int mainRadius, int sideRadius)
{
  switch (coordPlace) {
  case 0:
    {
      return (isSideDestroyable(index, newIndex, sideRadius) || isSideDestroyable(index, newIndex, mainRadius));
    }
  case 1:
    {
      return (isNeighborsDestroyable(index, newIndex, sideRadius) || isSideDestroyable(index, newIndex, sideRadius)
              || isSideDestroyable(index, newIndex, mainRadius));
    }
  case parameters::FIELD_SIZE - 2:
    {
      return (isNeighborsDestroyable(index, newIndex, sideRadius) || isSideDestroyable(index, newIndex, -sideRadius)
              || isSideDestroyable(index, newIndex, mainRadius));
    }
  case parameters::FIELD_SIZE - 1:
    {
      return (isSideDestroyable(index, newIndex, -sideRadius) || isSideDestroyable(index, newIndex, mainRadius));
    }
  default:
    {
      return (isSideDestroyable(index, newIndex, sideRadius) || isSideDestroyable(index, newIndex, -sideRadius)
              || isNeighborsDestroyable(index, newIndex, sideRadius) || isSideDestroyable(index, newIndex, mainRadius));
    }
  }
}
