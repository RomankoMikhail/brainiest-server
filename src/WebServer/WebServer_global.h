#ifndef WEBSERVER_GLOBAL_H
#define WEBSERVER_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(WEBSERVER_LIBRARY)
#  define WEBSERVER_EXPORT Q_DECL_EXPORT
#else
#  define WEBSERVER_EXPORT Q_DECL_IMPORT
#endif

#endif // WEBSERVER_GLOBAL_H
