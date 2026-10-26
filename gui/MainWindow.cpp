#include "MainWindow.h"
#include <QApplication>
#include <QFormLayout>
#include <QFrame>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QStackedWidget>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QTimer>
#include <QRegularExpression>
#include <cmath>

QLineEdit* MainWindow::field(const QString& placeholder, bool password) {
    auto* input = new QLineEdit;
    input->setPlaceholderText(placeholder);
    if (password) input->setEchoMode(QLineEdit::Password);
    return input;
}

MainWindow::MainWindow(Handler handler, QWidget* parent)
    : QMainWindow(parent), handler_(std::move(handler)) {
    setWindowTitle("Secure Password Manager");
    resize(900, 600);
    auto* central = new QWidget(this);
    setCentralWidget(central);
    auto* root = new QHBoxLayout(central);
    root->setContentsMargins(18, 18, 18, 18);
    root->setSpacing(20);

    auto* sidebar = new QFrame;
    sidebar->setObjectName("sidebar");
    sidebar->setFixedWidth(200);
    auto* nav = new QVBoxLayout(sidebar);
    auto* title = new QLabel("SECURE LOGIN");
    title->setObjectName("brand");
    nav->addWidget(title);
    nav->addSpacing(16);
    sessionLabel_ = new QLabel("Not logged in");
    sessionLabel_->setObjectName("sessionLabel");
    sessionLabel_->setWordWrap(true);
    nav->addWidget(sessionLabel_);
    nav->addSpacing(18);
    pages_ = new QStackedWidget;
    const QStringList names = {"Register", "Login", "Change Password", "View Records"};
    for (int i = 0; i < names.size(); ++i) {
        auto* button = new QPushButton(names[i]);
        button->setObjectName("navButton");
        button->setMinimumHeight(42);
        nav->addWidget(button);
        if (i == 2) { changeNavButton_ = button; button->setEnabled(false); }
        connect(button, &QPushButton::clicked, this, [this, i] { pages_->setCurrentIndex(i); status_->clear(); output_->clear(); });
    }
    nav->addStretch();
    logoutButton_ = new QPushButton("Log out");
    logoutButton_->setObjectName("logoutButton");
    logoutButton_->setVisible(false);
    nav->addWidget(logoutButton_);
    connect(logoutButton_, &QPushButton::clicked, this, [this] {
        setSession({});
        pages_->setCurrentIndex(1);
        status_->setText("Logged out successfully.");
        output_->clear();
    });
    nav->addWidget(new QLabel("C++17  |  Qt Widgets"));
    root->addWidget(sidebar);

    auto* content = new QVBoxLayout;
    root->addLayout(content, 1);
    auto addPage = [this](const QString& heading, const QString& description) {
        auto* page = new QWidget;
        auto* layout = new QVBoxLayout(page);
        auto* headingLabel = new QLabel(heading);
        headingLabel->setObjectName("heading");
        layout->addWidget(headingLabel);
        auto* desc = new QLabel(description);
        desc->setWordWrap(true);
        layout->addWidget(desc);
        auto* formBox = new QGroupBox("Details");
        auto* form = new QFormLayout(formBox);
        form->setSpacing(14);
        layout->addWidget(formBox);
        layout->addStretch();
        pages_->addWidget(page);
        return form;
    };

    auto* registration = addPage("Create an account", "Register a PRN with a password.");
    auto* rPrn = field("Enter PRN");
    auto* rPass = field("Enter password", true);
    auto* rConfirm = field("Confirm password", true);
    registration->addRow("PRN", rPrn);
    registration->addRow("Password", rPass);
    registration->addRow("Confirm", rConfirm);
    auto* registerButton = new QPushButton("Register");
    registration->addRow(registerButton);
    connect(registerButton, &QPushButton::clicked, this, [=, this] {
        if (rPrn->text().trimmed().isEmpty() || rPass->text().isEmpty() || rConfirm->text().isEmpty()) { status_->setText("Please complete every field."); return; }
        if (rPass->text() != rConfirm->text()) { status_->setText("Passwords do not match."); return; }
        run(1, {rPrn->text(), rPass->text(), rConfirm->text()});
        rPass->clear(); rConfirm->clear();
    });

    auto* login = addPage("Student login", "Enter your registered PRN and password.");
    auto* lPrn = field("Enter PRN");
    auto* lPass = field("Enter password", true);
    login->addRow("PRN", lPrn);
    login->addRow("Password", lPass);
    auto* loginButton = new QPushButton("Log in");
    loginPrn_ = lPrn;
    loginButton_ = loginButton;
    countdown_ = new QLabel;
    countdown_->setObjectName("countdown");
    countdown_->setVisible(false);
    login->addRow(countdown_);
    countdownTimer_ = new QTimer(this);
    countdownTimer_->setInterval(200);
    connect(countdownTimer_, &QTimer::timeout, this, [this] { refreshCountdown(); });
    countdownTimer_->start();
    connect(lPrn, &QLineEdit::textChanged, this, [this] { refreshCountdown(); });
    login->addRow(loginButton);
    connect(loginButton, &QPushButton::clicked, this, [=, this] {
        if (lPrn->text().trimmed().isEmpty() || lPass->text().isEmpty()) { status_->setText("Enter PRN and password."); return; }
        const QString prn = lPrn->text();
        if (lockouts_.contains(prn) && lockouts_[prn].clock.elapsed() < lockouts_[prn].durationMs) {
            refreshCountdown();
            lPass->clear();
            return;
        }
        run(3, {prn, lPass->text()});
        const QString result = output_->toPlainText();
        updateLockout(prn, result);
        if (result.contains("Login successful", Qt::CaseInsensitive)) {
            setSession(prn);
            status_->setText("Logged in successfully as PRN: " + prn);
            output_->setPlainText("Login successful. Welcome, " + prn + "!");
        }
        lPass->clear();
    });

    auto* change = addPage("Change password", "Verify your current password before setting a new one.");
    auto* cPrn = field("Enter PRN");
    auto* cOld = field("Current password", true);
    auto* cNew = field("New password", true);
    auto* cConfirm = field("Confirm new password", true);
    changePrn_ = cPrn;
    cPrn->setReadOnly(true);
    change->addRow("PRN", cPrn);
    change->addRow("Current password", cOld);
    change->addRow("New password", cNew);
    change->addRow("Confirm", cConfirm);
    auto* changeButton = new QPushButton("Update password");
    change->addRow(changeButton);
    connect(changeButton, &QPushButton::clicked, this, [=, this] {
        if (loggedInPrn_.isEmpty()) { status_->setText("Please log in first."); return; }
        if (cPrn->text().trimmed().isEmpty() || cOld->text().isEmpty() || cNew->text().isEmpty() || cConfirm->text().isEmpty()) { status_->setText("Please complete every field."); return; }
        run(4, {loggedInPrn_, cOld->text(), cNew->text(), cConfirm->text()});
        cOld->clear(); cNew->clear(); cConfirm->clear();
    });

    auto* records = addPage("Stored records", "View the records printed by your existing C++ implementation.");
    auto* refreshButton = new QPushButton("Refresh records");
    records->addRow(refreshButton);
    connect(refreshButton, &QPushButton::clicked, this, [this] { run(2, {}); });

    content->addWidget(pages_, 1);
    status_ = new QLabel;
    status_->setObjectName("status");
    status_->setWordWrap(true);
    content->addWidget(status_);
    output_ = new QTextEdit;
    output_->setReadOnly(true);
    output_->setPlaceholderText("Operation results appear here.");
    output_->setMaximumHeight(165);
    content->addWidget(output_);
    pages_->setCurrentIndex(1);

    setStyleSheet(R"(
      QWidget { font-size: 14px; background: #f8fafc; color: #1e293b; }
      #sidebar { background: #e9eff8; border-radius: 12px; }
      #sessionLabel { padding: 10px; font-weight: bold; color: #1d4ed8; }
      #logoutButton { background: #b91c1c; }
      QPushButton:disabled { background: #cbd5e1; color: #64748b; }
      #brand { font-size: 17px; font-weight: bold; padding: 8px; }
      #heading { font-size: 26px; font-weight: bold; margin-bottom: 6px; }
      #status { font-weight: bold; color: #14532d; padding: 6px; }
      #countdown { font-size: 16px; font-weight: bold; color: #b91c1c; padding: 10px; background: #fee2e2; border-radius: 6px; }
      QGroupBox { border: 1px solid #cbd5e1; border-radius: 10px; margin-top: 14px; padding: 16px; font-weight: bold; }
      QGroupBox::title { subcontrol-origin: margin; left: 12px; padding: 0 5px; }
      QLineEdit { background: white; border: 1px solid #cbd5e1; border-radius: 6px; padding: 10px; }
      QPushButton { background: #1d4ed8; color: white; border: none; border-radius: 7px; padding: 11px; }
      QPushButton:hover { background: #1e40af; }
      #navButton { text-align: left; background: #e0e7f3; color: #1e293b; }
      #navButton:hover { background: #c7d2fe; }
      QTextEdit { background: white; border: 1px solid #cbd5e1; border-radius: 7px; font-family: monospace; }
    )");
}

void MainWindow::run(int action, const QStringList& args) {
    QString result = handler_(action, args).trimmed();
    // Console prompts are already represented by labeled GUI fields.
    // Remove only the known prompt text, preserving all actual result messages.
    static const QRegularExpression prompts(
        R"((?:Enter PRN:|Enter current password:|Enter new password:|Confirm new password:|Enter Password:|Enter password:|Confirm Password:)\s*)",
        QRegularExpression::CaseInsensitiveOption);
    result.replace(prompts, "");
    static const QRegularExpression headings(
        R"(^(?:STUDENT LOGIN|STUDENT REGISTRATION|CHANGE PASSWORD|STUDENT RECORDS)\s*\n?)",
        QRegularExpression::CaseInsensitiveOption);
    result.remove(headings);
    result = result.trimmed();
    status_->setText(result.section('\n', -1));
    output_->setPlainText(result);
}

void MainWindow::updateLockout(const QString& prn, const QString& result) {
    // These strings originate from the unchanged Authentication class.
    static const QRegularExpression started(R"(Login locked for (\d+) seconds)");
    static const QRegularExpression remaining(R"(Try again in (\d+) seconds)");
    auto match = started.match(result);
    if (!match.hasMatch()) match = remaining.match(result);
    if (match.hasMatch()) {
        const int seconds = match.captured(1).toInt();
        Lockout lock;
        lock.durationMs = seconds * 1000;
        lock.clock.start();
        lockouts_.insert(prn, lock);
        // The countdown label replaces the static lockout messages.
        status_->setText("Account temporarily locked.");
        output_->setPlainText("Too many failed login attempts.");
    }
    refreshCountdown();
}

void MainWindow::refreshCountdown() {
    if (!loginPrn_ || !countdown_ || !loginButton_) return;
    const QString prn = loginPrn_->text();
    auto it = lockouts_.find(prn);
    if (it == lockouts_.end()) {
        countdown_->hide();
        loginButton_->setEnabled(true);
        return;
    }
    const int msLeft = it->durationMs - static_cast<int>(it->clock.elapsed());
    if (msLeft <= 0) {
        lockouts_.erase(it);
        countdown_->hide();
        loginButton_->setEnabled(true);
        status_->setText("Lockout expired. You can try logging in again.");
        return;
    }
    const int seconds = (msLeft + 999) / 1000;
    countdown_->setText(QString("Account locked — try again in %1:%2")
        .arg(seconds / 60, 2, 10, QChar('0'))
        .arg(seconds % 60, 2, 10, QChar('0')));
    countdown_->show();
    loginButton_->setEnabled(false);
}

void MainWindow::setSession(const QString& prn) {
    loggedInPrn_ = prn;
    const bool active = !prn.isEmpty();
    sessionLabel_->setText(active ? "Logged in as\nPRN: " + prn : "Not logged in");
    logoutButton_->setVisible(active);
    changeNavButton_->setEnabled(active);
    if (changePrn_) changePrn_->setText(prn);
    if (!active) {
        // Clear any displayed authenticated state. Authentication's existing
        // failure tracking remains untouched and continues across sessions.
        status_->clear();
    }
}
