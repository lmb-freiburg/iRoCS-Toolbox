#include "LineEditDelegate.hh"

#include <QtGui/QLineEdit>

namespace iRoCS {

  LineEditDelegate::LineEditDelegate(QObject *parent)
    : QStyledItemDelegate(parent) {}

  LineEditDelegate::~LineEditDelegate() {}

  QWidget *LineEditDelegate::createEditor(
    QWidget *parent, QStyleOptionViewItem const & /* option */,
    QModelIndex const &/* index */) const {
    QLineEdit *editor = new QLineEdit(parent);
    editor->setAutoFillBackground(true);
    editor->setFrame(false);
    return editor;
  }

  void LineEditDelegate::setEditorData(
    QWidget *editor, QModelIndex const &index) const {
    QString value = index.model()->data(index, Qt::EditRole).toString();

    QLineEdit *lineEdit = static_cast<QLineEdit*>(editor);
    lineEdit->setText(value);
  }

  void LineEditDelegate::setModelData(
    QWidget *editor, QAbstractItemModel *model,
    QModelIndex const &index) const {
    QLineEdit *lineEdit = static_cast<QLineEdit*>(editor);
    QString value = lineEdit->text();
    model->setData(index, value, Qt::EditRole);
  }

  void LineEditDelegate::updateEditorGeometry(
    QWidget *editor, QStyleOptionViewItem const &option,
    QModelIndex const &/* index */) const {
    editor->setGeometry(option.rect);
  }

}
