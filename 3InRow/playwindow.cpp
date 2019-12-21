#include "playwindow.hpp"

#include <QPainter>
#include <QColor>
#include <QStyleFactory>

#include "nodes.hpp"
#include "utils.hpp"
#include "scene-singletone.hpp"
#include "singletone.hpp"

static Qt::GlobalColor generateUniqueColor(size_t index)
{
  Qt::GlobalColor color = Nodes::randomiseNodeColor();

  while (index > 0 && index < parameters::FIELD_SIZE && (Singletone::instance()->at(index - 1)->getColor() == color)) {
    color = Nodes::randomiseNodeColor();
  }

  while ((index > parameters::FIELD_SIZE - 1) && ((Singletone::instance()->at(index - parameters::FIELD_SIZE)->getColor() == color)
      || (Singletone::instance()->at(index - 1)->getColor() == color))) {

    color = Nodes::randomiseNodeColor();
  }
  return color;
}

PlayWindow::PlayWindow(QWidget * parent) :
  QGraphicsView(parent),
  remainimgTimeMSec_(60000)
{
  SceneSingletone::instance()->setItemIndexMethod(QGraphicsScene::NoIndex);
  setScene(SceneSingletone::instance());
  setRenderHint(QPainter::Antialiasing);
  setCacheMode(QGraphicsView::CacheBackground);
  setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  const size_t verticalOffset = 160;
  const size_t horizontalOffset = 40;
  size_t width = horizontalOffset + parameters::FIELD_SIZE * (parameters::DIAMETER + parameters::INTERVAL);
  size_t height = verticalOffset + parameters::FIELD_SIZE * (parameters::DIAMETER + parameters::INTERVAL);

  SceneSingletone::instance()->setSceneRect(0, 0, width, height);
  Singletone::instance();

  textItems_["timeLeft"] = new QGraphicsSimpleTextItem();
  textItems_["timeLeft"]->setPos(25, 18);
  textItems_["timeLeft"]->setFont(QFont("Times", 25, QFont::Light));
  textItems_["timeLeft"]->setText("Time left: " + QString::number(remainimgTimeMSec_ / 1000));
  SceneSingletone::instance()->addItem(textItems_["timeLeft"]);

  textItems_["score"] = new QGraphicsSimpleTextItem();
  textItems_["score"]->setPos(465, 40);
  textItems_["score"]->setFont(QFont("Times", 40, QFont::Bold));
  textItems_["score"]->setText(QString::number(Nodes::getScore()));
  SceneSingletone::instance()->addItem(textItems_["score"]);

  textItems_["pause"] = new QGraphicsSimpleTextItem();
  textItems_["pause"]->setPos(275, 75);
  textItems_["pause"]->setFont(QFont("Times", 25, QFont::Bold));
  textItems_["pause"]->setText("||");

  QPen pen;
  pen.setColor("orange");
  pen.setWidth(3);
  pen.setCapStyle(Qt::SquareCap);
  pen.setStyle(Qt::SolidLine);
  textItems_["pause"]->setPen(pen);
  textItems_["pause"]->setBrush(QBrush(Qt::yellow, Qt::SolidPattern));
  SceneSingletone::instance()->addItem(textItems_["pause"]);
  textItems_["pause"]->hide();

  int buttonWidth = 200;
  int buttomHeight = 50;

  buttons_["restart"] = new QPushButton("Restart", this);

  buttons_["restart"]->setFont(QFont("Roman", 31, QFont::Bold));
  buttons_["restart"]->setGeometry(200, 300, buttonWidth, buttomHeight);
  buttons_["restart"]->setStyle(QStyleFactory::create("Fusion"));
  buttons_["restart"]->hide();
  connect(buttons_["restart"], SIGNAL(clicked()), SLOT(onRestartButtonClicked()));

  buttons_["quit"] = new QPushButton("Quit", this);

  buttons_["quit"]->setFont(QFont("Roman", 31, QFont::Bold));
  buttons_["quit"]->setGeometry(200, 400, buttonWidth, buttomHeight);
  buttons_["quit"]->setStyle(QStyleFactory::create("Fusion"));
  buttons_["quit"]->hide();
  connect(buttons_["quit"], SIGNAL(clicked()), SLOT(onQuitButtonClicked()));

  timer_ = new QTimer();
  connect(timer_, SIGNAL(timeout()), this, SLOT(slotTimerAlarm()));

  srand(time(0));

  for (size_t i = 0; i < parameters::FIELD_SIZE * parameters::FIELD_SIZE; i++) {
    Qt::GlobalColor color = generateUniqueColor(i);
    Node * item = new Node(color, i, parameters::DIAMETER);
    item->setPos(horizontalOffset + parameters::DIAMETER / 2
        + (i / parameters::FIELD_SIZE) * (parameters::DIAMETER + parameters::INTERVAL),
          verticalOffset + parameters::DIAMETER / 2
          + (i % parameters::FIELD_SIZE) * (parameters::DIAMETER + parameters::INTERVAL));

    item->setHomePos(item->pos());

    Nodes::add(item);
    SceneSingletone::instance()->addItem(item);
  }

  timer_->start(1);
}

PlayWindow::~PlayWindow()
{
  delete SceneSingletone::instance();
}

void PlayWindow::slotTimerAlarm()
{
  textItems_["timeLeft"]->setText("Time left: " + QString::number(--remainimgTimeMSec_ / 1000));
  textItems_["score"]->setText(QString::number(Nodes::getScore()));

  if (Nodes::getAnimationFlag()) {
    setInteractive(false);
    textItems_["pause"]->show();
  } else {
    textItems_["pause"]->hide();
    setInteractive(true);
  }

  if (remainimgTimeMSec_ == 10000) {
    textItems_["timeLeft"]->setPen(QPen(Qt::red, 1, Qt::SolidLine, Qt::FlatCap));
    textItems_["timeLeft"]->setBrush(QBrush(Qt::red, Qt::SolidPattern));
  }
  if (remainimgTimeMSec_ == 0) {
    textItems_["pause"]->hide();
    timer_->stop();
    textItems_["timeLeft"]->setText("Time's up!");


    std::for_each(Singletone::instance()->begin(), Singletone::instance()->end(), [](Node * node) { node->hide(); } );
    buttons_["restart"]->show();
    buttons_["quit"]->show();
  }
}

void PlayWindow::onRestartButtonClicked()
{
  buttons_["restart"]->hide();
  buttons_["quit"]->hide();

  Nodes::resetScore();

  textItems_["timeLeft"]->setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::FlatCap));
  textItems_["timeLeft"]->setBrush(QBrush(Qt::black, Qt::SolidPattern));

  std::for_each(Singletone::instance()->begin(), Singletone::instance()->end(), [](Node * node) {
      node->setColor(generateUniqueColor(node->getIndex()));
      node->show();
  });

  remainimgTimeMSec_ = 60000;
  timer_->start(1);
}

void PlayWindow::onQuitButtonClicked()
{
  exit(0);
}

