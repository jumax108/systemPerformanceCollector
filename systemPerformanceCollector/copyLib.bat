set name=packetPointer

mkdir %name%

robocopy headers %name%/headers
robocopy release %name% *.pdb
robocopy release %name% *.lib

del release /Q /S

rd release /S /Q

pause