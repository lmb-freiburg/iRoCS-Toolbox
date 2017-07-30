#include "DoubleSpinBoxDelegate.hh"

#include <QtGui/QDoubleSpinBox>

namespace iRoCS {

  DoubleSpinBoxDelegate::DoubleSpinBoxDelegate(QObject *parent)
    : QStyledItemDelegate(parent) {}

  DoubleSpinBoxDelegate::~DoubleSpinBoxDelegate() {}

  QWidget *DoubleSpinBoxDelegate::createEditor(
    QWidget *parent, QStyleOptionViewItem const & /* option */,
    QModelIndex const &/* index */) const {
    QDoubleSpinBox *editor = new QDoubleSpinBox(parent);
    editor->setAutoFillBackground(true);
    editor->setFrame(false);
    editor->setMinimum(0.0);
    editor->setMaximum(1.0);
    editor->setDecimals(2);
    editor->setSingleStep(0.01);
    return editor;
  }

  void DoubleSpinBoxDelegate::setEditorData(
    QWidget *editor, QModelIndex const &index) const {
    double value = index.model()->data(index, Qt::EditRole).toDouble();

    QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
    spinBox->setValue(value);
  }

  void DoubleSpinBoxDelegate::setModelData(
    QWidget *editor, QAbstractItemModel *model,
    QModelIndex const &index) const {
    QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
    spinBox->interpretText();
    double value = spinBox->value();
    model->setData(index, value, Qt::EditRole);
  }

  void DoubleSpinBoxDelegate::updateEditorGeometry(
    QWidget *editor, QStyleOptionViewItem const &option,
    QModelIndex const &/* index */) const {
    editor->setGeometry(option.rect);
  }

}
