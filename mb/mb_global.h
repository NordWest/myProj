#ifndef MB_GLOBAL_H
#define MB_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(MB_LIBRARY)
#  define MBSHARED_EXPORT Q_DECL_EXPORT
#else
#  define MBSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // MB_GLOBAL_H
