#ifndef __LOG_H
#define __LOG_H

#include <iostream>
#include <vector>
#include <sstream>
#include <syslog.h>

namespace bode {

/// \brief Log severity levels.
enum LogLevel { FATAL, ERROR, WARNING, INFO, DEBUG };

/// \brief <span class=stat_bad>red</span> Abstract base class for log destinations.
///
/// Subclass this class to add a new destination for logs, such as
/// feeding them to another program or something.  The "write" method should
/// dispatch the passed-in string to the appropriate place.  Ideally it should
/// flush the output channel as well, to ensure the message is not lost in
/// a buffer somewhere if the system crashes.
class LogDestination {
  public:
    virtual void write(int prio,char *msg) = 0;
    virtual void setLevel(LogLevel l) { _lev = l; };
    virtual ~LogDestination(void) {};
  protected:
    LogLevel _lev;
};

/**
 * \brief <span class=stat_bad>red</span> Sends log messages to a file.
 */
class Log2File: public LogDestination {
  public:
    Log2File(char *filename,LogLevel l=bode::INFO);
    virtual ~Log2File(void);
    virtual void write(int prio,char *msg);
  protected:
    std::ofstream *_out;
};

/**
 * \brief <span class=stat_bad>red</span> Sends log messages to the syslog daemon.
 */
class Log2Syslog: public LogDestination {
  public:
    Log2Syslog(LogLevel l=bode::INFO);
    virtual ~Log2Syslog(void);
    virtual void write(int prio,char *msg);
  protected:
    static const int facility = LOG_USER;
    static const int options = LOG_NOWAIT;
};

/**
 * \brief <span class=stat_bad>red</span> Sends log messages to stderr.
 */
class Log2Stderr: public LogDestination {
  public:
    Log2Stderr(LogLevel l=bode::WARNING);
    virtual ~Log2Stderr(void) {};
    virtual void write(int prio,char *msg);
};

/// \brief <span class=stat_bad>red</span> A basic logging class.
///
/// Dispatches log messages to (optionally) multiple destinations, based
/// on which log destination objects it is configured with.  By default
/// dispatches to stderr.  Logs are dispatched if their severity is at least
/// as high as the current log level, and otherwise discarded.
class Log {
  public:
    Log(LogLevel lev=INFO,std::string tag="BODE");
    virtual ~Log(void);
    void addDestination(LogDestination *dest);
    void setLevel(LogLevel lev);
    static Log *get(void);

    void fatal(char *fmt,...);
    void error(char *fmt,...);
    void warning(char *fmt,...);
    void info(char *fmt,...);
    void debug(char *fmt,...);

  protected:
    static Log *root;
    virtual void format(std::string &lev,char *fmt,va_list vl);
    virtual void dispatch(int prio);
    std::vector<LogDestination*> *_dest;
    LogLevel _lev;
    std::string _tag;
    static const int max_log_length = 1024;
    char msg_buffer[max_log_length];
};

Log *Log::root = NULL;

}

#endif
