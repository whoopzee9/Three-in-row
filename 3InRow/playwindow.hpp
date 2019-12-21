#ifndef PLAYWINDOW_HPP
#define PLAYWINDOW_HPP

#include <QGraphicsView>
#include <QTimer>
#include <QPushButton>
#include <unordered_map>

class Node;

class PlayWindow : public QGraphicsView {
  Q_OBJECT
public:
  explicit PlayWindow(QWidget * parent = nullptr);
  ~PlayWindow();

private slots:
  void slotTimerAlarm();
  void onRestartButtonClicked();
  void onQuitButtonClicked();

private:
  QTimer * timer_;
  std::unordered_map< std::string, QGraphicsSimpleTextItem * > textItems_;
  std::unordered_map< std::string, QPushButton * > buttons_;
  size_t remainimgTimeMSec_;
};

#endif
