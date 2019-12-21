#ifndef NODES_HPP
#define NODES_HPP

#include <vector>
#include <functional>
#include <QTimeLine>

#include "node.hpp"
#include "utils.hpp"

class Nodes {
public:
  static QTimeLine * timer_;
  static QTimeLine * replaceTimer_;

  Nodes();

  static void add(Node *);
  static void swap(size_t, size_t);
  static void process();
  static void setDestroyableFlag(bool);
  static void setAnimationFlag(bool);
  static void addToScore(size_t);

  static bool getAnimationFlag();
  static size_t getScore();
  static bool getDestroyableFlag();

  static void resetScore();
  static bool isDestroyable(size_t, parameters::direction_t);
  static void setDestroyFlags(parameters::direction_t, size_t);
  static Qt::GlobalColor randomiseNodeColor();

  static void generateNewNodes();

private:
  static bool isDestroyable_;
  static bool isAnimationGoing_;
  static size_t score_;
  static bool isSideDestroyable(size_t index, size_t newIndex, int);
  static bool isNeighborsDestroyable(size_t index, size_t newIndex, int);
  static bool isSpecialCaseDestroyable(size_t index, size_t newIndex, size_t coordPlace, int radius);
  static bool isDefaultCaseDestroyable(size_t index, size_t newIndex, size_t coordPlace, int mainRadius, int sideRadius);
};

#endif
