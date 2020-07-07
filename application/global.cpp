#include "global.h"

#include <QRandomGenerator>

ExecuteResult runCmd(QString cmd, bool interactive, bool onlyStdout)
{
    SCOPE_TRACKER;
    QEventLoop loop;
    QProcess proc;
    if(!onlyStdout)
        proc.setReadChannelMode(QProcess::MergedChannels);
    QObject::connect(&proc, QOverload<int>::of(&QProcess::finished), &loop, &QEventLoop::quit);
    DEBUG << "Execute Statment: command: " << "/bin/bash";
    DEBUG << "Execute Statment: args: " << (QStringList() << ((interactive) ? "-ic" : "-c") << cmd);
    proc.start("/bin/bash", (QStringList() << ((interactive) ? QString("-ic") : QString("-c")) << cmd));
    loop.exec();
    QObject::disconnect(&proc, nullptr, nullptr, nullptr);
    ExecuteResult result;
    result.rv = proc.exitCode();
    result.output = proc.readAll(); //remove .trimmed() because all spaces count in output
    proc.close();
    return result;
};

QString randomString(int length, QString possible)
{
    QString result;
    while(length > 0)
    {
        result.append(possible.at(static_cast<int>(QRandomGenerator::global()->generate() % static_cast<unsigned int>(possible.length()))));
        length--;
    }
    return result;
}

QString bashInteractiveVariable(QString name)
{
    QProcess proc;
    QString uniqueString = randomString(64);
    proc.setProgram("bash");
    proc.setArguments(QStringList() << "-ic" << QString("echo -n %1; echo -n \"$%2\"").arg(uniqueString).arg(name));
    proc.start(QProcess::ReadOnly);
    proc.waitForStarted();
    proc.waitForFinished();
    QString output = proc.readAll();
    int pos = output.indexOf(uniqueString);
    output = output.mid(pos + uniqueString.length());
    return output;
}


