#ifndef PLUGININTERFACE_HH
#define PLUGININTERFACE_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <map>
#include <string>

#include <QtCore/QtPlugin>
#include <QtCore/QString>

class LabellingMainWidget;

class PluginInterface
{

public:
  
  virtual ~PluginInterface()
        {}
  
  virtual QString name() const = 0;
  virtual void setLabellingMainWidget(LabellingMainWidget* parent) = 0;
  virtual void setParameters(
      const std::map<std::string,std::string>& parameters) = 0;

  virtual void run() = 0;

  virtual void abort() = 0;

};

Q_DECLARE_INTERFACE(PluginInterface,
                    "lmbsoft.labelling_qt4.PluginInterface/1.0")

#endif
