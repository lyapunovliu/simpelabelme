#ifndef LOGGER_CLASS_H
#define LOGGER_CLASS_H
#include <QMutex>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QString>
class Logger_Class
{
public:
    Logger_Class();
    static QFile logfile;
    static QTextStream text_stream;
    void init_log(QString str);
    void close_log(QString str);
    static void outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg);
};

#endif // LOGGER_CLASS_H
