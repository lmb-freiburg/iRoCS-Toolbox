#include "limagelabel.hh"

#include <QtGui/QWheelEvent>

#include <iostream>

lImageLabel::lImageLabel(QWidget *parent) :
  QLabel(parent)
{
  setFocusPolicy(Qt::WheelFocus);
  setMouseTracking(true);
}

lImageLabel::~lImageLabel()
{

}

void
lImageLabel::wheelEvent(QWheelEvent *event)
{

  if (event->modifiers() == Qt::ControlModifier)
  {
    int delta = event->delta();
//    int focusedView = hasFocus();
    //      std::cerr << focusedView << ":" << event->pos ().x() << "," << event->pos().y() << std::endl;
    emit
    zoom(delta);
    event->accept();
  }
  else if (event->modifiers() == Qt::NoModifier)
  {
    int delta = event->delta();
//    int focusedView = hasFocus();
    //      std::cerr << focusedView << ":" << event->pos ().x() << "," << event->pos().y() << std::endl;
    emit
    wheel(delta);
    event->accept();
  }
  else if (event->modifiers() == Qt::ShiftModifier)
  {
    int delta = event->delta();
//    int focusedView = hasFocus();
    //      std::cerr << focusedView << ":" << event->pos ().x() << "," << event->pos().y() << std::endl;
    emit status(delta);
    event->accept();
  }
  QWidget::wheelEvent(event);
}

void
lImageLabel::mouseMoveEvent(QMouseEvent * event)
{
  //  std::cerr << ":" << event->pos ().x() << "," << event->pos().y() << std::endl;
  if (event->modifiers() == Qt::NoModifier)
  {
    int x = event->x();
    int y = event->y();
    emit move(x, y, 0);
  }
  else if (event->buttons() == Qt::LeftButton && event->modifiers() == Qt::ControlModifier)
    {
      int x = event->x();
      int y = event->y();
      emit move(x, y, 1);
//      std::cerr <<"emit move 1" << std::endl;
    }
  QWidget::mouseMoveEvent(event);
}

void
lImageLabel::mouseDoubleClickEvent(QMouseEvent * event)
{
  if (event->modifiers() == Qt::NoModifier)
  {
    int x = event->x();
    int y = event->y();
    emit point(x, y, 0);
  }
  else if (event->button() == Qt::LeftButton && event->modifiers() == Qt::ControlModifier)
  {
    int x = event->x();
    int y = event->y();
    emit point(x, y, 1);
  }
  else if (event->button() == Qt::LeftButton && event->modifiers() == Qt::ShiftModifier)
  {
    int x = event->x();
    int y = event->y();
    emit point(x, y, 11);
  }
  QWidget::mouseDoubleClickEvent(event);
}
void
lImageLabel::mouseReleaseEvent(QMouseEvent * event)
{
  if (event->button() == Qt::LeftButton)
  {
    if (event->modifiers() == Qt::ControlModifier)
    {
      int x = event->x();
      int y = event->y();
      emit point(x, y, 2);
    }
    else if (event->modifiers() == Qt::ShiftModifier)
    {
      int x = event->x();
      int y = event->y();
      emit point(x, y, 3);
    }
    else if (event->modifiers() == Qt::AltModifier)
    {
      int x = event->x();
      int y = event->y();
      emit point(x, y, 4);
    }
  }
  else if (event->button() == Qt::RightButton)
  {
    if (event->modifiers() == Qt::ControlModifier)
    {
      int x = event->x();
      int y = event->y();
      emit point(x, y, 5);
    }
    else if (event->modifiers() == Qt::ShiftModifier)
    {
      int x = event->x();
      int y = event->y();
      emit point(x, y, 6);
    }
    else if (event->modifiers() == Qt::AltModifier)
    {
      int x = event->x();
      int y = event->y();
      emit point(x, y, 7);
    }
  }
  QWidget::mouseReleaseEvent(event);
}
