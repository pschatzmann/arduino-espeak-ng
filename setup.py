# setup of Arduino Library from https://github.com/espeak-ng/espeak-ng.git 
import os, command, shutil

# get latest original source code
def execute_git(fromUrl, name):
    if not os.path.exists(name):
        res = command.run(["git", "clone", fromUrl, name]) 
    else:
        currentDir = os.getcwd()
        os.chdir(name)
        res = command.run(["git","pull"]) 
        os.chdir(currentDir)
    print(res.output) 
    return res

# make sure that we have an empty src directory
def clean_src():
    if os.path.exists("src"):
        shutil.rmtree("src")
    if not os.path.exists("src"):
        os.mkdir("src")
        os.mkdir("src/ucd")
        os.mkdir("src/pcaudiolib")

# copy all relevant files
def copy_files():
    shutil.copytree('original/src/include/espeak', 'src', dirs_exist_ok=True) 
    shutil.copytree('original/src/include/espeak-ng', 'src/espeak-ng', dirs_exist_ok=True) 
    shutil.copytree('original/src/libespeak-ng', 'src/libespeak-ng', dirs_exist_ok=True) 
    shutil.copytree('original/docs', 'docs', dirs_exist_ok=True) 
    shutil.copytree('original/phsource', 'data/phsource', dirs_exist_ok=True) 
    shutil.copytree('original/dictsource', 'data/dictsource', dirs_exist_ok=True) 
    shutil.copytree('original/espeak-ng-data', 'data/espeak-ng-data', dirs_exist_ok=True) 
    shutil.copytree('original/src/speechPlayer/src', 'src/speechPlayer', dirs_exist_ok=True) 
    shutil.copy2('original/src/speechPlayer/include/speechPlayer.h', 'src/speechPlayer.h') 
    # ucd
    shutil.copytree('original/src/ucd-tools/src', 'src/ucd', dirs_exist_ok=True) 
    shutil.copy2('original/src/ucd-tools/src/include/ucd/ucd.h', 'src/ucd/ucd.h') 
    # pcmaudio
    shutil.copy2('original-pcaudio/src/include/pcaudiolib/audio.h', 'src/pcaudiolib/audio.h') 
    shutil.copy2('data/bootstrap/audio_audiotools_i2s.cpp', 'src/pcaudiolib/audio_audiotools_i2s.cpp') 
    # config
    shutil.copy2('data/bootstrap/config.h', 'src/config.h') 

def cleanup():
    os.remove("src/libespeak-ng/mbrowrap.c")
    os.remove("src/libespeak-ng/mbrowrap.h")
    shutil.rmtree("src/speechPlayer/.deps")
    os.remove("src/speechPlayer/.dirstamp")
    shutil.rmtree("src/libespeak-ng/.deps")
    os.remove("src/libespeak-ng/.dirstamp")
    shutil.rmtree("src/ucd/.deps")
    os.remove("src/ucd/.dirstamp")


def file_replace_text(fileName, fromStr, toStr):
        text_file = open(fileName, "r")
        # read whole file to a string
        data = text_file.read()
        text_file.close()
        # write updated file
        text_file = open(fileName, "w")
        new_txt = data.replace(fromStr,toStr)
        text_file.write(new_txt)
        text_file.close()

# convert double to float
def convert_double():
    arr = os.listdir("src/speechPlayer")
    for file in arr:
        if not "." in file:
            print(file)
            fpath = "src/speechPlayer/"+file
            file_replace_text(fpath, "double","float")

## setup

res = execute_git("https://github.com/espeak-ng/pcaudiolib.git", "original-pcaudio")
res = execute_git("https://github.com/espeak-ng/espeak-ng.git", "original")
if res.exit==0:
    clean_src()
    copy_files()
    cleanup()
    convert_double()
    # conflict with string
    file_replace_text("src/libespeak-ng/compilembrola.c","basename(","basefilename(")
    print("setup completed")
else:
    print("Could not execute git command")