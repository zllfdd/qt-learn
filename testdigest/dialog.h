#ifndef DIALOG_H
#define DIALOG_H

#include <QCryptographicHash>
#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class Dialog; }
QT_END_NAMESPACE

class Dialog : public QDialog
{
    Q_OBJECT

public:
    Dialog(QWidget *parent = nullptr);
    ~Dialog();

private:
    bool m_hash = true;
    bool m_hex = true;
    Ui::Dialog *ui;
    QCryptographicHash::Algorithm m_algorithm = QCryptographicHash::Md5;
    bool m_text = true;
};
#endif // DIALOG_H
