/****************************************************************************
** This file is a part of Syncopate Limited GameNet Application or it parts.
**
** Copyright (�) 2011 - 2012, Syncopate Limited and/or affiliates.
** All rights reserved.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
****************************************************************************/

#ifndef LIBTORRENTWRAPPER_GLOBAL_H
#define LIBTORRENTWRAPPER_GLOBAL_H

#include <QtCore/qglobal.h>

// ���������� ��� ������� ������ ������.
#ifdef VLD_CHECK_ENABLED
#include <vld.h>
#pragma comment(lib, "vld.lib")
#endif


#ifdef LIBTORRENTWRAPPER_LIB
# define LIBTORRENTWRAPPER_EXPORT Q_DECL_EXPORT
#else
# define LIBTORRENTWRAPPER_EXPORT Q_DECL_IMPORT
#endif

#define CRITICAL_LOG qCritical() << __FILE__ << __LINE__ << __FUNCTION__
#define WARNING_LOG qWarning() << __FILE__ << __LINE__ << __FUNCTION__
#define DEBUG_LOG qDebug() << __FILE__ << __LINE__ << __FUNCTION__

#endif // LIBTORRENTWRAPPER_GLOBAL_H