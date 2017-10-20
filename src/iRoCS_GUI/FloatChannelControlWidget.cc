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

#include "FloatChannelControlWidget.hh"

#include <QtGui/QFormLayout>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>

#include <libMultiChannelModel/FloatChannel.hh>
#include "FloatColormapEditorWidget.hh"
#include "MultichannelModelControlWidgetStack.hh"

namespace iRoCS {

  FloatChannelControlWidget::FloatChannelControlWidget(
    FloatChannel &channel,
    MultichannelModelControlWidgetStack *controlStack, bool advancedMode)
    : ChannelControlWidget(channel, controlStack, advancedMode) {
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "<FloatChannelControlWidget@" << this
      << "::FloatChannelControlWidget channel=\"" << &channel
      << "\" controlStack=\"" << controlStack
      << "\" advancedMode = \"" << advancedMode << "\">" << std::endl;
#endif
    FloatColormapEditorWidget *floatColormapEditor =
      new FloatColormapEditorWidget(channel.colormap());
    connect(floatColormapEditor, SIGNAL(normalizeRequest()),
      SLOT(normalizeIndexRange()));
    static_cast<QFormLayout*>(layout())->addRow(floatColormapEditor);
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "</FloatChannelControlWidget@" << this
      << "::FloatChannelControlWidget>" << std::endl;
#endif
  }

  FloatChannelControlWidget::~FloatChannelControlWidget() {
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "<FloatChannelControlWidget@" << this
      << "::~FloatChannelControlWidget />" << std::endl;
#endif
  }

  void FloatChannelControlWidget::updateData() {}

  FloatChannelControlWidget *FloatChannelControlWidget::instance(
    FloatChannel &channel,
    MultichannelModelControlWidgetStack *controlStack, bool advancedMode) {
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "<FloatChannelControlWidget::instance channel=\""
      << typeid(channel).name() << "@" << &channel << "(" << channel.name()
      << ")\" controlStack=\"" << controlStack << "\">" << std::endl;
#endif
    // First check whether an Observer for this channel/stack combination
    // already exists. If so return the pointer to the existing Observer.
    for (std::set<ChannelObserver*>::const_iterator it =
      channel.observers().begin(); it != channel.observers().end(); ++it) {
      FloatChannelControlWidget *controlWidget =
        dynamic_cast<FloatChannelControlWidget*>(*it);
      if (controlWidget != NULL && controlWidget->parent() == controlStack) {
#ifdef DEBUG_VERBOSE_XML
        std::cerr << "return " << controlWidget << " (old instance)"
          << std::endl;
        std::cerr << "</FloatChannelControlWidget::instance>" << std::endl;
#endif
        return controlWidget;
      }
    }
    FloatChannelControlWidget *controlWidget =
      new FloatChannelControlWidget(channel, controlStack, advancedMode);
#ifdef DEBUG_VERBOSE_XML
    std::cerr << "return " << controlWidget << " (new instance)" << std::endl;
    std::cerr << "</FloatChannelControlWidget::instance>" << std::endl;
#endif
    return controlWidget;
  }

  void FloatChannelControlWidget::normalizeIndexRange() {
    FloatChannel &c = dynamic_cast<FloatChannel&>(_channel);
    float minValue = blitz::min(c.data());
    float maxValue = blitz::max(c.data());
    c.colormap().setIndexRange(minValue, maxValue);
  }

}
