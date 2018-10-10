#pragma once

#include <QtCore/qglobal.h>

#ifdef LIBTORRENTWRAPPER_STATIC_LIB
 # define LIBTORRENTWRAPPER_EXPORT 
#else

#ifdef LIBTORRENTWRAPPER_LIB
# define LIBTORRENTWRAPPER_EXPORT Q_DECL_EXPORT
#else
# define LIBTORRENTWRAPPER_EXPORT Q_DECL_IMPORT
#endif

#endif

#define CRITICAL_LOG qCritical() << __FILE__ << __LINE__ << __FUNCTION__
#define WARNING_LOG qWarning() << __FILE__ << __LINE__ << __FUNCTION__
#define DEBUG_LOG qDebug() << __FILE__ << __LINE__ << __FUNCTION__
