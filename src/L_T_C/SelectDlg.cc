#include "SelectDlg.hh"

#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QCheckBox>

SelectDlg::SelectDlg(QWidget* parent,
                     std::map<std::string, bool> &chooseFields,
                     QString cpt)
        : QDialog(parent),
          _chooseFields(&chooseFields)
{
//  setCaption(cpt);

  _toggleAllButton = new QPushButton(tr("Toggle All"), this);
  _toggleAllButton->setMaximumSize(100, 40);

  _okButton = new QPushButton(tr("OK"), this);
  _okButton->setMaximumSize(100, 40);
  _okButton->setDefault(true);

  QVBoxLayout *vLayout = new QVBoxLayout(this);
  QLabel *infoText = new QLabel(this);

  infoText->setText(tr("Please select the ") + cpt + ":");
  vLayout->setMargin(20);
  vLayout->addWidget(infoText);

  // Create Checkboxes and compute needed scroll content extents
//  QScrollArea* scrollView = new QScrollArea(this);
//  scrollView->enableClipper(true);
  int scrollWidth = -1;
  int scrollHeight = 0;
  std::map<std::string, bool>::iterator iter;
  for (iter = _chooseFields->begin(); iter != _chooseFields->end(); ++iter)
  {
    _checkBoxes[iter->first] = new QCheckBox();
    _checkBoxes[iter->first]->setText(iter->first.c_str());
    _checkBoxes[iter->first]->setChecked(false);//iter->second);
    _checkBoxes[iter->first]->resize(_checkBoxes[iter->first]->sizeHint());
    if (_checkBoxes[iter->first]->width() > scrollWidth)
    {
      scrollWidth = _checkBoxes[iter->first]->width();
    }
    scrollHeight += _checkBoxes[iter->first]->height();
  }

//  scrollView->viewport()->setBackgroundMode(Qt::PaletteBackground);
//  scrollView->resizeContents(scrollWidth, scrollHeight);
  int currentPos = 0;
  for (iter = _chooseFields->begin(); iter != _chooseFields->end(); ++iter)
  {
    vLayout->addWidget(_checkBoxes[iter->first]);
    currentPos += _checkBoxes[iter->first]->height();
  }

//  vLayout->addWidget(scrollView);
  vLayout->addWidget(_toggleAllButton);
  vLayout->addWidget(_okButton);

  connect(_okButton, SIGNAL(clicked()), this, SLOT(accept()));
  connect(_toggleAllButton, SIGNAL(clicked()), this, SLOT(toggleAll()));

  vLayout->activate();

}

void SelectDlg::toggleAll()
{
  std::map<std::string, bool>::iterator iter = _chooseFields->begin();
  for (; iter != _chooseFields->end(); ++iter)
  {
    _checkBoxes[iter->first]->setChecked(!_checkBoxes[iter->first]->isChecked());
  }

}
void SelectDlg::unselectAll()
{
  std::map<std::string, bool>::iterator iter = _chooseFields->begin();
  for (; iter != _chooseFields->end(); ++iter)
  {
    _checkBoxes[iter->first]->setChecked(false);
    iter->second = false;
  }

}
void SelectDlg::selectOnlyOneVar()
{
  std::map<std::string, bool>::iterator iter = _chooseFields->begin();
  for (; iter != _chooseFields->end(); ++iter)
  {
    if(_checkBoxes[iter->first]->isChecked() && iter->second)
    {
      iter->second = false;
      _checkBoxes[iter->first]->setChecked(false);
    }
    else if(_checkBoxes[iter->first]->isChecked() && !(iter->second))
    {
      iter->second = true;
    }
  }

}
void SelectDlg::accept()
{
  std::map<std::string, QCheckBox*>::iterator boxIter = _checkBoxes.begin();
  for (; boxIter != _checkBoxes.end(); ++boxIter)
  {
    if (boxIter == _checkBoxes.end()){
      std::cerr << "Error while copying checkboxes values.";
      return;
    }

    (*_chooseFields)[boxIter->first] = boxIter->second->isChecked();
  }
  QDialog::accept();
}

void SelectDlg::makeOneSelectionDialog()
{

    unselectAll();
    std::map<std::string, QCheckBox*>::iterator boxIter = _checkBoxes.begin();
    for (; boxIter != _checkBoxes.end(); ++boxIter)
    {
      connect(boxIter->second, SIGNAL(toggled(bool)),
              this, SLOT(selectOnlyOneVar()));
    }
    _toggleAllButton->hide();
}
