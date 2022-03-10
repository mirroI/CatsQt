#ifndef CATS_GLOBAL_H
#define CATS_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(CATS_LIBRARY)
#  define CATS_EXPORT Q_DECL_EXPORT
#else
#  define CATS_EXPORT Q_DECL_IMPORT
#endif

#endif // CATS_GLOBAL_H
