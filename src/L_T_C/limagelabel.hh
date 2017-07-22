#ifndef LIMAGELABEL_H
#define LIMAGELABEL_H

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <QtGui/QWidget>
#include <QtGui/QLabel> //For Base Class

class QLabel;

class lImageLabel : public QLabel
{
    Q_OBJECT

public:
    lImageLabel(QWidget *parent = 0);
    ~lImageLabel();

public slots:

signals:
      void wheel(int delta);
      void zoom(int delta);
      void move(int x, int y, int flag);
      void point(int x, int y, int flag);
      void status(int delta);


protected:
    void wheelEvent(QWheelEvent *event);
    void mouseMoveEvent(QMouseEvent * event);
    void mouseDoubleClickEvent ( QMouseEvent * event );
    void mouseReleaseEvent ( QMouseEvent * event );
};

#endif // LIMAGELABEL_H
