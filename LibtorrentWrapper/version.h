#ifndef _GGS_LIBTORRENTWRAPPER_VERSION_H_
#define _GGS_LIBTORRENTWRAPPER_VERSION_H_

#ifndef HUDSON_BUILD
#define FILEVER          1,0,0,0
#define PRODUCTVER       FILEVER
#define STRFILEVER       "1,0,0,0"
#define STRPRODUCTVER    STRFILEVER

#define COMPANYNAME      "GGS"
#define FILEDESCRIPTION  "Developer version of GameNet LibtorrentWrapper library"
#define INTERNALNAME     "LibtorrentWrapper"
#define LEGALCOPYRIGHT   "Copyright(c) 2010 - 2012"

// UNDONE: ������ ������ ��� �� �������� ��� �������� �� �� ��� ���-���� ����� ��������.
#ifndef _DEBUG
  #define ORIGINALFILENAME "LibtorrentWrapperX86.dll"
#else
  #define ORIGINALFILENAME "LibtorrentWrapperX86d.dll"
#endif

#define PRODUCTNAME      "GameNet LibtorrentWrapper library"
#else
    #define FILEVER          $$MAJOR$$,$$MINOR$$,$$HUDSON_BUILD$$
    #define PRODUCTVER       FILEVER
    #define STRFILEVER       "$$MAJOR$$,$$MINOR$$,$$HUDSON_BUILD$$,$$GIT_REVISION$$"
    #define STRPRODUCTVER    STRFILEVER

#define COMPANYNAME      "$$COMPANYNAME$$"
#define FILEDESCRIPTION  "$$FILEDESCRIPTION$$"
#define INTERNALNAME     "$$INTERNALNAME$$"
#define LEGALCOPYRIGHT   "$$LEGALCOPYRIGHT$$"
#define ORIGINALFILENAME "$$FILENAME$$"
#define PRODUCTNAME      "$$PRODUCTNAME$$"
#endif

#endif // _GGS_LIBTORRENTWRAPPER_VERSION_H_
