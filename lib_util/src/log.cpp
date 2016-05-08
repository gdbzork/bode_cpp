#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <cstdarg>
#include <time.h>
#include <syslog.h>

#include "bode/log.h"
#include "bode/bodeException.h"

bode::Log2File::Log2File(char *filename,LogLevel l) {
  _out = new std::ofstream(filename);
  _lev = l;
}

bode::Log2File::~Log2File(void) {
  _out->flush();
  _out->close();
  delete _out;
}

void bode::Log2File::write(int prio,char *msg) {
  if (prio <= _lev) {
    (*_out) << msg << std::endl;
    _out->flush();
  }
}

bode::Log2Syslog::Log2Syslog(LogLevel l) {
  _lev = l;
  openlog("",options,facility);
}

bode::Log2Syslog::~Log2Syslog(void) {
  closelog();
}

void bode::Log2Syslog::write(int prio,char *msg) {
  if (prio <= _lev) {
    syslog(prio,msg);
  }
}

bode::Log2Stderr::Log2Stderr(LogLevel l) {
  _lev = l;
}

void bode::Log2Stderr::write(int prio,char *msg) {
  if (prio <= _lev) {
    fprintf(stderr,"%s\n",msg);
    fflush(stderr);
  }
}

/// \brief Create a new logger.  
bode::Log::Log(bode::LogLevel l,std::string t) {
  _dest = new std::vector<LogDestination*>();
  _lev = l;
  _tag = t;
}

bode::Log::~Log(void) {
  std::vector<LogDestination*>::iterator it;
  for (it=_dest->begin();it<_dest->end();it++) {
    delete *it;
  }
  delete _dest;
}

/// \brief Add a new log destination to the logger.
///
/// By default no destinations are added, so the logger will not do much.
void bode::Log::addDestination(LogDestination *dest) {
  _dest->push_back(dest);
}

/// \brief Set the current logging level.
void bode::Log::setLevel(LogLevel lev) {
  _lev = lev;
}

/// \brief Get the default logger.
bode::Log *bode::Log::get(void) {
  if (root == NULL) {
    root = new Log();
    root->addDestination(new Log2Stderr());
  }
  return root;
}

void bode::Log::format(std::string &lev,char *fmt,va_list vl) {
  char time_buffer[max_log_length];
  char fmt_buffer[max_log_length];
  time_t tms;

  tms = time(NULL);
  strftime(time_buffer,max_log_length,"%F %T",localtime(&tms));
  sprintf(fmt_buffer,"[%s] %s %s %s",time_buffer,
                                     _tag.c_str(),
                                     lev.c_str(),
                                     fmt);
//  va_start(vl,fmt);
  vsnprintf(msg_buffer,max_log_length,fmt_buffer,vl);
//  va_end(vl);
//  fprintf(stderr,"msg buffer: '%s'",msg_buffer);
}

void bode::Log::dispatch(int prio) {
  std::vector<LogDestination*>::iterator it;
  for (it=_dest->begin();it<_dest->end();it++) {
    (*it)->write(prio,msg_buffer);
  }
}

/// \brief Log a fatal error.
///
/// This method does not return, but rather throws a FatalLogException.
void bode::Log::fatal(char *fmt,...) {
  va_list vl;
  std::string tag = "FATAL";

  va_start(vl,fmt);
  format(tag,fmt,vl);
  va_end(vl);
  dispatch(FATAL);
  throw bode::LogError(msg_buffer);
}

void bode::Log::error(char *fmt,...) {
  if (_lev >= ERROR) {
    va_list vl;
    std::string tag = "ERROR";
    va_start(vl,fmt);
    format(tag,fmt,vl);
    va_end(vl);
    dispatch(ERROR);
  }
}

void bode::Log::warning(char *fmt,...) {
  if (_lev >= WARNING) {
    va_list vl;
    std::string tag = "WARNING";
    va_start(vl,fmt);
    format(tag,fmt,vl);
    va_end(vl);
    dispatch(WARNING);
  }
}

void bode::Log::info(char *fmt,...) {
  if (_lev >= INFO) {
    va_list vl;
    std::string tag = "INFO";
    va_start(vl,fmt);
    format(tag,fmt,vl);
    va_end(vl);
    dispatch(INFO);
  }
}

void bode::Log::debug(char *fmt,...) {
  if (_lev >= DEBUG) {
    va_list vl;
    std::string tag = "DEBUG";
    va_start(vl,fmt);
    format(tag,fmt,vl);
    va_end(vl);
    dispatch(DEBUG);
  }
}
