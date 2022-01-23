import os
from pickle import TRUE
import sys

# Check if mklittlefs.exe exists, download if not.
# This can be overriden by setting the bool below to FALSE.
doDownload = True
doesExist = False

if not os.path.exists('./mklittlefs.exe'):
    if doDownload:
        print("\033[96m[i] mklittlefs.exe not found, downloading from GitHub (v3.0.0)...\033[0m")

        if not os.path.exists("./tmp"):
            os.mkdir("./tmp")
        
        import urllib.request
        if urllib.request.urlretrieve("https://github.com/earlephilhower/mklittlefs/releases/download/3.0.0/i686-w64-mingw32-mklittlefs-295fe9b.zip", "./tmp/tmp.zip"):
            print("\033[96m[i] > OK, extracting and moving file ...\033[0m")
        
        import zipfile
        with zipfile.ZipFile("./tmp/tmp.zip","r") as zip_ref:
            zip_ref.extractall("./tmp/")
        
        import shutil
        if shutil.move("./tmp/mklittlefs/mklittlefs.exe", "./mklittlefs.exe"):
            doesExist = True
            print("\033[96m[i] > OK, cleaning up...\033[0m")

        try:
            shutil.rmtree("./tmp")
        except OSError as e:
            print("[X] > Error: %s - %s." % (e.filename, e.strerror))

        print("\033[92m[i] > All done!\033[0m")
    else:
        print("\033[93m[!] mklittlefs.exe will not be downloaded.\033[0m")
        print("\033[93m    Please manually download it from 'https://github.com/earlephilhower/mklittlefs/releases'.\033[0m")
else:
    doesExist = True

if doesExist:
    Import("env")

    print("\033[96m[i] MKSPIFFSTOOL will be replaced with 'mklittlefs.exe'.\033[0m")
    env.Replace (MKSPIFFSTOOL = "mklittlefs.exe")
else:
    print("\033[91m[!] mklittlefs.exe is unavailable.\n    The filesystem might not build correctly!\033[0m")

# Acquire mklittlefs.exe from here: https://github.com/earlephilhower/mklittlefs/releases
# Place in root of project directory.