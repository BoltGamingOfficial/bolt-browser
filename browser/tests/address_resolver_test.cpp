#include "../address_resolver.h"

#include <QCoreApplication>
#include <QUrlQuery>

#include <iostream>

struct Case { const char *input; const char *expected; };

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    const Case validCases[] = {
        {"example.com", "https://example.com"},
        {"example.com:8080", "https://example.com:8080"},
        {"intranet:3000", "https://intranet:3000"},
        {"localhost", "http://localhost"},
        {"localhost:3000", "http://localhost:3000"},
        {"127.0.0.1:3000", "http://127.0.0.1:3000"},
        {"[::1]:3000", "http://[::1]:3000"},
        {"::1", "http://[::1]"},
        {"2001:db8::1", "https://[2001:db8::1]"},
        {"https://example.com/path", "https://example.com/path"},
    };
    for (const auto &test : validCases) {
        const auto result = resolveAddressInput(QString::fromUtf8(test.input));
        if (!result.isValid() || result.url.toString(QUrl::FullyDecoded) != QString::fromUtf8(test.expected)) {
            std::cerr << "Unexpected resolution for " << test.input << ": "
                      << result.url.toString().toStdString() << " " << result.error.toStdString() << '\n';
            return 1;
        }
    }
    const auto search = resolveAddressInput(QStringLiteral("bolt browser test"));
    if (!search.isValid() || search.url.host() != QStringLiteral("duckduckgo.com")
        || QUrlQuery(search.url).queryItemValue(QStringLiteral("q")) != QStringLiteral("bolt browser test"))
        return 2;

    const char *invalidCases[] = {"javascript:alert(1)", "https://", "example.com:abc", "example.com:70000", "[not-ipv6]:80"};
    for (const char *input : invalidCases) {
        const auto result = resolveAddressInput(QString::fromUtf8(input));
        if (result.isValid()) {
            std::cerr << "Malformed/unsupported input was accepted: " << input << '\n';
            return 3;
        }
    }
    return 0;
}
