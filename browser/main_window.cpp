#include "main_window.h"
#include "address_resolver.h"

#include <QCoreApplication>
#include <QDir>
#include <QHBoxLayout>
#include <QKeySequence>
#include <QLineEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QShortcut>
#include <QStandardPaths>
#include <QStatusBar>
#include <QToolButton>
#include <QUrl>
#include <QVBoxLayout>
#include <QWebEngineCertificateError>
#include <QWebEngineHistory>
#include <QWebEnginePage>
#include <QWebEngineProfile>
#include <QWebEngineView>
#include <QWidget>

#include <functional>

namespace {

constexpr auto kDark = "#111318";
constexpr auto kPanel = "#1b1e25";
constexpr auto kGold = "#f5c542";
constexpr auto kMuted = "#9da3af";

QToolButton *makeToolButton(const QString &text, const QString &name, QWidget *parent)
{
    auto *button = new QToolButton(parent);
    button->setText(text);
    button->setObjectName(name);
    button->setCursor(Qt::PointingHandCursor);
    button->setToolButtonStyle(Qt::ToolButtonTextOnly);
    button->setAccessibleName(name);
    button->setMinimumSize(38, 36);
    return button;
}

} // namespace

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setObjectName(QStringLiteral("boltBrowserWindow"));
    setWindowTitle(QStringLiteral("BOLT Browser"));
    resize(1180, 780);
    setMinimumSize(680, 440);

    auto *central = new QWidget(this);
    auto *root = new QVBoxLayout(central);
    root->setContentsMargins(12, 10, 12, 0);
    root->setSpacing(8);

    auto *toolbar = new QHBoxLayout;
    toolbar->setSpacing(6);

    auto *brand = new QPushButton(QStringLiteral("⚡  BOLT"), central);
    brand->setObjectName(QStringLiteral("boltBrand"));
    brand->setToolTip(QStringLiteral("BOLT Browser — open the local test page"));
    brand->setCursor(Qt::PointingHandCursor);
    brand->setMinimumHeight(36);
    toolbar->addWidget(brand);

    backButton_ = makeToolButton(QStringLiteral("‹"), QStringLiteral("backButton"), central);
    backButton_->setToolTip(QStringLiteral("Back (Alt+Left)"));
    toolbar->addWidget(backButton_);

    forwardButton_ = makeToolButton(QStringLiteral("›"), QStringLiteral("forwardButton"), central);
    forwardButton_->setToolTip(QStringLiteral("Forward (Alt+Right)"));
    toolbar->addWidget(forwardButton_);

    reloadButton_ = makeToolButton(QStringLiteral("↻"), QStringLiteral("reloadButton"), central);
    reloadButton_->setToolTip(QStringLiteral("Reload (Ctrl+R or F5)"));
    toolbar->addWidget(reloadButton_);

    auto *homeButton = makeToolButton(QStringLiteral("⌂"), QStringLiteral("homeButton"), central);
    homeButton->setToolTip(QStringLiteral("Home (Alt+Home)"));
    toolbar->addWidget(homeButton);

    addressBar_ = new QLineEdit(central);
    addressBar_->setObjectName(QStringLiteral("addressBar"));
    addressBar_->setPlaceholderText(QStringLiteral("Search or enter a website address"));
    addressBar_->setClearButtonEnabled(true);
    addressBar_->setMinimumHeight(38);
    addressBar_->setAccessibleName(QStringLiteral("Address and search"));
    toolbar->addWidget(addressBar_, 1);

    root->addLayout(toolbar);

    progressBar_ = new QProgressBar(central);
    progressBar_->setObjectName(QStringLiteral("navigationProgress"));
    progressBar_->setTextVisible(false);
    progressBar_->setFixedHeight(3);
    progressBar_->setRange(0, 100);
    progressBar_->hide();
    root->addWidget(progressBar_);

    view_ = new QWebEngineView(central);
    view_->setObjectName(QStringLiteral("webContent"));
    root->addWidget(view_, 1);
    setCentralWidget(central);

    setStyleSheet(QStringLiteral(R"(
        QMainWindow, QWidget#boltBrowserWindow { background: %1; color: #f5f5f5; }
        QWidget { color: #f5f5f5; font-family: "Segoe UI", "Noto Sans", sans-serif; font-size: 10pt; }
        QPushButton#boltBrand { background: qlineargradient(x1:0,y1:0,x2:1,y2:1,stop:0 #2b2922,stop:1 %2); color: %3; border: 1px solid #51462a; border-radius: 12px; padding: 0 16px; font-weight: 750; letter-spacing: 1px; }
        QToolButton { background: %2; color: #d9d7d0; border: 1px solid #303036; border-radius: 12px; font-size: 17pt; }
        QToolButton:hover, QPushButton#boltBrand:hover { background: #29271f; border-color: %3; color: %3; }
        QToolButton:pressed, QPushButton#boltBrand:pressed { background: #38311a; }
        QToolButton:disabled { color: #5c5b60; background: #151517; }
        QLineEdit#addressBar { background: #1a1a1d; border: 1px solid #34343a; border-radius: 15px; padding: 0 16px; selection-background-color: %3; }
        QLineEdit#addressBar:hover { border-color: #55504a; }
        QLineEdit#addressBar:focus { border: 1px solid %3; background: #202023; }
        QProgressBar { background: %2; border: 0; }
        QProgressBar::chunk { background: %3; }
        QStatusBar { background: %1; color: %4; border-top: 1px solid #242428; }
    )").arg(QString::fromLatin1(kDark), QString::fromLatin1(kPanel),
            QString::fromLatin1(kGold), QString::fromLatin1(kMuted)));

    const QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dataDir + QStringLiteral("/profile"));
    QDir().mkpath(dataDir + QStringLiteral("/cache"));
    profile_ = new QWebEngineProfile(QStringLiteral("BOLT Browser"), this);
    profile_->setPersistentStoragePath(dataDir + QStringLiteral("/profile"));
    profile_->setCachePath(dataDir + QStringLiteral("/cache"));
    profile_->setHttpCacheType(QWebEngineProfile::DiskHttpCache);
    profile_->setPersistentCookiesPolicy(QWebEngineProfile::AllowPersistentCookies);

    auto *page = new QWebEnginePage(profile_, view_);
    view_->setPage(page);

    connect(brand, &QPushButton::clicked, this, [this] { view_->setUrl(homeUrl()); });
    connect(backButton_, &QToolButton::clicked, view_, &QWebEngineView::back);
    connect(forwardButton_, &QToolButton::clicked, view_, &QWebEngineView::forward);
    connect(reloadButton_, &QToolButton::clicked, view_, &QWebEngineView::reload);
    connect(homeButton, &QToolButton::clicked, this, [this] { view_->setUrl(homeUrl()); });
    connect(addressBar_, &QLineEdit::returnPressed, this, &MainWindow::navigateFromAddressBar);
    connect(view_, &QWebEngineView::urlChanged, this, [this](const QUrl &url) {
        addressBar_->setText(url.toString(QUrl::FullyEncoded));
        backButton_->setEnabled(view_->history()->canGoBack());
        forwardButton_->setEnabled(view_->history()->canGoForward());
    });
    connect(view_, &QWebEngineView::titleChanged, this, [this](const QString &title) {
        setWindowTitle(title.isEmpty() ? QStringLiteral("BOLT Browser")
                                      : title + QStringLiteral(" — BOLT Browser"));
    });
    connect(view_, &QWebEngineView::loadStarted, this, [this] {
        certificateRejected_ = false;
        progressBar_->setValue(0);
        progressBar_->show();
        setNavigationStatus(QStringLiteral("Loading…"));
    });
    connect(view_, &QWebEngineView::loadProgress, progressBar_, &QProgressBar::setValue);
    connect(view_, &QWebEngineView::loadFinished, this, [this](bool ok) {
        progressBar_->hide();
        if (!certificateRejected_)
            setNavigationStatus(ok ? QStringLiteral("Ready")
                                   : QStringLiteral("Page could not be loaded. Check the address or connection."));
        backButton_->setEnabled(view_->history()->canGoBack());
        forwardButton_->setEnabled(view_->history()->canGoForward());
    });
    connect(page, &QWebEnginePage::certificateError, this,
            [this](QWebEngineCertificateError error) {
                error.rejectCertificate();
                certificateRejected_ = true;
                setNavigationStatus(QStringLiteral("Blocked invalid HTTPS certificate: %1")
                                        .arg(error.url().host()));
            });

    auto addShortcut = [this](const QKeySequence &key, const std::function<void()> &action) {
        auto *shortcut = new QShortcut(key, this);
        shortcut->setContext(Qt::WindowShortcut);
        connect(shortcut, &QShortcut::activated, this, action);
    };
    addShortcut(QKeySequence(Qt::CTRL | Qt::Key_L), [this] {
        addressBar_->setFocus();
        addressBar_->selectAll();
    });
    addShortcut(QKeySequence(Qt::CTRL | Qt::Key_R), [this] { view_->reload(); });
    addShortcut(QKeySequence(Qt::Key_F5), [this] { view_->reload(); });
    addShortcut(QKeySequence(Qt::ALT | Qt::Key_Left), [this] { view_->back(); });
    addShortcut(QKeySequence(Qt::ALT | Qt::Key_Right), [this] { view_->forward(); });
    addShortcut(QKeySequence(Qt::ALT | Qt::Key_Home), [this] { view_->setUrl(homeUrl()); });
    addShortcut(QKeySequence(Qt::Key_Escape), [this] { view_->stop(); });
    addShortcut(QKeySequence(Qt::CTRL | Qt::Key_W), [this] { close(); });

    backButton_->setEnabled(false);
    forwardButton_->setEnabled(false);
    view_->setUrl(homeUrl());
    statusBar()->showMessage(QStringLiteral("BOLT Browser · Powered by Qt WebEngine"));
}

void MainWindow::navigateFromAddressBar()
{
    const AddressResolution result = resolveAddressInput(addressBar_->text());
    if (!result.isValid()) {
        setNavigationStatus(result.error);
        return;
    }
    view_->setUrl(result.url);
}
void MainWindow::setNavigationStatus(const QString &message)
{
    statusBar()->showMessage(message);
}

QUrl homeUrl()
{
    return QUrl::fromLocalFile(QCoreApplication::applicationDirPath()
                               + QStringLiteral("/home.html"));
}
