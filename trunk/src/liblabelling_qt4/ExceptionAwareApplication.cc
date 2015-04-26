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

#include "ExceptionAwareApplication.hh"

#include <QtGui/QMessageBox>

#include <iostream>

ExceptionAwareApplication::ExceptionAwareApplication(int &argc, char **argv)
        : QApplication(argc, argv)
{}

ExceptionAwareApplication::~ExceptionAwareApplication()
{}

bool ExceptionAwareApplication::notify(QObject *receiver, QEvent *e)
{
  try
  {
    return QApplication::notify(receiver, e);
  }
  catch (std::bad_alloc &e)
  {
    QMessageBox::critical(
        activeWindow(), tr("Out of memory"),
        tr("An unhandled out-of-memory error occured while processing a GUI "
           "event.\n"
           "You do not have enough system memory to perform the requested \n"
           "operation. The bad news is, the problem was detected too late \n"
           "and your project is probably lost. Sorry! :( \n"
           "Please report this problem and the steps leading to it to the "
           "package mainainer.\n\n"
           "Exception text: "));
    std::cerr << "Out-of-memory error caught in event handler" << std::endl;
    std::cerr << "Report this bug!" << std::endl;
  }
  catch (std::exception &e)
  {
    QMessageBox::critical(
        activeWindow(), tr("Exception caught in event handler"),
        tr("An unhandled exception occured while processing a GUI event. \n"
           "This may not happen and if you see this message you were lucky \n"
           "that the application did not crash so far!\n"
           "Please send a bug report including steps to reproduce this \n"
           "error to the package maintainer.\n\n"
           "Exception text: ").arg(e.what()));
    std::cerr << "Exception caught in event handler: " << e.what()
              << std::endl;
    std::cerr << "Report this bug!" << std::endl;
  }
  catch (...)
  {
    QMessageBox::critical(
        activeWindow(), tr("Exception caught in event handler"),
        tr("An unhandled exception occured while processing a GUI event. \n"
           "This may not happen and if you see this message you were lucky \n"
           "that the application did not crash so far!\n"
           "Please send a bug report including steps to reproduce this \n"
           "error to the package maintainer."));
    std::cerr << "Unknown exception caught in event handler! Report this as "
              << "a bug!" << std::endl;
  }
  return false;
}
