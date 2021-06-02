#ifndef __PROGTEST__

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cctype>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;
#endif /* __PROGTEST__ */


///Transaction class
class CTransaction {
public:
    char *accID{};
    char *otherAcc{};
    char *signature{};
    int balanceBefore{};
    unsigned int amount{};
    bool incoming{};

    CTransaction() = default;

    CTransaction(const char *firstID, const char *secondID, const char *sig, unsigned int amountPayed, int balBefore,
                 bool inc);

    explicit CTransaction(const CTransaction *src);

    ~CTransaction();


    CTransaction &operator=(const CTransaction *src);
};

CTransaction::CTransaction(const char *firstID, const char *secondID, const char *sig, unsigned int amountPayed,
                           int balBefore, bool inc) {
    accID = new char[strlen(firstID) + 1];
    otherAcc = new char[strlen(secondID) + 1];
    signature = new char[strlen(sig) + 1];

    strcpy(accID, firstID);
    strcpy(otherAcc, secondID);
    strcpy(signature, sig);
    balanceBefore = balBefore;
    amount = amountPayed;
    incoming = inc;
}

CTransaction::~CTransaction() {
    delete[] accID;
    delete[] otherAcc;
    delete[] signature;
}


CTransaction::CTransaction(const CTransaction *src) {
    accID = new char[strlen(src->accID) + 1];
    otherAcc = new char[strlen(src->otherAcc) + 1];
    signature = new char[strlen(src->signature) + 1];

    strcpy(accID, src->accID);
    strcpy(otherAcc, src->otherAcc);
    strcpy(signature, src->signature);
    balanceBefore = src->balanceBefore;
    amount = src->amount;
    incoming = src->incoming;
}

CTransaction &CTransaction::operator=(const CTransaction *src) {
//    if (src == this) return *this; //TODO?

    ///Deleting old stuff
    delete[] accID;
    delete[] otherAcc;
    delete[] signature;

    ///Assigning new stuff
    accID = new char[strlen(src->accID) + 1];
    otherAcc = new char[strlen(src->otherAcc) + 1];
    signature = new char[strlen(src->signature) + 1];

    strcpy(accID, src->accID);
    strcpy(otherAcc, src->otherAcc);
    strcpy(signature, src->signature);
    balanceBefore = src->balanceBefore;
    amount = src->amount;
    incoming = src->incoming;
    return *this;
}

///Account class
class CAccount {
public:
    char *ID;
    int currBalance;
    int maxTrans = 5;
    int transCnt;
    CTransaction **transactions;

    CAccount();

    CAccount(const char *accID, int balance);

    explicit CAccount(const CAccount *src);

    ~CAccount();

    ///Adds transaction and reallocates the array if needed
    void AddTransaction(const char *firstID, const char *secondID, const char *sig,
                        unsigned int amountPayed, int balBefore, bool inc);

    int Balance();

    friend ostream &operator<<(ostream &o, CAccount &acc);

private:
    ///Reallocates the transaction array
    void ResizeTransArr();
};

CAccount::CAccount() {
    ID = nullptr;
    currBalance = 0;
    transCnt = 0;
    transactions = new CTransaction *[transCnt];
}

void CAccount::AddTransaction(const char *firstID, const char *secondID, const char *sig, unsigned int amountPayed,
                              int balBefore, bool inc) {
    auto *transaction = new CTransaction(firstID, secondID, sig, amountPayed, balBefore, inc);
    if (transCnt >= maxTrans)
        ResizeTransArr();
    transactions[transCnt] = transaction;
    transCnt++;
}

void CAccount::ResizeTransArr() {
    maxTrans += maxTrans < 20 ? 5 : maxTrans / 2;
    auto **newArr = new CTransaction *[maxTrans];
    for (int i = 0; i < transCnt; ++i)
        newArr[i] = transactions[i];
    delete[] transactions;
    transactions = newArr;
}

CAccount::~CAccount() {
    delete[] ID;
    for (int i = 0; i < transCnt; ++i)
        delete transactions[i];
    delete[] transactions;
}

CAccount::CAccount(const CAccount *src) {
    currBalance = src->currBalance;
    maxTrans = src->maxTrans;
    transCnt = src->transCnt;
    transactions = new CTransaction *[src->maxTrans];
    for (int i = 0; i < transCnt; ++i)
        transactions[i] = new CTransaction(src->transactions[i]);
    ID = new char[strlen(src->ID) + 1];
    strcpy(ID, src->ID);
}

int CAccount::Balance() {
    return currBalance;
}

ostream &operator<<(ostream &o, CAccount &acc) {
    if (acc.transCnt == 0) {
        o << acc.ID << ":" << endl << "   " << acc.currBalance << endl << " = " << acc.currBalance << endl;
        return o;
    }

    char InOut;
    o << acc.ID << ":" << endl << "   " << acc.transactions[0]->balanceBefore << endl;
    for (int i = 0; i < acc.transCnt; ++i) {
        if (acc.transactions[i]->incoming) {
            InOut = 43;
            o << " " << InOut << " " << acc.transactions[i]->amount << ", from: " << acc.transactions[i]->otherAcc
              << ", sign: " << acc.transactions[i]->signature << endl;
        } else {
            InOut = 45;
            o << " " << InOut << " " << acc.transactions[i]->amount << ", to: " << acc.transactions[i]->otherAcc
              << ", sign: " << acc.transactions[i]->signature << endl;
        }
    }
    o << " = " << acc.currBalance << endl;
    return o;
}

CAccount::CAccount(const char *accID, int balance) {
    ID = new char[strlen(accID) + 1];
    strcpy(ID, accID);
    currBalance = balance;
    maxTrans = 5;
    transCnt = 0;
    transactions = new CTransaction *[maxTrans];
}

///Bank class
class CBank {
public:
    CBank();

    CBank(const CBank &src);

    ~CBank();

    CBank &operator=(const CBank &src);

    bool NewAccount(const char *accID, int initialBalance);

    bool Transaction(const char *debAccID, const char *credAccID, unsigned int amount, const char *signature);

    bool TrimAccount(const char *accID);

    CAccount &Account(const char *accID);

private:
    CAccount **accounts;
    int accCnt;
    int maxAccs;
};

CBank::CBank() {
    accCnt = 0;
    maxAccs = 5;
    accounts = new CAccount *[maxAccs];
}

bool CBank::NewAccount(const char *accID, int initialBalance) {
    ///Check for duplicate account
    for (int i = 0; i < accCnt; ++i) {
        if (strcmp(accID, accounts[i]->ID) == 0)
            return false;
    }
    ///Allocating the memory for the array
    if (accCnt >= maxAccs) {
        maxAccs += (maxAccs < 25) ? 5 : maxAccs / 2;
        auto **tmp = new CAccount *[maxAccs];
        for (int i = 0; i < accCnt; ++i)
            tmp[i] = accounts[i];
        delete[] accounts;
        accounts = tmp;
    }
    ///Inserting the new account
    accounts[accCnt] = new CAccount(accID, initialBalance);
    accCnt++;
    return true;
}

bool CBank::Transaction(const char *debAccID, const char *credAccID, unsigned int amount, const char *signature) {
    int debPos = -1, crePos = -1;
    ///Finds the position of the accounts & returns false if at least 1 account wasn't found
    for (int i = 0; i < accCnt; ++i) {
        if (strcmp(accounts[i]->ID, debAccID) == 0)
            debPos = i;
        else if (strcmp(accounts[i]->ID, credAccID) == 0)
            crePos = i;
        if (debPos != -1 && crePos != -1)
            break;
    }
    if (debPos == -1 || crePos == -1)
        return false;
    ///Outgoing transaction
    accounts[debPos]->AddTransaction(debAccID, credAccID, signature, amount, accounts[debPos]->currBalance, false);
    accounts[debPos]->currBalance -= amount;
    ///Incoming transaction
    accounts[crePos]->AddTransaction(credAccID, debAccID, signature, amount, accounts[crePos]->currBalance, true);
    accounts[crePos]->currBalance += amount;
    return true;
}

bool CBank::TrimAccount(const char *accID) {
    for (int i = 0; i < accCnt; ++i) {
        if (strcmp(accID, accounts[i]->ID) == 0) {
            for (int j = 0; j < accounts[i]->transCnt; ++j)
                delete accounts[i]->transactions[j];
            accounts[i]->transCnt = 0;
            return true;
        }
    }
    return false;
}

CAccount &CBank::Account(const char *accID) {
    for (int i = 0; i < accCnt; ++i) {
        if (strcmp(accounts[i]->ID, accID) == 0)
            return *accounts[i];
    }
    throw invalid_argument("Account not found!");
}

CBank::CBank(const CBank &src) {
    accCnt = src.accCnt;
    maxAccs = src.maxAccs;
    accounts = new CAccount *[src.accCnt];
    for (int i = 0; i < accCnt; ++i)
        accounts[i] = new CAccount(src.accounts[i]);
}

CBank &CBank::operator=(const CBank &src) {
    if (&src == this) return *this;

    ///Delete old stuff
    for (int i = 0; i < accCnt; ++i)
        delete accounts[i];
    delete[] accounts;

    ///Assign new stuff
    accounts = new CAccount *[src.maxAccs];
    for (int i = 0; i < src.accCnt; ++i)
        accounts[i] = new CAccount(src.accounts[i]);
    accCnt = src.accCnt;
    maxAccs = src.maxAccs;
    return *this;
}

CBank::~CBank() {
    for (int i = 0; i < accCnt; ++i) {
        delete accounts[i];
    }
    delete[] accounts;
}

#ifndef __PROGTEST__

int main() {
    ostringstream os;
    char accCpy[100], debCpy[100], credCpy[100], signCpy[100];

    CBank x0;
    assert(x0.NewAccount("987654", -500));
    assert(!x0.NewAccount("987654", 342500)); //ADDED
    assert(x0.NewAccount("123456", 1000));
    assert(!x0.NewAccount("123456", 1000));     //ADDED
    assert(!x0.Transaction("2345", "987654", 300, "XAbG5uKz6E="));     ///ADDED
    assert(!x0.Transaction("2345", "2345", 300, "XAbG5uKz6E="));     ///ADDED
    assert(x0.Transaction("123456", "987654", 300, "XAbG5uKz6E="));
    assert(x0.Transaction("123456", "987654", 2890, "AbG5uKz6E="));
    assert(x0.NewAccount("111111", 5000));
    assert(x0.Transaction("111111", "987654", 290, "Okh6e+8rAiuT5="));

    assert(x0.Account("123456").Balance() == -2190);
    assert(x0.Account("987654").Balance() == 2980);
    assert(x0.Account("111111").Balance() == 4710);
    os.str("");
    os << x0.Account("123456");
    assert(!strcmp(os.str().c_str(),
                   "123456:\n   1000\n - 300, to: 987654, sign: XAbG5uKz6E=\n - 2890, to: 987654, sign: AbG5uKz6E=\n = -2190\n"));
    os.str("");
    os << x0.Account("987654");
    assert(!strcmp(os.str().c_str(),
                   "987654:\n   -500\n + 300, from: 123456, sign: XAbG5uKz6E=\n + 2890, from: 123456, sign: AbG5uKz6E=\n + 290, from: 111111, sign: Okh6e+8rAiuT5=\n = 2980\n"));
    os.str("");
    os << x0.Account("111111");
    assert(!strcmp(os.str().c_str(), "111111:\n   5000\n - 290, to: 987654, sign: Okh6e+8rAiuT5=\n = 4710\n"));
    assert(x0.TrimAccount("987654"));
    assert(x0.Transaction("111111", "987654", 123, "asdf78wrnASDT3W"));
    os.str("");
    os << x0.Account("987654");
    assert(!strcmp(os.str().c_str(), "987654:\n   2980\n + 123, from: 111111, sign: asdf78wrnASDT3W\n = 3103\n"));


    CBank x2;
    strncpy(accCpy, "123456", sizeof(accCpy));
    assert(x2.NewAccount(accCpy, 1000));
    strncpy(accCpy, "987654", sizeof(accCpy));
    assert(x2.NewAccount(accCpy, -500));
    strncpy(debCpy, "123456", sizeof(debCpy));
    strncpy(credCpy, "987654", sizeof(credCpy));
    strncpy(signCpy, "XAbG5uKz6E=", sizeof(signCpy));
    assert(x2.Transaction(debCpy, credCpy, 300, signCpy));
    strncpy(debCpy, "123456", sizeof(debCpy));
    strncpy(credCpy, "987654", sizeof(credCpy));
    strncpy(signCpy, "AbG5uKz6E=", sizeof(signCpy));
    assert(x2.Transaction(debCpy, credCpy, 2890, signCpy));
    strncpy(accCpy, "111111", sizeof(accCpy));
    assert(x2.NewAccount(accCpy, 5000));
    strncpy(debCpy, "111111", sizeof(debCpy));
    strncpy(credCpy, "987654", sizeof(credCpy));
    strncpy(signCpy, "Okh6e+8rAiuT5=", sizeof(signCpy));
    assert(x2.Transaction(debCpy, credCpy, 2890, signCpy));


    assert(x2.Account("123456").Balance() == -2190);
    assert(x2.Account("987654").Balance() == 5580);
    assert(x2.Account("111111").Balance() == 2110);
    os.str("");
    os << x2.Account("123456");
    assert(!strcmp(os.str().c_str(),
                   "123456:\n   1000\n - 300, to: 987654, sign: XAbG5uKz6E=\n - 2890, to: 987654, sign: AbG5uKz6E=\n = -2190\n"));
    os.str("");
    os << x2.Account("987654");
    assert(!strcmp(os.str().c_str(),
                   "987654:\n   -500\n + 300, from: 123456, sign: XAbG5uKz6E=\n + 2890, from: 123456, sign: AbG5uKz6E=\n + 2890, from: 111111, sign: Okh6e+8rAiuT5=\n = 5580\n"));
    os.str("");
    os << x2.Account("111111");
    assert(!strcmp(os.str().c_str(), "111111:\n   5000\n - 2890, to: 987654, sign: Okh6e+8rAiuT5=\n = 2110\n"));
    assert(x2.TrimAccount("987654"));
    strncpy(debCpy, "111111", sizeof(debCpy));
    strncpy(credCpy, "987654", sizeof(credCpy));
    strncpy(signCpy, "asdf78wrnASDT3W", sizeof(signCpy));
    assert(x2.Transaction(debCpy, credCpy, 123, signCpy));
    os.str("");
    os << x2.Account("987654");
    assert(!strcmp(os.str().c_str(), "987654:\n   5580\n + 123, from: 111111, sign: asdf78wrnASDT3W\n = 5703\n"));


    CBank x4;
    assert(x4.NewAccount("123456", 1000));
    assert(x4.NewAccount("987654", -500));
    assert(!x4.NewAccount("123456", 3000));
    assert(!x4.Transaction("123456", "666", 100, "123nr6dfqkwbv5"));
    assert(!x4.Transaction("666", "123456", 100, "34dGD74JsdfKGH"));
    assert(!x4.Transaction("123456", "123456", 100, "Juaw7Jasdkjb5"));


    try {
        x4.Account("666").Balance();
        assert("Missing exception !!" == NULL);
    }
    catch (...) {
    }
    try {
        os << x4.Account("666").Balance();
        assert("Missing exception !!" == NULL);
    }
    catch (...) {
    }
    assert(!x4.TrimAccount("666"));


    CBank x6;
    assert(x6.NewAccount("123456", 1000));
    assert(x6.NewAccount("987654", -500));
    assert(x6.Transaction("123456", "987654", 300, "XAbG5uKz6E="));
    assert(x6.Transaction("123456", "987654", 2890, "AbG5uKz6E="));
    assert(x6.NewAccount("111111", 5000));
    assert(x6.Transaction("111111", "987654", 2890, "Okh6e+8rAiuT5="));


    CBank x7(x6);
    assert(x6.Transaction("111111", "987654", 123, "asdf78wrnASDT3W"));
    assert(x7.Transaction("111111", "987654", 789, "SGDFTYE3sdfsd3W"));
    assert(x6.NewAccount("99999999", 7000));
    assert(x6.Transaction("111111", "99999999", 3789, "aher5asdVsAD"));
    assert(x6.TrimAccount("111111"));
    assert(x6.Transaction("123456", "111111", 221, "Q23wr234ER=="));
    os.str("");
    os << x6.Account("111111");
    assert(!strcmp(os.str().c_str(), "111111:\n   -1802\n + 221, from: 123456, sign: Q23wr234ER==\n = -1581\n"));
    os.str("");
    os << x6.Account("99999999");
    assert(!strcmp(os.str().c_str(), "99999999:\n   7000\n + 3789, from: 111111, sign: aher5asdVsAD\n = 10789\n"));
    os.str("");
    os << x6.Account("987654");
    assert(!strcmp(os.str().c_str(),
                   "987654:\n   -500\n + 300, from: 123456, sign: XAbG5uKz6E=\n + 2890, from: 123456, sign: AbG5uKz6E=\n + 2890, from: 111111, sign: Okh6e+8rAiuT5=\n + 123, from: 111111, sign: asdf78wrnASDT3W\n = 5703\n"));
    os.str("");
    os << x7.Account("111111");
    assert(!strcmp(os.str().c_str(),
                   "111111:\n   5000\n - 2890, to: 987654, sign: Okh6e+8rAiuT5=\n - 789, to: 987654, sign: SGDFTYE3sdfsd3W\n = 1321\n"));
    try {
        os << x7.Account("99999999").Balance();
        assert("Missing exception !!" == NULL);
    }
    catch (...) {
    }
    os.str("");
    os << x7.Account("987654");
    assert(!strcmp(os.str().c_str(),
                   "987654:\n   -500\n + 300, from: 123456, sign: XAbG5uKz6E=\n + 2890, from: 123456, sign: AbG5uKz6E=\n + 2890, from: 111111, sign: Okh6e+8rAiuT5=\n + 789, from: 111111, sign: SGDFTYE3sdfsd3W\n = 6369\n"));

    CBank x8;
    CBank x9;
    assert(x8.NewAccount("123456", 1000));
    assert(x8.NewAccount("987654", -500));
    assert(x8.Transaction("123456", "987654", 300, "XAbG5uKz6E="));
    assert(x8.Transaction("123456", "987654", 2890, "AbG5uKz6E="));
    assert(x8.NewAccount("111111", 5000));
    assert(x8.Transaction("111111", "987654", 2890, "Okh6e+8rAiuT5="));
    x9 = x8;
    assert(x8.Transaction("111111", "987654", 123, "asdf78wrnASDT3W"));
    assert(x9.Transaction("111111", "987654", 789, "SGDFTYE3sdfsd3W"));
    assert(x8.NewAccount("99999999", 7000));
    assert(x8.Transaction("111111", "99999999", 3789, "aher5asdVsAD"));
    assert(x8.TrimAccount("111111"));
    os.str("");
    os << x8.Account("111111");
    assert(!strcmp(os.str().c_str(), "111111:\n   -1802\n = -1802\n"));
    os.str("");
    os << x9.Account("111111");
    assert(!strcmp(os.str().c_str(),
                   "111111:\n   5000\n - 2890, to: 987654, sign: Okh6e+8rAiuT5=\n - 789, to: 987654, sign: SGDFTYE3sdfsd3W\n = 1321\n"));
    return 0;
}

#endif /* __PROGTEST__ */
