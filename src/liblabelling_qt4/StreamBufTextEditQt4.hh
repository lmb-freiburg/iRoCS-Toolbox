#ifndef STREAMBUFTEXTEDITQT4_HH
#define STREAMBUFTEXTEDITQT4_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <streambuf>

#include <QtCore/QObject>
#include <QtGui/QTextEdit>

class StreamBufTextEditQt4 : public QObject, public std::streambuf
{
  
  Q_OBJECT
  
  public:
  
  explicit StreamBufTextEditQt4(QTextEdit *out);
  ~StreamBufTextEditQt4();
  
signals:
  
  void textAdded(QString const &text);
                                     
private slots:
  
  void appendTextToTextEdit(QString const &text);
  
private:
  
  StreamBufTextEditQt4::int_type overflow(int_type ch);
  
  StreamBufTextEditQt4(StreamBufTextEditQt4 const &);
  StreamBufTextEditQt4 &operator=(StreamBufTextEditQt4 const &);
  
  bool _insertTimeStamp;
  QTextEdit *p_out;
  
};

#endif
