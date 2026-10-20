#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <ctime>
using namespace std;

// Node for the singly linked list that holds login history entries
struct HistoryNode {
    string prn;
    string timestamp;
    string status; // "SUCCESS" or "FAILED"
    HistoryNode* next;

    HistoryNode(const string& p, const string& t, const string& s)
        : prn(p), timestamp(t), status(s), next(nullptr) {}
};

class LoginHistory {
private:
    HistoryNode* head;

    // Get current date-time as a formatted string
    string getCurrentTimestamp() {
        time_t now = time(nullptr);
        struct tm timeInfo;
        localtime_s(&timeInfo, &now);
        char buffer[20];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeInfo);
        return string(buffer);
    }

    // Free the linked list
    void clearList() {
        while (head) {
            HistoryNode* temp = head;
            head = head->next;
            delete temp;
        }
    }

    // Load history entries from file into the linked list (for a specific PRN, or all if prn is empty)
    void loadFromFile(const string& filterPRN = "") {
        clearList();
        ifstream file("LoginHistory.txt");
        if (!file) return;

        string line;
        getline(file, line); // skip header
        HistoryNode* tail = nullptr;

        while (getline(file, line)) {
            if (line.empty()) continue;
            // Format: PRN | TIMESTAMP | STATUS
            size_t pos1 = line.find('|');
            size_t pos2 = line.find('|', pos1 + 1);
            if (pos1 == string::npos || pos2 == string::npos) continue;

            string prn = line.substr(0, pos1);
            string timestamp = line.substr(pos1 + 1, pos2 - pos1 - 1);
            string status = line.substr(pos2 + 1);

            // Trim whitespace
            while (!prn.empty() && prn.back() == ' ') prn.pop_back();
            while (!prn.empty() && prn.front() == ' ') prn.erase(prn.begin());
            while (!timestamp.empty() && timestamp.back() == ' ') timestamp.pop_back();
            while (!timestamp.empty() && timestamp.front() == ' ') timestamp.erase(timestamp.begin());
            while (!status.empty() && status.back() == ' ') status.pop_back();
            while (!status.empty() && status.front() == ' ') status.erase(status.begin());

            if (!filterPRN.empty() && prn != filterPRN) continue;

            HistoryNode* node = new HistoryNode(prn, timestamp, status);
            if (!head) {
                head = node;
                tail = node;
            } else {
                tail->next = node;
                tail = node;
            }
        }
    }

public:
    LoginHistory() : head(nullptr) {}

    ~LoginHistory() {
        clearList();
    }

    // Record a login attempt to the file
    void recordAttempt(const string& prn, bool success) {
        bool isEmpty = true;
        ifstream checkFile("LoginHistory.txt");
        if (checkFile && checkFile.peek() != EOF) isEmpty = false;
        checkFile.close();

        ofstream file("LoginHistory.txt", ios::app);
        if (!file) {
            cerr << "Error opening LoginHistory.txt\n";
            return;
        }

        if (isEmpty) {
            file << left << setw(20) << "PRN" << "| "
                 << setw(22) << "TIMESTAMP" << "| "
                 << "STATUS\n";
        }

        string ts = getCurrentTimestamp();
        string st = success ? "SUCCESS" : "FAILED";
        file << left << setw(20) << prn << "| "
             << setw(22) << ts << "| "
             << st << '\n';
    }

    // Display login history for a specific PRN using linked list traversal
    void displayHistory() {
        string prn;
        cout << "\nLOGIN HISTORY\n";
        cout << "Enter PRN (or 'all' to view all): ";
        cin >> prn;

        if (prn == "all" || prn == "ALL") {
            loadFromFile();
        } else {
            loadFromFile(prn);
        }

        if (!head) {
            cout << "No login history found\n";
            return;
        }

        cout << '\n' << left << setw(20) << "PRN"
             << setw(22) << "TIMESTAMP"
             << "STATUS\n";
        cout << string(55, '-') << '\n';

        HistoryNode* current = head;
        int count = 0;
        while (current) {
            cout << left << setw(20) << current->prn
                 << setw(22) << current->timestamp
                 << current->status << '\n';
            current = current->next;
            ++count;
        }

        cout << string(55, '-') << '\n';
        cout << "Total entries: " << count << '\n';
    }
};
