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
#include <set>
#include <list>
#include <map>
#include <utility>
#include <vector>
#include <queue>
#include <string>
#include <algorithm>
#include <memory>
#include <functional>
#include <stdexcept>

#endif /* __PROGTEST */
using namespace std;

#ifndef __PROGTEST__

class CTimeStamp {
public:
    CTimeStamp() = default;

    CTimeStamp(int year, int month, int day, int hour, int minute, int sec);

    int Compare(const CTimeStamp &x) const;

    friend ostream &operator<<(ostream &os,
                               const CTimeStamp &x);

private:
    int m_Year, m_Month, m_Day, m_Hour, m_Minute, m_Sec;
};

CTimeStamp::CTimeStamp(int year, int month, int day, int hour, int minute, int sec)
/*: m_Year(year), m_Month(month), m_Day(day), m_Hour(hour), m_Minute(minute), m_Sec(sec) */{
    m_Year = year;
    m_Month = month;
    m_Day = day;
    m_Hour = hour;
    m_Minute = minute;
    m_Sec = sec;
}

int CTimeStamp::Compare(const CTimeStamp &x) const {
    if (m_Year < x.m_Year)
        return -1;
    if (m_Year == x.m_Year && m_Month < x.m_Month)
        return -2;
    if (m_Year == x.m_Year && m_Month == x.m_Month && m_Day < x.m_Day)
        return -3;
    if (m_Year == x.m_Year && m_Month == x.m_Month && m_Day == x.m_Day && m_Hour < x.m_Hour)
        return -5;
    if (m_Year == x.m_Year && m_Month == x.m_Month && m_Day == x.m_Day && m_Hour == x.m_Hour && m_Minute < x.m_Minute)
        return -6;
    if (m_Year == x.m_Year && m_Month == x.m_Month && m_Day == x.m_Day && m_Hour == x.m_Hour &&
        m_Minute == x.m_Minute && m_Sec < x.m_Sec)
        return -7;
    if (m_Year == x.m_Year && m_Month == x.m_Month && m_Day == x.m_Day && m_Hour == x.m_Hour &&
        m_Minute == x.m_Minute && m_Sec == x.m_Sec)
        return 0;
    return 69;
}

ostream &operator<<(ostream &os, const CTimeStamp &x) {
    os << x.m_Year << "-" << setfill('0') << setw(2) << x.m_Month << "-" << x.m_Day << " " << x.m_Hour << ":"
       << x.m_Minute << ":" << x.m_Sec;
    return os;
}

//=================================================================================================
class CMailBody {
public:
    CMailBody() = default;

    CMailBody(int size,
              const char *data);

    ~CMailBody() {
//        delete[] m_Data;
        m_Size = 0;
    }

    // copy cons/op=/destructor is correctly implemented in the testing environment

    CMailBody &operator=(const CMailBody *src);

    friend ostream &operator<<(ostream &os,
                               const CMailBody &x) {
        return os << "mail body: " << x.m_Size << " B";
    }

private:
//    char *m_Data; TODO: moved these to public for printing? how bad is that?
    string m_Data;
    int m_Size{};
};

CMailBody::CMailBody(int size, const char *data) /*: m_Size(size), m_Data(data) */{
//    m_Data = new char[strlen(data) + 1];
//    strcpy(m_Data, data);
    m_Size = size;
    m_Data = data;
}

CMailBody &CMailBody::operator=(const CMailBody *src) {
//    m_Data = new char[strlen(src->m_Data) + 1];
//    strcpy(m_Data, src->m_Data);
    m_Data = src->m_Data;
    return *this;
}

//=================================================================================================
//Pre-made
class CAttach {
public:
    CAttach(int x) : m_X(x), m_RefCnt(1) {}

    void AddRef() const {
        m_RefCnt++;
    }

    void Release() const {
        if (!--m_RefCnt)
            delete this;
    }

    int m_X; //TODO: moved this as well
private:
    mutable int m_RefCnt;

    CAttach(const CAttach &x);

    CAttach &operator=(const CAttach &x);

    ~CAttach() = default;

    friend ostream &operator<<(ostream &os,
                               const CAttach &x) {
        return os << "attachment: " << x.m_X << " B";
    }
};
//=================================================================================================
#endif /* __PROGTEST__, DO NOT remove */

//THIS
class CMail {
public:
    CMail() = default;

    CMail(const CTimeStamp &timeStamp, string from, const CMailBody &body, const CAttach *attach);

    CMail(const CMail &src);

    ~CMail();

    const string &From() const;

    const CMailBody &Body() const;

    const CTimeStamp &TimeStamp() const;

    const CAttach *Attachment() const;

    friend ostream &operator<<(ostream &os, const CMail &x);

    bool operator<(const CMail &other) const;

private:
    CTimeStamp m_TimeStamp;
    string m_From;
    CMailBody m_Body;
    const CAttach *m_Attachment;
};

CMail::CMail(const CTimeStamp &timeStamp, string from, const CMailBody &body, const CAttach *attach) : m_TimeStamp(
        timeStamp), m_From(std::move(from)), m_Body(body) {
    m_Attachment = attach;
    if (attach != nullptr)
        attach->AddRef();
}

CMail::CMail(const CMail &src) : m_TimeStamp(src.m_TimeStamp), m_From(src.m_From), m_Body(src.m_Body) {
    m_Attachment = src.m_Attachment;
    if (src.m_Attachment != nullptr)
        src.m_Attachment->AddRef();
}

CMail::~CMail() {
    if (m_Attachment != nullptr)
        m_Attachment->Release();
    //TODO rest?
}

const string &CMail::From() const {
    return m_From;
}

const CMailBody &CMail::Body() const {
    return m_Body;
}

const CTimeStamp &CMail::TimeStamp() const {
    return m_TimeStamp;
}

const CAttach *CMail::Attachment() const {
    return m_Attachment;
}

ostream &operator<<(ostream &os, const CMail &x) {
    cout << x.m_TimeStamp << " " << x.m_From << " " << x.m_Body;
    if (x.Attachment() != nullptr)
        cout << " + " << *x.Attachment();
    cout << endl;

    os << x.m_TimeStamp << " " << x.m_From << " " << x.m_Body;
    if (x.Attachment() != nullptr)
        os << " + " << *x.Attachment();

    return os;
}

bool CMail::operator<(const CMail &other) const {
    int tmp = this->TimeStamp().Compare(other.TimeStamp());
    return tmp < 0;
}

//=================================================================================================
class CMailBox {
public:
    CMailBox();

    bool Delivery(const CMail &mail);

    bool NewFolder(const string &folderName);

    bool MoveMail(const string &fromFolder,
                  const string &toFolder);

    list<CMail> ListMail(const string &folderName,
                         const CTimeStamp &from,
                         const CTimeStamp &to) const;

    set<string> ListAddr(const CTimeStamp &from,
                         const CTimeStamp &to) const;

private:
    map<string, multiset<CMail>> m_Box;
};

CMailBox::CMailBox() {
    m_Box["inbox"] = multiset<CMail>();
}

bool CMailBox::Delivery(const CMail &mail) {
    m_Box["inbox"].insert(mail);
    return true;
}

bool CMailBox::NewFolder(const string &folderName) {
    if (m_Box.count(folderName) == 0) { //New folder name
        m_Box[folderName];
        return true;
    }
    return false;
}

bool CMailBox::MoveMail(const string &fromFolder, const string &toFolder) {
    if (m_Box.count(fromFolder) == 0 || m_Box.count(toFolder) == 0)//If either folder doesn't exist return false
        return false;
    if (fromFolder == toFolder)
        return true;
    m_Box[toFolder].merge(m_Box[fromFolder]);
    return true;
}

list<CMail> CMailBox::ListMail(const string &folderName, const CTimeStamp &from, const CTimeStamp &to) const {
    list<CMail> tmp;
    auto place = m_Box.find(folderName);
    if (place == m_Box.end()) //Folder doesn't exist
        return tmp;

    CMailBody tempBody(0, "");
    CMail FFrom(from, "", tempBody, nullptr);
    CMail TTo(to, "", tempBody, nullptr);
    auto itFrom = place->second.lower_bound(FFrom);
    auto itTo = place->second.upper_bound(TTo);

    while (itFrom != itTo) {
        tmp.push_back(*itFrom);
        itFrom++;
    }
    return tmp;
}

set<string> CMailBox::ListAddr(const CTimeStamp &from, const CTimeStamp &to) const {
    set<string> tmp;
    CMailBody tempBody(0, "");
    CMail FFrom(from, "", tempBody, nullptr);
    CMail TTo(to, "", tempBody, nullptr);

    for (const auto &it : m_Box) {
        auto itStart = it.second.lower_bound(FFrom);
        auto itEnd = it.second.upper_bound(TTo);
        for (; itStart != itEnd; ++itStart)
            tmp.insert(itStart->From());
    }
    return tmp;
}
//=================================================================================================
#ifndef __PROGTEST__

static string showMail(const list<CMail> &l) {
    ostringstream oss;
    for (const auto &x : l)
        oss << x << endl;
    return oss.str();
}

static string showUsers(const set<string> &s) {
    ostringstream oss;
    for (const auto &x : s)
        oss << x << endl;
    return oss.str();
}

int main() {
    list<CMail> mailList;
    set<string> users;
    CAttach *att;

    CMailBox m0;
    assert (m0.Delivery(
            CMail(CTimeStamp(2014, 3, 31, 15, 24, 13), "user1@fit.cvut.cz", CMailBody(14, "mail content 1"), nullptr)));
    assert (m0.Delivery(
            CMail(CTimeStamp(2014, 3, 31, 15, 26, 23), "user2@fit.cvut.cz", CMailBody(22, "some different content"),
                  nullptr)));
    att = new CAttach(200);
    assert (m0.Delivery(
            CMail(CTimeStamp(2014, 3, 31, 11, 23, 43), "boss1@fit.cvut.cz", CMailBody(14, "urgent message"), att)));
    assert (m0.Delivery(
            CMail(CTimeStamp(2014, 3, 31, 18, 52, 27), "user1@fit.cvut.cz", CMailBody(14, "mail content 2"), att)));
    att->Release();
    att = new CAttach(97);
    assert (m0.Delivery(
            CMail(CTimeStamp(2014, 3, 31, 16, 12, 48), "boss1@fit.cvut.cz", CMailBody(24, "even more urgent message"),
                  att)));
    att->Release();
    assert (showMail(m0.ListMail("inbox",
                                 CTimeStamp(2000, 1, 1, 0, 0, 0),
                                 CTimeStamp(2050, 12, 31, 23, 59, 59))) ==
            "2014-03-31 11:23:43 boss1@fit.cvut.cz mail body: 14 B + attachment: 200 B\n"
            "2014-03-31 15:24:13 user1@fit.cvut.cz mail body: 14 B\n"
            "2014-03-31 15:26:23 user2@fit.cvut.cz mail body: 22 B\n"
            "2014-03-31 16:12:48 boss1@fit.cvut.cz mail body: 24 B + attachment: 97 B\n"
            "2014-03-31 18:52:27 user1@fit.cvut.cz mail body: 14 B + attachment: 200 B\n");
    assert (showMail(m0.ListMail("inbox",
                                 CTimeStamp(2014, 3, 31, 15, 26, 23),
                                 CTimeStamp(2014, 3, 31, 16, 12, 48))) ==
            "2014-03-31 15:26:23 user2@fit.cvut.cz mail body: 22 B\n"
            "2014-03-31 16:12:48 boss1@fit.cvut.cz mail body: 24 B + attachment: 97 B\n");
    assert (showUsers(m0.ListAddr(CTimeStamp(2000, 1, 1, 0, 0, 0),
                                  CTimeStamp(2050, 12, 31, 23, 59, 59))) == "boss1@fit.cvut.cz\n"
                                                                            "user1@fit.cvut.cz\n"
                                                                            "user2@fit.cvut.cz\n");
    assert (showUsers(m0.ListAddr(CTimeStamp(2014, 3, 31, 15, 26, 23),
                                  CTimeStamp(2014, 3, 31, 16, 12, 48))) == "boss1@fit.cvut.cz\n"
                                                                           "user2@fit.cvut.cz\n");

    CMailBox m1;
    assert (m1.NewFolder("work"));
    assert (m1.NewFolder("spam"));
    assert (!m1.NewFolder("spam"));
    assert (m1.Delivery(
            CMail(CTimeStamp(2014, 3, 31, 15, 24, 13), "user1@fit.cvut.cz", CMailBody(14, "mail content 1"), nullptr)));
    att = new CAttach(500);
    assert (m1.Delivery(
            CMail(CTimeStamp(2014, 3, 31, 15, 26, 23), "user2@fit.cvut.cz", CMailBody(22, "some different content"),
                  att)));
    att->Release();
    assert (m1.Delivery(
            CMail(CTimeStamp(2014, 3, 31, 11, 23, 43), "boss1@fit.cvut.cz", CMailBody(14, "urgent message"), nullptr)));
    att = new CAttach(468);
    assert (m1.Delivery(
            CMail(CTimeStamp(2014, 3, 31, 18, 52, 27), "user1@fit.cvut.cz", CMailBody(14, "mail content 2"), att)));
    att->Release();
    assert (m1.Delivery(
            CMail(CTimeStamp(2014, 3, 31, 16, 12, 48), "boss1@fit.cvut.cz", CMailBody(24, "even more urgent message"),
                  nullptr)));
    assert (showMail(m1.ListMail("inbox",
                                 CTimeStamp(2000, 1, 1, 0, 0, 0),
                                 CTimeStamp(2050, 12, 31, 23, 59, 59))) ==
            "2014-03-31 11:23:43 boss1@fit.cvut.cz mail body: 14 B\n"
            "2014-03-31 15:24:13 user1@fit.cvut.cz mail body: 14 B\n"
            "2014-03-31 15:26:23 user2@fit.cvut.cz mail body: 22 B + attachment: 500 B\n"
            "2014-03-31 16:12:48 boss1@fit.cvut.cz mail body: 24 B\n"
            "2014-03-31 18:52:27 user1@fit.cvut.cz mail body: 14 B + attachment: 468 B\n");
    assert (showMail(m1.ListMail("work",
                                 CTimeStamp(2000, 1, 1, 0, 0, 0),
                                 CTimeStamp(2050, 12, 31, 23, 59, 59))) == "");
    assert (m1.MoveMail("inbox", "work"));
    assert (showMail(m1.ListMail("inbox",
                                 CTimeStamp(2000, 1, 1, 0, 0, 0),
                                 CTimeStamp(2050, 12, 31, 23, 59, 59))) == "");
    assert (showMail(m1.ListMail("work",
                                 CTimeStamp(2000, 1, 1, 0, 0, 0),
                                 CTimeStamp(2050, 12, 31, 23, 59, 59))) ==
            "2014-03-31 11:23:43 boss1@fit.cvut.cz mail body: 14 B\n"
            "2014-03-31 15:24:13 user1@fit.cvut.cz mail body: 14 B\n"
            "2014-03-31 15:26:23 user2@fit.cvut.cz mail body: 22 B + attachment: 500 B\n"
            "2014-03-31 16:12:48 boss1@fit.cvut.cz mail body: 24 B\n"
            "2014-03-31 18:52:27 user1@fit.cvut.cz mail body: 14 B + attachment: 468 B\n");
    assert (m1.Delivery(
            CMail(CTimeStamp(2014, 3, 31, 19, 24, 13), "user2@fit.cvut.cz", CMailBody(14, "mail content 4"), nullptr)));
    att = new CAttach(234);
    assert (m1.Delivery(
            CMail(CTimeStamp(2014, 3, 31, 13, 26, 23), "user3@fit.cvut.cz", CMailBody(9, "complains"), att)));
    att->Release();
    assert (showMail(m1.ListMail("inbox",
                                 CTimeStamp(2000, 1, 1, 0, 0, 0),
                                 CTimeStamp(2050, 12, 31, 23, 59, 59))) ==
            "2014-03-31 13:26:23 user3@fit.cvut.cz mail body: 9 B + attachment: 234 B\n"
            "2014-03-31 19:24:13 user2@fit.cvut.cz mail body: 14 B\n");
    assert (showMail(m1.ListMail("work",
                                 CTimeStamp(2000, 1, 1, 0, 0, 0),
                                 CTimeStamp(2050, 12, 31, 23, 59, 59))) ==
            "2014-03-31 11:23:43 boss1@fit.cvut.cz mail body: 14 B\n"
            "2014-03-31 15:24:13 user1@fit.cvut.cz mail body: 14 B\n"
            "2014-03-31 15:26:23 user2@fit.cvut.cz mail body: 22 B + attachment: 500 B\n"
            "2014-03-31 16:12:48 boss1@fit.cvut.cz mail body: 24 B\n"
            "2014-03-31 18:52:27 user1@fit.cvut.cz mail body: 14 B + attachment: 468 B\n");
    assert (m1.MoveMail("inbox", "work"));
    assert (showMail(m1.ListMail("inbox",
                                 CTimeStamp(2000, 1, 1, 0, 0, 0),
                                 CTimeStamp(2050, 12, 31, 23, 59, 59))) == "");
    assert (showMail(m1.ListMail("work",
                                 CTimeStamp(2000, 1, 1, 0, 0, 0),
                                 CTimeStamp(2050, 12, 31, 23, 59, 59))) ==
            "2014-03-31 11:23:43 boss1@fit.cvut.cz mail body: 14 B\n"
            "2014-03-31 13:26:23 user3@fit.cvut.cz mail body: 9 B + attachment: 234 B\n"
            "2014-03-31 15:24:13 user1@fit.cvut.cz mail body: 14 B\n"
            "2014-03-31 15:26:23 user2@fit.cvut.cz mail body: 22 B + attachment: 500 B\n"
            "2014-03-31 16:12:48 boss1@fit.cvut.cz mail body: 24 B\n"
            "2014-03-31 18:52:27 user1@fit.cvut.cz mail body: 14 B + attachment: 468 B\n"
            "2014-03-31 19:24:13 user2@fit.cvut.cz mail body: 14 B\n");

    return 0;
}

#endif /* __PROGTEST__ */
