#ifndef AUTOINSTALLS_H
#define AUTOINSTALLS_H

#include "qprocess.h"
#include "windows.h"
#include <QObject>

class Exesetup : public QObject
{
    Q_OBJECT
public:
    explicit Exesetup(QObject *parent = nullptr);
    virtual ~Exesetup();
    const QString& getinfo(){return *info;};
    const QStringList analysis_args(const QString& args);
public slots:
    void exeinstall(const QStringList& applist, bool cmd_or_man);
signals:
    void execomplte(const QString& msg);
    void execomplte(int countapp,bool cmd_or_man);
private:
    QProcess*  Installprocess;
    QString* info;
    int countapp;
};


class AutoInstalls : public QObject
{
    Q_OBJECT
public:
    explicit AutoInstalls(QObject *parent = nullptr);
    virtual ~AutoInstalls();
public slots:
    void appautoinstall(const QString& win_name, const QStringList& setuplist);
    void InstallActions(const QString& namestr, const QStringList& setup);
    void browersetups(HWND Appname,const QStringList& setupstrlist);
signals:
private:
    QStringList* savelist;
};

#endif // AUTOINSTALLS_H
