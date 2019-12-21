#ifndef UTILITS_HPP
#define UTILITS_HPP

#include <iostream>
#include <QPainter>

namespace parameters {

  enum class direction_t {
    VERTICAL,
    HORIZONTAL,
    NONE
  };

  enum class color_t {
    RED = Qt::red,
    GREEN = Qt::green,
    BLUE = Qt::blue,
    YELLOW = Qt::yellow,
    MAGENTA = Qt::magenta
  };

  static const size_t FIELD_SIZE = 8;
  static const size_t INTERVAL = 20;
  static const size_t DIAMETER = 50;
  static const size_t ANIMATION_PERIOD_MS = 200;

}

#endif
