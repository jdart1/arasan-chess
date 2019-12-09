pushd ..
7z u -tzip -orelease %1 README.md
7z u -tzip -orelease %1 src\*.cpp
7z u -tzip -orelease %1 src\*.h src\*.cpp src\*.c src\Makefile
7z u -tzip -orelease %1 src\CMakeLists.txt
7z u -tzip -orelease %1 src\release.bat
7z u -tzip -orelease %1 src\syzygy\*.*
7z u -tzip -orelease %1 src\syzygy\src\*.*
7z u -tzip -orelease %1 src\util\*.*
7z u -tzip -orelease %1 tools\*.*
7z u -tzip -orelease %1 gui\*.h gui\*.cpp  gui\gui.rc gui\res\*.*
7z u -tzip -orelease %1 gui\gui.sln gui\gui.vcxproj
7z u -tzip -orelease %1 gui\fonts\*.zip
7z u -tzip -orelease %1 prj\prj.sln prj\prj.vcxproj
7z u -tzip -orelease %1 book\basic.pgn book\eco
7z u -tzip -orelease %1 doc\programr.html doc\style_pg.css
7z u -tzip -orelease %1 doc\*license*.txt
7z u -tzip -orelease %1 doc\TODO doc\CHANGES doc\BUILD.md
7z u -tzip -orelease %1 tests\bt2630.epd tests\ecmgcp.epd
7z u -tzip -orelease %1 tests\arasan20.epd tests\pet.epd tests\prof
7z u -tzip -orelease %1 tests\eet.epd
7z u -tzip -orelease %1 tests\wacnew.epd tests\lapuce2.epd
7z u -tzip -orelease %1 tests\lapuce2.doc tests\iq4.epd tests\prof.epd
7z u -tzip -orelease %1 tests\eet.txt
7z u -tzip -orelease %1 tests\RESULTS tests\rating.txt
7z u -tzip -orelease %1 tests\tests.pl
7z u -tzip -orelease %1 gui\help\*.*
7z u -tzip -orelease %1 gui\help\html\*.*
7z u -tzip -orelease %1 gui\install\arasan.iss gui\install\readme.txt
popd

