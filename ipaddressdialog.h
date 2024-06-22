#ifndef IPADDRESSDIALOG_H
#define IPADDRESSDIALOG_H

#include <QDialog>
#include <QDialog>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QDialogButtonBox>

class IpAddressDialog : public QDialog
{
    Q_OBJECT
public:
    IpAddressDialog(QWidget *parent = nullptr);

    QString ipAddress() const { return ipEdit->text(); }
    QString portNumber() const { return portEdit->text(); }

protected:
    void accept() override;

private:
    QLineEdit *ipEdit;
    QLineEdit *portEdit;
};

#endif // IPADDRESSDIALOG_H
