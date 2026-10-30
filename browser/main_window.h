#pragma once

#include <QMainWindow>

class QLineEdit;
class QProgressBar;
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

    QWebEngineProfile *profile_ = nullptr;
    QWebEngineView *view_ = nullptr;
    QLineEdit *addressBar_ = nullptr;
    QProgressBar *progressBar_ = nullptr;
    QToolButton *backButton_ = nullptr;
    QToolButton *forwardButton_ = nullptr;
    QToolButton *reloadButton_ = nullptr;
    bool certificateRejected_ = false;
};
