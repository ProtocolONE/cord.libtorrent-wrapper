См. README в корне SVN для папки GNA 2.0
Дополнительные переменные среды окружения:
	BOOST_ROOT на корень папки с сдк. Буст должен быть собран с дефолтными путями для библиотек.

Следующие QT библиотеки нужны для работы проекта:
QtCore4.dll

Либторрент собирается следующими коммандами:
Debug:
bjam -j 4 toolset=msvc-10.0 boost=source boost-link=static geoip=off resolve-countries=off timer=performance dht-support=on encryption=off invariant-checks=off asserts=off pool-allocators=on deprecated-functions=off link=static runtime-link=shared variant=debug debug-symbols=on
Release:
bjam -j 4 toolset=msvc-10.0 boost=source boost-link=static geoip=off resolve-countries=off timer=performance dht-support=on encryption=off invariant-checks=off asserts=off pool-allocators=on deprecated-functions=off link=static runtime-link=shared variant=release