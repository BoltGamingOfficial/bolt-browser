#pragma once

#include <QString>
#include <QUrl>

struct AddressResolution
{
    QUrl url;
    QString error;

    bool isValid() const { return error.isEmpty() && url.isValid(); }
};

AddressResolution resolveAddressInput(const QString &input);
