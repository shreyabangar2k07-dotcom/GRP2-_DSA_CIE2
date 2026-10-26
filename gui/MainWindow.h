#pragma once
#include <QMainWindow>
#include <QString>
#include <QStringList>
#include <functional>
#include <QElapsedTimer>
#include <QHash>

class QLineEdit;
class QLabel;
class QTextEdit;
class QStackedWidget;
class QPushButton;
class QTimer;

class MainWindow : public QMainWindow {
public:
    using Handler = std::function<QString(int, const QStringList&)>;
    explicit MainWindow(Handler handler, QWidget* parent = nullptr);
private:
    Handler handler_;
    QStackedWidget* pages_{};
    QLabel* status_{};
    QTextEdit* output_{};
    QLabel* countdown_{};
    QPushButton* loginButton_{};
    QLineEdit* loginPrn_{};
    QTimer* countdownTimer_{};
    QLabel* sessionLabel_{};
    QPushButton* logoutButton_{};
    QPushButton* changeNavButton_{};
    QLineEdit* changePrn_{};
    QString loggedInPrn_;
    void setSession(const QString& prn);
    struct Lockout { QElapsedTimer clock; int durationMs; };
    QHash<QString, Lockout> lockouts_;
    void refreshCountdown();
    void updateLockout(const QString& prn, const QString& result);
    void run(int action, const QStringList& args);
    QLineEdit* field(const QString& placeholder, bool password = false);
};
