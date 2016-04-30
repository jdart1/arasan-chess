pushd ..
7z u -tzip -orelease %1 src\*.cpp
7z u -tzip -orelease %1 src\*.h src\*.cpp src\*.c src\Makefile src\Makefile.icl
7z u -tzip -orelease %1 src\release.bat
7z u -tzip -orelease %1 src\syzygy\*.*
7z u -tzip -orelease %1 gui\*.h gui\*.cpp  gui\gui.rc gui\res\*.*
7z u -tzip -orelease %1 gui\gui.sln gui\gui.vcxproj
7z u -tzip -orelease %1 gui\fonts\*.zip
7z u -tzip -orelease %1 prj\prj.sln prj\prj.vcxproj
7z u -tzip -orelease %1 book\basic.pgn book\eco doc\changes.txt
7z u -tzip -orelease %1 doc\readme.txt
7z u -tzip -orelease %1 doc\programr.html doc\style.css
7z u -tzip -orelease %1 doc\*license*.txt
7z u -tzip -orelease %1 tests\bt2630.epd tests\ecmgcp.epd
7z u -tzip -orelease %1 tests\arasan18.epd tests\pet.epd tests\prof
7z u -tzip -orelease %1 tests\eet.epd tests\tests
7z u -tzip -orelease %1 tests\wacnew.epd tests\lapuce2.epd
7z u -tzip -orelease %1 tests\lapuce2.doc tests\iq4.epd tests\prof.epd
7z u -tzip -orelease %1 tests\eet.txt
7z u -tzip -orelease %1 tests\results.txt tests\rating.txt
7z u -tzip -orelease %1 tests\tests.pl
7z u -tzip -orelease %1 gui\help\*.*
7z u -tzip -orelease %1 gui\help\html\*.*
7z u -tzip -orelease %1 gui\install\arasan.iss gui\install\readme.txt
popd

