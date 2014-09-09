#include "StreamBufTextEditQt4.hh"

#include <libArrayToolbox/ATBTiming.hh>

StreamBufTextEditQt4::StreamBufTextEditQt4(QTextEdit *out)
        : QObject(), std::streambuf(), _insertTimeStamp(true), p_out(out)
{
  p_out->clear();
  
  // No buffer at all, directly write the characters to the output
  setp(NULL, NULL);
  
  connect(this, SIGNAL(textAdded(QString const &)),
          SLOT(appendTextToTextEdit(QString const &)));
}

StreamBufTextEditQt4::~StreamBufTextEditQt4()
{}

StreamBufTextEditQt4::int_type StreamBufTextEditQt4::overflow(int_type ch)
{
  if (ch != traits_type::eof())
  {
    if (traits_type::to_char_type(ch) == '\n') _insertTimeStamp = true;
    else
    {
      if (_insertTimeStamp)
      {
        emit textAdded((atb::MyDateTime::prettyDate() + " - ").c_str());
        _insertTimeStamp = false;
      }
    }
    QString outString(
        std::string(1, traits_type::to_char_type(ch)).c_str());
    emit textAdded(outString);
    return traits_type::to_int_type(true);
  }
  return traits_type::eof();    
}

void StreamBufTextEditQt4::appendTextToTextEdit(QString const &text)
{
  p_out->moveCursor(QTextCursor::End);
  p_out->insertPlainText(text);
  p_out->ensureCursorVisible();
}

StreamBufTextEditQt4::StreamBufTextEditQt4(StreamBufTextEditQt4 const &)
        : QObject(), std::streambuf(), p_out(NULL)
{}

StreamBufTextEditQt4 &StreamBufTextEditQt4::operator=(
    StreamBufTextEditQt4 const &)
{
  return *this;
}

