#ifndef __PROGTEST__

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cassert>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <string>
#include <utility>
#include <vector>
#include <memory>
#include <functional>
#include <stdexcept>
#include <algorithm>

using namespace std;
#endif /* __PROGTEST__ */
/**Used to define the action certain methods(findAccount, findHuman will execute.*/
enum class actionType {
    BIRTH,
    DEATH,
    INCOME,
    EXPENSE,
    AUDIT
};

class CHuman {
private:
    string name = {};
    string address = {};
    string account = {};
public:
    int totalIncome = 0;
    int totalExpenses = 0;

    /**
     * Creates new instance of CHuman with given name, address and account.
     * @param name CHuman name
     * @param address CHuman address
     * @param account CHuman account
     */
    CHuman(string name, string address, string account);

    ~CHuman() = default;

    string getName() const { return name; }

    string getAddress() const { return address; }

    string getAccount() const { return account; }

    /** @return true if CHuman "a" is before CHuman "b" based on name and then address, otherwise returns false.*/
    friend bool operator<(const CHuman &a, const CHuman &b);
};

CHuman::CHuman(string name, string address, string account) :
        name(move(name)), address(move(address)), account(move(account)) {}

bool operator<(const CHuman &a, const CHuman &b) {
    int cmp = a.getName().compare(b.getName());
    if (cmp == 0) {
        return a.getAddress() < b.getAddress();
    }
    return cmp < 0;
}

class CIterator {
private:
    uint16_t it = 0;
    vector<CHuman> humans;
public:
    /**
     * Constructor
     * @param srcHumans  vector<CHuman> to create iterator for
     */
    explicit CIterator(vector<CHuman> &srcHumans) : humans(srcHumans) {}

    ~CIterator() = default;

    /** @return true if iterator reached end of vector*/
    bool AtEnd() const { return humans.size() == it; }

    void Next() { it++; }

    string Name() const { return humans.at(it).getName(); }

    string Addr() const { return humans.at(it).getAddress(); }

    string Account() const { return humans.at(it).getAccount(); }
};


class CTaxRegister {
private:
    vector<CHuman> humans;
    /**Iterator used in find human method*/
    vector<CHuman>::iterator it;

    /**
     * Finds out if CHuman on position has same name and address as the ones given in the input
     * @param position of the human in vector<CHuman>
     * @param name to check for equality
     * @param addr to check for equality
     * @return true if the person on given position has the same name and address otherwise returns false
     */
    bool identicalCheck(vector<CHuman>::iterator position, const string &name, const string &addr) const;

    /**
     * Iterates through our human vector and uses identicalCheck to find desired human and depending on the type parameter
     * certain actions will be made.
     * @param account account name
     * @param amount amount to be added to totalIncome/totalExpenses, only used in type (INCOME/EXPENSE)
     * @param type specifies type of action to be made from actionType
     * @return true if account was found otherwise returns false
     */
    bool findAccount(const string &account, int amount, actionType type);

    /**
     * Creates a temporary CHuman to store input info in and then uses lower_bound function to find the position of said
     * CHuman in our vector<CHuman>. It also does certain actions depending on the type parameter, that varies depending
     * on in which method it was called.
     * @param name CHuman name
     * @param address CHuman address
     * @param account CHuman account
     * @param type specifies which action should be made, depends on in which method its called
     * @param amount amount to be added to totalIncome/totalExpenses, only used in type (INCOME/EXPENSE)
     * @param iterator only used when type is AUDIT. It's set to the position of the tmp CHuman
     * @return true on success, otherwise returns false. (Details depend on the type of action)
     */
    bool findHuman(const string &name, const string &address, const string &account, actionType type, int amount,
                   vector<CHuman>::iterator &iterator);

public:

    CTaxRegister() = default;

    ~CTaxRegister() = default;

    /**Birth & Death are inspired by J. Matousek's stream (https://www.twitch.tv/honzamatousek, https://www.youtube.com/channel/UCaOKhMQTuBscw4zKQwdA4Qg/videos)*/
    /**
     * Creates and inserts new CHuman in humans vector
     * @param name CHuman name
     * @param address CHuman address
     * @param account CHuman account
     * @return true on success, otherwise returns false
     */
    bool Birth(const string &name, const string &addr, const string &account);

    /**
     * Removes CHuman from humans vector, given its name and address
     * @param name CHuman name
     * @param address CHuman address
     * @return true on success, otherwise returns false
     */
    bool Death(const string &name, const string &addr);

    /**
     * Increase totalIncome on given account
     * @param account account name
     * @param amount amount to add
     * @return true on success, otherwise returns false
     */
    bool Income(const string &account, int amount);

    /**
     * Increase totalIncome of given CHuman
     * @param name name of CHuman
     * @param addr address of CHuman
     * @param amount to add
     * @return true on success, otherwise returns false
     */
    bool Income(const string &name, const string &addr, int amount);

    /**
     * Increase totalExpense on given account
     * @param account account name
     * @param amount amount to add
     * @return true on success, otherwise returns false
     */
    bool Expense(const string &account, int amount);

    /**
     * Increase totalExpense of given CHuman
     * @param name name of CHuman
     * @param addr address of CHuman
     * @param amount to add
     * @return true on success, otherwise returns false
     */
    bool Expense(const string &name, const string &addr, int amount);

    /**
     * Finds CHuman with given name & address. Saves all his account details to input parameters(account, income sum and expenses sum)
     * @param name CHuman name
     * @param addr CHuman address
     * @param account name
     * @param sumIncome sum of all incomes
     * @param sumExpense sum of all expenses
     * @return true on success, otherwise returns false
     */
    bool Audit(const string &name, const string &addr, string &account, int &sumIncome, int &sumExpense);

    /**
     * Used to iterate through our CHuman ector
     * @return CIterator instance
     */
    CIterator ListByName() const;
};

bool CTaxRegister::Birth(const string &name, const string &addr, const string &account) {
    //Checking for duplicate account name
    if (findAccount(account, 0, actionType::BIRTH)) {
        return false;
    }
    return findHuman(name, addr, account, actionType::BIRTH, 0, it);
}

bool CTaxRegister::Death(const string &name, const string &addr) {
    return findHuman(name, addr, "", actionType::DEATH, 0, it);
}

bool CTaxRegister::Income(const string &account, int amount) {
    return findAccount(account, amount, actionType::INCOME);
}

bool CTaxRegister::Income(const string &name, const string &addr, int amount) {
    return findHuman(name, addr, "", actionType::INCOME, amount, it);
}

bool CTaxRegister::Expense(const string &account, int amount) {
    return findAccount(account, amount, actionType::EXPENSE);
}

bool CTaxRegister::Expense(const string &name, const string &addr, int amount) {
    return findHuman(name, addr, "", actionType::EXPENSE, amount, it);
}

bool CTaxRegister::Audit(const string &name, const string &addr, string &account, int &sumIncome, int &sumExpense) {
    //Creating tmpHuman to store and check info
    if (findHuman(name, addr, "", actionType::AUDIT, 0, it)) {
        account = it->getAccount();
        sumIncome = it->totalIncome;
        sumExpense = it->totalExpenses;
        return true;
    }
    return false;
}

CIterator CTaxRegister::ListByName() const {
    auto iter = CIterator((vector<struct CHuman> &) humans);
    return iter;
}

bool CTaxRegister::identicalCheck(vector<CHuman>::iterator position, const string &name, const string &addr) const {
    if (position != humans.end()) {
        if (position->getName() == name && position->getAddress() == addr) {
            return true;
        }
    }
    return false;
}

bool CTaxRegister::findAccount(const string &account, int amount, actionType type) {
    for (auto &i : humans) {
        if (i.getAccount() == account) {
            switch (type) {
                case actionType::BIRTH:
                    break;
                case actionType::INCOME:
                    i.totalIncome += amount;
                    break;
                case actionType::EXPENSE:
                    i.totalExpenses += amount;
                    break;
                default:
                    break;
            }
            return true;
        }
    }
    return false;
}

bool
CTaxRegister::findHuman(const string &name, const string &address, const string &account, actionType type, int amount,
                        vector<CHuman>::iterator &iterator) {
    auto *tmpHuman = new CHuman(name, address, account);
    auto position = lower_bound(humans.begin(), humans.end(), *tmpHuman);
    bool identical = identicalCheck(position, name, address);

    switch (type) {
        case actionType::BIRTH:
            if (!identical)
                humans.insert(position, *tmpHuman);
            delete tmpHuman;
            return !identical;

        case actionType::DEATH:
            if (identical)
                humans.erase(position);
            delete tmpHuman;
            return identical;

        case actionType::INCOME:
            if (identical)
                position->totalIncome += amount;
            delete tmpHuman;
            return identical;

        case actionType::EXPENSE:
            if (identical)
                position->totalExpenses += amount;
            delete tmpHuman;
            return identical;

        case actionType::AUDIT:
            if (identical)
                iterator = position;
            delete tmpHuman;
            return identical;

        default:
            return false;
    }
}

#ifndef __PROGTEST__

int main() {
    string acct;
    int sumIncome, sumExpense;
    CTaxRegister b1;
    assert (b1.Birth("John Smith", "Oak Road 23", "123/456/789"));
    assert (b1.Birth("Jane Hacker", "Main Street 17", "Xuj5#94"));
    assert (b1.Birth("Peter Hacker", "Main Street 17", "634oddT"));
    assert (b1.Birth("John Smith", "Main Street 17", "Z343rwZ"));
    assert (b1.Income("Xuj5#94", 1000));
    assert (b1.Income("634oddT", 2000));
    assert (b1.Income("123/456/789", 3000));
    assert (b1.Income("634oddT", 4000));
    assert (b1.Income("Peter Hacker", "Main Street 17", 2000));
    assert (b1.Expense("Jane Hacker", "Main Street 17", 2000));
    assert (b1.Expense("John Smith", "Main Street 17", 500));
    assert (b1.Expense("Jane Hacker", "Main Street 17", 1000));
    assert (b1.Expense("Xuj5#94", 1300));
    assert (b1.Audit("John Smith", "Oak Road 23", acct, sumIncome, sumExpense));
    assert (acct == "123/456/789");
    assert (sumIncome == 3000);
    assert (sumExpense == 0);
    assert (b1.Audit("Jane Hacker", "Main Street 17", acct, sumIncome, sumExpense));
    assert (acct == "Xuj5#94");
    assert (sumIncome == 1000);
    assert (sumExpense == 4300);
    assert (b1.Audit("Peter Hacker", "Main Street 17", acct, sumIncome, sumExpense));
    assert (acct == "634oddT");
    assert (sumIncome == 8000);
    assert (sumExpense == 0);
    assert (b1.Audit("John Smith", "Main Street 17", acct, sumIncome, sumExpense));
    assert (acct == "Z343rwZ");
    assert (sumIncome == 0);
    assert (sumExpense == 500);
    CIterator it = b1.ListByName();
    assert (!it.AtEnd()
            && it.Name() == "Jane Hacker"
            && it.Addr() == "Main Street 17"
            && it.Account() == "Xuj5#94");
    it.Next();
    assert (!it.AtEnd()
            && it.Name() == "John Smith"
            && it.Addr() == "Main Street 17"
            && it.Account() == "Z343rwZ");
    it.Next();
    assert (!it.AtEnd()
            && it.Name() == "John Smith"
            && it.Addr() == "Oak Road 23"
            && it.Account() == "123/456/789");
    it.Next();
    assert (!it.AtEnd()
            && it.Name() == "Peter Hacker"
            && it.Addr() == "Main Street 17"
            && it.Account() == "634oddT");
    it.Next();
    assert (it.AtEnd());
    assert (b1.Death("John Smith", "Main Street 17"));
    CTaxRegister b2;
    assert (b2.Birth("John Smith", "Oak Road 23", "123/456/789"));
    assert (b2.Birth("Jane Hacker", "Main Street 17", "Xuj5#94"));
    assert (!b2.Income("634oddT", 4000));
    assert (!b2.Expense("John Smith", "Main Street 18", 500));
    assert (!b2.Audit("John Nowak", "Second Street 23", acct, sumIncome, sumExpense));
    assert (!b2.Death("Peter Nowak", "5-th Avenue"));
    assert (!b2.Birth("Jane Hacker", "Main Street 17", "4et689A"));
    assert (!b2.Birth("Joe Hacker", "Elm Street 23", "Xuj5#94"));
    assert (b2.Death("Jane Hacker", "Main Street 17"));
    assert (b2.Birth("Joe Hacker", "Elm Street 23", "Xuj5#94"));
    assert (b2.Audit("Joe Hacker", "Elm Street 23", acct, sumIncome, sumExpense));
    assert (acct == "Xuj5#94");
    assert (sumIncome == 0);
    assert (sumExpense == 0);
    assert (!b2.Birth("Joe Hacker", "Elm Street 23", "AAj5#94"));
    return 0;
}

#endif /* __PROGTEST__ */
