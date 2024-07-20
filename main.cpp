#include <iostream>
#include <vector>
#include <unordered_map>
#include <random>
#include <chrono>
#include <thread>

using namespace std;

const int MAX_LINES = 3;
const int MAX_BET = 1000;
const int MIN_BET = 1;

const int ROWS = 3;
const int COLS = 3;

unordered_map<string, int> symbol_count = {
    {"7    ", 2},
    {"BAR  ", 4},
    {"APPLE", 6},
    {"BELL ", 7}
};

unordered_map<string, int> symbol_value = {
    {"7    ", 5},
    {"BAR  ", 4},
    {"APPLE", 3},
    {"BELL ", 2}
};

pair<int, vector<int>> check_win(const vector<vector<string>> &columns, int lines, int bet, const unordered_map<string, int> &symbol_value) {
    int winnings = 0;
    vector<int> winning_line;
    for (int line = 0; line < lines; line++) {
        string symbol = columns[0][line];
        bool win = true;
        for (const auto &column : columns) {
            if (symbol != column[line]) {
                win = false;
                break;
            }
        }
        if (win) {
            winnings += symbol_value.at(symbol) * bet;
            winning_line.push_back(line + 1);
        }
    }
    return {winnings, winning_line};
}

vector<vector<string>> get_machine_spin_output(int rows, int cols, const unordered_map<string, int> &symbol_count) {
    vector<string> all_symbols;
    for (const auto &[symbol, count] : symbol_count) {
        for (int i = 0; i < count; i++) {
            all_symbols.push_back(symbol);
        }
    }

    vector<vector<string>> columns(cols);
    random_device rd;
    mt19937 gen(rd());

    for (int i = 0; i < cols; i++) {
        vector<string> current_symbols = all_symbols;
        for (int j = 0; j < rows; j++) {
            uniform_int_distribution<> dis(0, current_symbols.size() - 1);
            int index = dis(gen);
            columns[i].push_back(current_symbols[index]);
            current_symbols.erase(current_symbols.begin() + index);
        }
    }
    return columns;
}

void print_slot_machine_output(const vector<vector<string>>& columns) {
    for (int row = 0; row < columns[0].size(); row++) {
        for (int i = 0; i < columns.size(); i++) {
            cout << columns[i][row];
            if (i != columns.size() - 1) {
                cout << " | ";
                this_thread::sleep_for(chrono::milliseconds(500));
            }
        }
        cout << endl;
        this_thread::sleep_for(chrono::milliseconds(500));
    }
    this_thread::sleep_for(chrono::milliseconds(400));
}

int deposit() {
    int amount;
    while (true) {
        cout << "How much would you like to deposit? ?";
        cin >> amount;
        if (cin.fail() || amount <= 0 || amount > MAX_BET) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Please enter a valid number greater than 0 and less than "<< MAX_BET << endl;
        } else {
            break;
        }
    }
    return amount;
}

int number_of_lines() {
    int lines;
    while (true) {
        cout << "How many lines would you like to bet on (1-" << MAX_LINES << ")? ";
        cin >> lines;
        if (cin.fail() || lines < 1 || lines > MAX_LINES) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Please enter a valid number of lines." << endl;
        } else {
            break;
        }
    }
    return lines;
}

int bet_on_lines(int balance) {
    if (balance <= 0) {
        cout << "Insufficient balance." << endl;
        cout << "Thank you for playing. PLAY AGAIN!!!" << endl;
        exit(0);
    }
    int amount;
    while (true) {
        cout << "How much would you like to bet on each line? ?";
        cin >> amount;
        if (cin.fail() || amount < MIN_BET || amount > MAX_BET) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Betting amount must be within ?" << MIN_BET << " - ?" << MAX_BET << endl;
        } else {
            break;
        }
    }
    return amount;
}

void round(int &balance) {
    int lines = number_of_lines();

    int bet;
    while (true) {
        bet = bet_on_lines(balance);
        int total_bet = bet * lines;
        if (total_bet > balance) {
            cout << "You do not have enough balance to bet that amount.\nBalance = ?" << balance << endl;
        } else {
            break;
        }
    }

    cout << "You are betting ?" << bet << " on " << lines << " lines." << endl;
    balance -= bet * lines;

    auto columns = get_machine_spin_output(ROWS, COLS, symbol_count);
    print_slot_machine_output(columns);

    auto [winnings, winning_lines] = check_win(columns, lines, bet, symbol_value);
    balance += winnings;

    if (!winning_lines.empty()) {
        cout << "You won on lines: ";
        for (int line : winning_lines) {
            cout << line << " ";
        }
        cout << endl;
        cout << "You won ?" << winnings << "!" << endl;
    } else {
        cout << "You didn't win this time." << endl;
    }
}

int main() {
    int balance = deposit();
    while (true) {
        round(balance);
        cout << "Your current balance is ?" << balance << endl;
        char play_again;
        cout << "Do you want to play again? (y/n): ";
        cin >> play_again;
        if (play_again != 'y') {
            break;
        }
    }
    cout << "Thank you for playing. Your final balance is ?" << balance << endl;
    return 0;
}
