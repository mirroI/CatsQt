#ifndef REACTORCORE_GLOBAL_H
#define REACTORCORE_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(REACTORCORE_LIBRARY)
#  define REACTORCORE_EXPORT Q_DECL_EXPORT
#else
#  define REACTORCORE_EXPORT Q_DECL_IMPORT
#endif

#endif // REACTORCORE_GLOBAL_H
