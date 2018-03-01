@echo off
navgen template.html .
if errorlevel 1 goto err
"C:\Chris\MyProjects\FixSelfUrl\Release\FixSelfUrl" *.html
if errorlevel 1 goto err
md Help
del /s Help\*.htm
C:\Chris\MyProjects\doc2web\release\doc2web /nospaces C:\Chris\MyProjects\ChordEase\Help\help.txt Help Contents.htm C:\Chris\MyProjects\ChordEase\doc\ChordEaseHelp.htm "ChordEase Help"
if errorlevel 1 goto err
cd Help
rem md images
rem copy C:\Chris\MyProjects\ChordEase\Help\images\*.* images
copy ..\helptopic.css content.css
navgen C:\Chris\MyProjects\ChordEase\Help\template.txt .
copy ..\helpheader.txt x
copy x + Contents.htm
echo ^<body^>^<html^> >>x
del Contents.htm
ren x Contents.htm
md printable
cd printable
move C:\Chris\MyProjects\ChordEase\doc\ChordEaseHelp.htm .
rem cd ..
rem cd ..
rem ren issues.html issues.htm
rem echo y | fsr issues.htm "<div id=body>" "<div id=widebody>"
rem ren issues.htm issues.html
goto exit
:err
pause Error!
:exit
