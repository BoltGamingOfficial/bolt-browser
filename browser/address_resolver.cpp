#include "address_resolver.h"

#include <QAbstractSocket>
#include <QHostAddress>
#include <QRegularExpression>
#include <QUrlQuery>

namespace {

AddressResolution invalid(const QString &message)
{
    return {{}, message};
}

bool validPort(const QString &port)
{
    bool ok = false;
    const uint value = port.toUInt(&ok);
    return ok && value > 0 && value <= 65535;
}

bool isLoopback(const QString &host)
{
    if (host.compare(QStringLiteral("localhost"), Qt::CaseInsensitive) == 0
        || host.endsWith(QStringLiteral(".localhost"), Qt::CaseInsensitive))
        return true;
    QHostAddress address;
    return address.setAddress(host) && address.isLoopback();
}

AddressResolution makeWebUrl(const QString &candidate)
{
    const QUrl url(candidate, QUrl::StrictMode);
    if (!url.isValid() || url.host().isEmpty()
        || (url.scheme() != QStringLiteral("http") && url.scheme() != QStringLiteral("https")))
        return invalid(QStringLiteral("That address is malformed or uses an unsupported scheme."));
    if (url.port(-1) == 0 || url.port(-1) > 65535)
        return invalid(QStringLiteral("The address contains an invalid port."));
    return {url, {}};
}

} // namespace

AddressResolution resolveAddressInput(const QString &rawInput)
{
    const QString input = rawInput.trimmed();
    if (input.isEmpty())
        return invalid(QStringLiteral("Enter a website address or search query."));

    const QRegularExpression explicitWebScheme(QStringLiteral("^https?://"),
                                               QRegularExpression::CaseInsensitiveOption);
    if (explicitWebScheme.match(input).hasMatch())
        return makeWebUrl(input);

    // An explicit scheme is never silently reinterpreted as a search or host.
    const bool hostPortForm = QRegularExpression(QStringLiteral("^[^/:]+:[0-9]+(?:/|$)")).match(input).hasMatch();
    if (QRegularExpression(QStringLiteral("^[A-Za-z][A-Za-z0-9+.-]*:")).match(input).hasMatch()
        && !hostPortForm && !input.startsWith(QStringLiteral("::")))
        return invalid(QStringLiteral("Only HTTP and HTTPS addresses are supported."));

    if (input.contains(QRegularExpression(QStringLiteral("\\s")))) {
        QUrl search(QStringLiteral("https://duckduckgo.com/"));
        QUrlQuery query;
        query.addQueryItem(QStringLiteral("q"), input);
        search.setQuery(query);
        return {search, {}};
    }

    QString hostAndPath = input;
    QString hostOnly = input.section(QLatin1Char('/'), 0, 0).section(QLatin1Char('?'), 0, 0)
                           .section(QLatin1Char('#'), 0, 0);
    bool ipv6 = false;
    QHostAddress address;

    if (hostOnly.startsWith(QLatin1Char('['))) {
        const qsizetype close = hostOnly.indexOf(QLatin1Char(']'));
        if (close < 0 || !address.setAddress(hostOnly.mid(1, close - 1))
            || address.protocol() != QAbstractSocket::IPv6Protocol)
            return invalid(QStringLiteral("The bracketed IPv6 address is malformed."));
        const QString suffix = hostOnly.mid(close + 1);
        if (!suffix.isEmpty() && (!suffix.startsWith(QLatin1Char(':')) || !validPort(suffix.mid(1))))
            return invalid(QStringLiteral("The address contains an invalid port."));
        ipv6 = true;
    } else if (hostOnly.contains(QLatin1Char(':'))) {
        if (address.setAddress(hostOnly) && address.protocol() == QAbstractSocket::IPv6Protocol) {
            ipv6 = true;
            hostAndPath = QStringLiteral("[") + hostOnly + QStringLiteral("]");
        } else {
            const qsizetype colon = hostOnly.lastIndexOf(QLatin1Char(':'));
            const QString possiblePort = hostOnly.mid(colon + 1);
            const QString possibleHost = hostOnly.left(colon);
            if (!possiblePort.isEmpty() && possiblePort.contains(QRegularExpression(QStringLiteral("\\D"))))
                return invalid(QStringLiteral("The address contains an invalid port."));
            if (possibleHost.isEmpty() || !validPort(possiblePort))
                return invalid(QStringLiteral("The address contains an invalid host or port."));
            hostOnly = possibleHost;
        }
    }

    const bool hasPathOrSuffix = input.contains(QLatin1Char('/')) || input.contains(QLatin1Char('?'))
        || input.contains(QLatin1Char('#'));
    if (!ipv6 && !hostOnly.contains(QLatin1Char('.')) && !hostPortForm && !hasPathOrSuffix
        && hostOnly.compare(QStringLiteral("localhost"), Qt::CaseInsensitive) != 0) {
        QUrl search(QStringLiteral("https://duckduckgo.com/"));
        QUrlQuery query;
        query.addQueryItem(QStringLiteral("q"), input);
        search.setQuery(query);
        return {search, {}};
    }

    QString scheme = isLoopback(ipv6 ? address.toString() : hostOnly)
            ? QStringLiteral("http") : QStringLiteral("https");
    // Bracket a bare IPv6 literal before attaching its scheme; retain any supplied path.
    if (ipv6 && !hostOnly.startsWith(QLatin1Char('['))) {
        const qsizetype suffixAt = input.indexOf(QLatin1Char('/'));
        if (suffixAt >= 0)
            hostAndPath += input.mid(suffixAt);
    }
    return makeWebUrl(scheme + QStringLiteral("://") + hostAndPath);
}
