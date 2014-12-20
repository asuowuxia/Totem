#ifndef ANIMATIONEDITOR_GLOBAL_H
#define ANIMATIONEDITOR_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef ANIMATIONEDITOR_LIB
# define ANIMATIONEDITOR_EXPORT Q_DECL_EXPORT
#else
# define ANIMATIONEDITOR_EXPORT Q_DECL_IMPORT
#endif

#endif // ANIMATIONEDITOR_GLOBAL_H
