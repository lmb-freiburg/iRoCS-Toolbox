#include "mainFrame.hh"
#include <QtGui/QApplication>

int
main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  mainFrame w;
  w.show();
  return a.exec();
}
