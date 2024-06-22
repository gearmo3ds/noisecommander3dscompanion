#include "ipaddressdialog.h"
#include <QSettings>

IpAddressDialog::IpAddressDialog(QWidget *parent)
    : QDialog(parent)
{
    // Create the line edits
    ipEdit = new QLineEdit();
    portEdit = new QLineEdit();

    // Set input masks
    // ipEdit->setInputMask("000.000.000.000;_");
    // portEdit->setInputMask("00000;_"); // Optional, restrict to 5 digits

    // Load the last entered IP address and port from settings
    QSettings settings;
    ipEdit->setText(settings.value("lastIpAddress").toString());
    portEdit->setText(settings.value("lastPortNumber").toString());

    // Create labels
    QLabel *ipLabel = new QLabel("IP Address:");
    QLabel *portLabel = new QLabel("Port (optional):");

    // Create layout
    QVBoxLayout *layout = new QVBoxLayout(this);
    QHBoxLayout *ipLayout = new QHBoxLayout();
    QHBoxLayout *portLayout = new QHBoxLayout();

    // Add widgets to layouts
    ipLayout->addWidget(ipLabel);
    ipLayout->addWidget(ipEdit);
    portLayout->addWidget(portLabel);
    portLayout->addWidget(portEdit);

    // Add layouts to main layout
    layout->addLayout(ipLayout);
    layout->addLayout(portLayout);

    // Add OK and Cancel buttons
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &IpAddressDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &IpAddressDialog::reject);
    layout->addWidget(buttonBox);
}

void IpAddressDialog::accept()
{
    // Save the entered IP address and port to settings
    QSettings settings;
    settings.setValue("lastIpAddress", ipEdit->text());
    settings.setValue("lastPortNumber", portEdit->text());

    // Call the base class implementation of accept
    QDialog::accept();
}
