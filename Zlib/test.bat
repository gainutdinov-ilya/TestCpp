for /L %%i IN (0, 1, 9) DO TESTzip.exe f.txt test/fzip_%%i.txtz c %%i
for /L %%i IN (0, 1, 9) DO TESTzip.exe test/fzip_%%i.txtz test/ftext_%%i.txt d %%i