#include "node.hpp"

#include <iostream>
#include <cmath>
#include <iterator>
#include <algorithm>

#include <QGraphicsItemAnimation>
#include <QTimeLine>
#include <windows.h>

#include "nodes.hpp"
#include "singletone.hpp"

static void setRemoveFlags(size_t index1, size_t index2)
{
  Nodes::setDestroyFlags(parameters::direction_t::HORIZONTAL, index1 % parameters::FIELD_SIZE);
  Nodes::setDestroyFlags(parameters::direction_t::HORIZONTAL, index2 % parameters::FIELD_SIZE);
  Nodes::setDestroyFlags(parameters::direction_t::VERTICAL, index1 / parameters::FIELD_SIZE);
  Nodes::setDestroyFlags(parameters::direction_t::VERTICAL, index2 / parameters::FIELD_SIZE);
}

Node::Node(Qt::GlobalColor color, size_t index, size_t diameter):
  QObject(),
  QGraphicsItem(),
  animation_(nullptr),
  movingDir_(parameters::direction_t::NONE),
  homePos_(0, 0),
  color_(color),
  index_(index),
  diameter_(diameter),
  isDestroyable_(false)
{
}

void Node::moveToPos(const QPointF & point)
{
  animation_ = new QGraphicsItemAnimation(Nodes::timer_);

  animation_->setItem(this);
  animation_->setTimeLine(Nodes::timer_);
  animation_->setPosAt(1.0, point);
}

void Node::initAnimation()
{
  Nodes::timer_ = new QTimeLine(parameters::ANIMATION_PERIOD_MS);
  connect(Nodes::timer_, SIGNAL(finished()), SLOT(onAnimationFinished()));
}

void Node::initReplaceAnimation()
{
  Nodes::replaceTimer_ = new QTimeLine(parameters::ANIMATION_PERIOD_MS);
  QObject::connect(Nodes::replaceTimer_, SIGNAL(finished()), SLOT(onReplaceAnimationFinished()));
}


void Node::setColor(Qt::GlobalColor color)
{
  color_ = color;
}

void Node::setHomePos(const QPointF & point)
{
  homePos_.setX(point.x());
  homePos_.setY(point.y());
}

void Node::setIndex(size_t index)
{
  index_ = index;
}

void Node::setDestroyFlag(bool flag)
{
  isDestroyable_ = flag;
}

Qt::GlobalColor Node::getColor() const
{
  return color_;
}

QPointF Node::getHomePos() const
{
  return homePos_;
}

size_t Node::getIndex() const
{
  return index_;
}

bool Node::getDestroyFlag() const
{
  return isDestroyable_;
}

void Node::onAnimationFinished()
{
  Sleep(300);

  Nodes::setAnimationFlag(true);

  initReplaceAnimation();
  Nodes::generateNewNodes();

  Nodes::replaceTimer_->start();
}

void Node::onReplaceAnimationFinished()
{
  Sleep(300);

  for (size_t i = 0; i < parameters::FIELD_SIZE; i++) {
    Nodes::setDestroyFlags(parameters::direction_t::VERTICAL, i);
    Nodes::setDestroyFlags(parameters::direction_t::HORIZONTAL, i);
  }

  if (Nodes::getDestroyableFlag()) {
    Nodes::setAnimationFlag(true);
    initAnimation();
    Nodes::process();
    Nodes::timer_->start();
  } else {
    Nodes::setAnimationFlag(false);
  }
}

QRectF Node::boundingRect() const
{
  return QRectF(-30, -30, diameter_, diameter_);
}

QPainterPath Node::shape() const
{
  QPainterPath path;
  path.addEllipse(-30, -30, diameter_, diameter_);
  return path;
}

void Node::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
  painter->setPen(Qt::black);
  painter->setBrush(getColor());
  painter->drawEllipse(-30, -30, diameter_, diameter_);
  Q_UNUSED(option)
  Q_UNUSED(widget)
}

void Node::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{
  QPointF point = mapToScene(event->pos());

  const double deltaX = point.x() - homePos_.x();
  const double deltaY = point.y() - homePos_.y();

  if (movingDir_ == parameters::direction_t::NONE) {
    if ((std::fabs(deltaX) > diameter_ / 2) && (std::fabs(deltaX) > std::fabs(deltaY))) {
      movingDir_ = parameters::direction_t::HORIZONTAL;
    } else if ((std::fabs(deltaY) > diameter_ / 2) && (std::fabs(deltaX) < std::fabs(deltaY))) {
      movingDir_ = parameters::direction_t::VERTICAL;
    }
  }

  if (movingDir_ == parameters::direction_t::HORIZONTAL) {

    if ((index_ < parameters::FIELD_SIZE)) {
      if ((deltaX < diameter_ + parameters::INTERVAL) && (deltaX > 0)) {
        setPos(point.x(), y());
      }
    } else if ((index_ > ( parameters::FIELD_SIZE * (parameters::FIELD_SIZE - 1) - 1))) {
      if ((-deltaX < diameter_ + parameters::INTERVAL) && (deltaX < 0)) {
        setPos(point.x(), y());
      }
    } else if (std::fabs(deltaX) < diameter_ + parameters::INTERVAL) {
      setPos(point.x(), y());
    }

  } else if (movingDir_ == parameters::direction_t::VERTICAL) {

    if (index_ % parameters::FIELD_SIZE == 0) {
      if ((deltaY < diameter_ + parameters::INTERVAL) && (deltaY > 0)) {
        setPos(x(), point.y());
      }
    } else if (index_ % parameters::FIELD_SIZE == (parameters::FIELD_SIZE - 1)) {
      if ((-deltaY < diameter_ + parameters::INTERVAL) && (deltaY < 0)) {
        setPos(x(), point.y());
      }
    } else if (std::fabs(deltaY) < diameter_ + parameters::INTERVAL) {
      setPos(x(), point.y());
    }

  }
}

void Node::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
  setCursor(QCursor(Qt::ClosedHandCursor));
  Q_UNUSED(event)
}

void Node::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
  const double deltaX = x() - homePos_.x();
  const double deltaY = y() - homePos_.y();

  setCursor(QCursor(Qt::ArrowCursor));

  initAnimation();

  if (movingDir_ == parameters::direction_t::HORIZONTAL) {

    if (-deltaX > diameter_ + parameters::INTERVAL / 2) {
      if (Nodes::isDestroyable(index_, movingDir_)) {
        Nodes::setDestroyableFlag(true);
        Nodes::setAnimationFlag(true);
        Nodes::swap(index_, index_ - parameters::FIELD_SIZE);
        setRemoveFlags(index_, index_ + parameters::FIELD_SIZE);
      }  
    } else if ((deltaX > diameter_ + parameters::INTERVAL / 2) && (Nodes::isDestroyable(index_, movingDir_))) {
      Nodes::setDestroyableFlag(true);
      Nodes::setAnimationFlag(true);
      Nodes::swap(index_, index_ + parameters::FIELD_SIZE);
      setRemoveFlags(index_, index_ - parameters::FIELD_SIZE);
    }

  } else if (movingDir_ == parameters::direction_t::VERTICAL) {

    if (-deltaY > diameter_ + parameters::INTERVAL / 2) {
      if (Nodes::isDestroyable(index_, movingDir_)) {
        Nodes::setDestroyableFlag(true);
        Nodes::setAnimationFlag(true);
        Nodes::swap(index_, index_ - 1);
        setRemoveFlags(index_, index_ + 1);
      }
    } else if ((deltaY > diameter_ + parameters::INTERVAL / 2) && (Nodes::isDestroyable(index_, movingDir_))) {
      Nodes::setDestroyableFlag(true);
      Nodes::setAnimationFlag(true);
      Nodes::swap(index_, index_ + 1);
      setRemoveFlags(index_, index_ - 1);
    }

  }
  setPos(homePos_);
  Nodes::process();
  Nodes::timer_->start();
  movingDir_ = parameters::direction_t::NONE;

  Q_UNUSED(event)
}
