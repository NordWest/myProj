#ifndef RADA_GLOBAL_H
#define RADA_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(RADA_LIBRARY)
#  define RADASHARED_EXPORT Q_DECL_EXPORT
#else
#  define RADASHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // RADA_GLOBAL_H
