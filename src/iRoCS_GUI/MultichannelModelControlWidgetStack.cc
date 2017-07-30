/**************************************************************************
 *
 * Copyright (C) 2015 Thorsten Falk
 *
 *        Image Analysis Lab, University of Freiburg, Germany
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 *
 **************************************************************************/

#include "MultichannelModelControlWidgetStack.hh"

#include <libMultiChannelModel/MultichannelModel.hh>
#include "ChannelControlWidget.hh"

namespace iRoCS {

  MultichannelModelControlWidgetStack::MultichannelModelControlWidgetStack(
    MultichannelModel &model, QWidget *parent)
    : QStackedWidget(parent), MultichannelModelObserver(model) {
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "<MultichannelModelControlWidgetStack@" << this
      << "::MultichannelModelControlWidgetStack model=\"" << &model
      << "\" parent=\"" << parent << "\">" << std::endl;
#endif
    updateModel();
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "</MultichannelModelControlWidgetStack@" << this
      << "::MultichannelModelControlWidgetStack>" << std::endl;
#endif
  }

  MultichannelModelControlWidgetStack::~MultichannelModelControlWidgetStack() {
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "<MultichannelModelControlWidgetStack@" << this
      << "::~MultichannelModelControlWidgetStack />" << std::endl;
#endif
  }

  void MultichannelModelControlWidgetStack::updateAddChannel(Channel *channel) {
    if (channel == NULL) return;
    for (int i = 0; i < count(); ++i)
      if (&static_cast<ChannelControlWidget*>(widget(i))->channel() ==
        channel) return;

#ifdef DEBUG_VERBOSE_XML
    std::cerr << "<MultichannelModelControlWidgetStack@" << this
      << "::updateAddChannel channel=\"" << channel << "\">"
      << std::endl;
#endif
    ChannelControlWidget::instance(*channel, this);
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "</MultichannelModelControlWidgetStack@" << this
      << "::updateAddChannel>" << std::endl;
#endif
  }

  void MultichannelModelControlWidgetStack::updateRemoveChannel(
    Channel *channel) {
    if (channel == NULL) return;
    for (int i = 0; i < count(); ++i) {
      if (&static_cast<ChannelControlWidget*>(widget(i))->channel() == channel) {
        delete widget(i);
        return;
      }
    }
  }

  void MultichannelModelControlWidgetStack::updateModified(bool /*modified*/) {}

  void MultichannelModelControlWidgetStack::updateModel() {
    while (count() != 0) delete widget(0);
    for (MultichannelModel::const_iterator it = _model.begin();
      it != _model.end(); ++it) ChannelControlWidget::instance(**it, this);
  }

}
