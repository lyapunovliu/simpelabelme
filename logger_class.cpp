//这个程序主要是为了进行日志撰写
//首先调用init——log 再链接消息 最后关闭
//Logger_Class log;
//log.init_log("log1.txt");
//qInstallMessageHandler((log.outputMessage));
//log.close_log("log.txt");

//打印日志到文件中
#include "logger_class.h"
QFile Logger_Class::logfile="log.txt";
QTextStream Logger_Class::text_stream(&logfile);
Logger_Class::Logger_Class()
{

}
/**
 * @brief Logger_Class::init_log 初始化日志文件
 * @param str 文件名
 */
void Logger_Class::init_log(QString str)
{
    logfile.setFileName(str);
//    QFile file("log.txt");
    logfile.open(QIODevice::WriteOnly | QIODevice::Append);

}
/**
 * @brief Logger_Class::close_log 关闭日志
 * @param str 日志文件名
 */
void Logger_Class::close_log(QString str)
{
    logfile.flush();
    logfile.close();

}
/**
 * @brief Logger_Class::outputMessage 打印消息
 * @param type 消息类型
 * @param context 上下文
 * @param msg 内容
 */
void Logger_Class::outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    static QMutex mutex;
    mutex.lock();

    QString text;
    switch(type)
    {
    case QtDebugMsg:
        text = QString("Debug:");
        break;

    case QtWarningMsg:
        text = QString("Warning:");
        break;

    case QtCriticalMsg:
        text = QString("Critical:");
        break;

    case QtFatalMsg:
        text = QString("Fatal:");
    }

    QString context_info = QString("File:(%1) Line:(%2)").arg(QString(context.file)).arg(context.line);
    QString current_date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd");
    QString current_date = QString("(%1)").arg(current_date_time);
    QString message = QString("%1 %2 %3 %4").arg(text).arg(context_info).arg(msg).arg(current_date);

//    QFile file("log.txt");
//    file.open(QIODevice::WriteOnly | QIODevice::Append);
//    QTextStream text_stream(&file);

    text_stream.setDevice(&logfile);

    text_stream<< message<< "\r\n";
//    file.flush();
//    file.close();

    mutex.unlock();
}
