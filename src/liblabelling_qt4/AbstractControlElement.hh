#ifndef ABSTRACTCONTROLELEMENT_HH
#define ABSTRACTCONTROLELEMENT_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <QtGui/QWidget>

#include <string>

class QHBoxLayout;
class QLabel;
class QCheckBox;

class AbstractControlElement : public QWidget
{

  Q_OBJECT
  
public:
  
  AbstractControlElement(
      const QString& label, QWidget* parent = 0);
  virtual ~AbstractControlElement();

  void setSwitchable(const bool switchable);
  bool switchable() const;

  void setActive(const bool active);
  bool isActive() const;

  QString label() const;
  void setLabel(const QString& label);

  virtual std::string toStdString() const = 0;

signals:
  
  void activationStateChanged(bool);
  void valueChanged();

public slots:
  
  virtual void setValue(const std::string& value) = 0;

protected:
  
  QWidget* p_control;
  QHBoxLayout* p_outerLayout;
  QHBoxLayout* p_layout;

  QCheckBox* p_switch;
  QLabel* p_label;
  
};

#endif
