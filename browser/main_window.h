#pragma once

#include <QMainWindow>

class QLineEdit;
class QProgressBar;
class QTabWidget;
class QToolButton;
class QUrl;
class QWebEngineProfile;
class QWebEngineView;

QUrl homeUrl();

class MainWindow final : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    void navigateFromAddressBar();
    void setNavigationStatus(const QString &message);
    void addTab(const QUrl &url);
    void closeTab(int index);
    void syncActiveTab();
    QWebEngineView *activeView() const;

    QWebEngineProfile *profile_ = nullptr;
    QTabWidget *tabs_ = nullptr;
    QLineEdit *addressBar_ = nullptr;
    QProgressBar *progressBar_ = nullptr;
    QToolButton *backButton_ = nullptr;
    QToolButton *forwardButton_ = nullptr;
    QToolButton *reloadButton_ = nullptr;
};
