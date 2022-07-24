pushd ..
7z u -tzip -orelease %1 README.md
7z u -tzip -orelease %1 LICENSE
7z u -tzip -orelease %1 src\*.cpp
7z u -tzip -orelease %1 src\*.h src\*.cpp src\*.c src\Makefile
7z u -tzip -orelease %1 src\CMakeLists.txt
7z u -tzip -orelease %1 src\release.bat
7z u -tzip -orelease %1 src\syzygy\*.*
7z u -tzip -orelease %1 src\syzygy\src\*.*
7z u -tzip -r -orelease %1 src\nnue
7z u -tzip -orelease %1 src\util\*.*
7z u -tzip -orelease %1 tools\README tools\analyze.py tools\label_positions.py
7z u -tzip -orelease %1 tools\match.py tools\match_status.py tools\monitor.py
7z u -tzip -orelease %1 tools\playchess.py tools\stats\README.md tools\stats\*.py
7z u -tzip -orelease %1 gui\*.h gui\*.cpp gui\gui.rc gui\res\*.*
7z u -tzip -orelease %1 gui\gui.sln gui\gui.vcxproj gui\arasan.props
7z u -tzip -orelease %1 gui\fonts\*.zip
7z u -tzip -orelease %1 prj\prj.sln prj\prj.vcxproj
7z u -tzip -orelease %1 book\basic.pgn book\eco
7z u -tzip -orelease %1 doc\programr.html doc\style_pg.css
7z u -tzip -orelease %1 doc\TODO doc\CHANGES doc\BUILD.md
7z u -tzip -orelease %1 tests\bt2630.epd tests\ecmgcp.epd
7z u -tzip -orelease %1 tests\arasan21.epd tests\pet.epd tests\prof
7z u -tzip -orelease %1 tests\eet.epd
7z u -tzip -orelease %1 tests\wacnew.epd tests\lapuce2.epd
7z u -tzip -orelease %1 tests\lapuce2.doc tests\iq4.epd tests\prof.epd
7z u -tzip -orelease %1 tests\eet.txt
7z u -tzip -orelease %1 tests\RESULTS
7z u -tzip -orelease %1 tests\tests.pl
7z u -tzip -orelease %1 gui\help\*.*
7z u -tzip -orelease %1 gui\help\html\*.*
7z u -tzip -orelease %1 gui\install\arasan.iss gui\install\readme.txt
7z u -tzip -orelease %1 network\arasan-d10-20220723.nnue

