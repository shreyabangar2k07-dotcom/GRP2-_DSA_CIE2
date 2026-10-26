#include <QApplication>
#include <QMessageBox>
#include <QDir>
#include <QStringList>
#include <QRegularExpression>
#include <sstream>
#include <iostream>
#include <sodium.h>
#include "MainWindow.h"

// Original implementations are included, not edited. They use cin/cout directly.
#include "../core/Registration.cpp"
#include "../core/Authentication.cpp"
#include "../core/PasswordChange.cpp"
#include "../core/UserRecords.cpp"

class ConsoleAdapter {
    Regstration registration_;
    Authentication authentication_; // persistent so lockout counters survive button clicks
    PasswordChange passwordChange_;
    UserRecords records_;
public:
    QString invoke(int action, const QStringList& args) {
        // Existing code reads whitespace-delimited tokens using operator>>.
        // Reject whitespace to avoid feeding one field into another prompt.
        for (const auto& arg : args) {
            if (arg.isEmpty() || arg.contains(QRegularExpression("\\s")))
                return "Inputs cannot be empty or contain spaces in this version.";
        }
        std::ostringstream input;
        for (const auto& arg : args) input << arg.toStdString() << '\n';
        std::istringstream redirectedInput(input.str());
        std::ostringstream capturedOutput;
        auto* oldInput = std::cin.rdbuf(redirectedInput.rdbuf());
        auto* oldOutput = std::cout.rdbuf(capturedOutput.rdbuf());
        auto* oldError = std::cerr.rdbuf(capturedOutput.rdbuf());
        std::cin.clear();
        try {
            switch (action) {
                case 1: registration_.registerUser(); break;
                case 2: records_.displayRecords(); break;
                case 3: authentication_.authenticateUser(); break;
                case 4: passwordChange_.changePassword(); break;
                default: capturedOutput << "Unknown action\n";
            }
        } catch (const std::exception& e) {
            capturedOutput << "Error: " << e.what() << '\n';
        } catch (...) {
            capturedOutput << "Unexpected error\n";
        }
        std::cin.rdbuf(oldInput);
        std::cout.rdbuf(oldOutput);
        std::cerr.rdbuf(oldError);
        std::cin.clear();
        return QString::fromStdString(capturedOutput.str());
    }
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    if (sodium_init() < 0) {
        QMessageBox::critical(nullptr, "Initialization error", "Unable to initialize libsodium.");
        return 1;
    }
    ConsoleAdapter adapter;
    MainWindow window([&adapter](int action, const QStringList& args) { return adapter.invoke(action, args); });
    window.show();
    return app.exec();
}
