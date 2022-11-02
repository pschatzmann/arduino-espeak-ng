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
    #shutil.copytree('original/phsource', 'data/phsource', dirs_exist_ok=True) 
    #shutil.copytree('original/dictsource', 'data/dictsource', dirs_exist_ok=True) 
    shutil.copytree('original/espeak-ng-data', 'espeak-ng-data', dirs_exist_ok=True) 
    #shutil.copytree('original/src/speechPlayer/src', 'src/speechPlayer', dirs_exist_ok=True) 
    shutil.copy2('original/src/speechPlayer/include/speechPlayer.h', 'src/speechPlayer.h') 
    # ucd
    shutil.copytree('original/src/ucd-tools/src', 'src/ucd', dirs_exist_ok=True) 
    shutil.copy2('original/src/ucd-tools/src/include/ucd/ucd.h', 'src/ucd/ucd.h') 

# create link with the help of an #include
def link(linkTo, fileName):
    text_file = open(fileName, "w")
    content = "#include \""+linkTo+"\""
    text_file.write(content)
    text_file.close()

# create links for files available elsewhere
def link_files():
    # pcmaudio
    link('../../arduino/audio.h', 'src/pcaudiolib/audio.h') 
    link('../../arduino/audio_object.h', 'src/pcaudiolib/audio_object.h') 
    link('../../arduino/audio_audiotools_i2s.cpp', 'src/pcaudiolib/audio_audiotools_i2s.cpp') 
    # config
    link('../arduino/config.h', 'src/config.h') 
    link('../arduino/espeak.h', 'src/espeak.h') 
    link('../arduino/direntx.h', 'src/direntx.h') 

# deletes a file if it exists
def remove(file):
    if os.path.exists(file):
        os.remove(file)

# delete unnecessary files and directories
def cleanup():
    remove("src/libespeak-ng/mbrowrap.c")
    remove("src/libespeak-ng/mbrowrap.h")
    shutil.rmtree("src/speechPlayer/.deps", ignore_errors=True)
    #remove("src/speechPlayer/.dirstamp")
    shutil.rmtree("src/libespeak-ng/.deps", ignore_errors=True)
    remove("src/libespeak-ng/.dirstamp")
    shutil.rmtree("src/ucd/.deps", ignore_errors=True)
    remove("src/ucd/.dirstamp")
    remove("src/libespeak-ng/sPlayer.c")
    remove("src/libespeak-ng/sPlayer.h")

# replace texts in a file
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

# Change standard functionality
def apply_patches():
    res = command.run(["git", "apply", "arduino/patches/dirent.patch"]) 
    if res.exit!=0:
        print(res.output) 

# execute xxd on data file
def execute_xxd(infile, outfile):
    cmd = ["xxd","-i", infile, outfile]
    print(cmd)
    res = command.run(cmd) 
    #print(res.output) 


# traverse espeak-ng-data directory to create .h file from data
def create_data():
    includes = "#pragma once\n\n"
    for root, dirs, files in os.walk("espeak-ng-data"):
        path = root.split(os.sep)
        for file in files:
            if not file.startswith("."):
                new_path = root.replace("espeak-ng-data","src/data")
                new_file = new_path + "/"+file+".h"
                old_path = root+"/"+file
                new_file_incl = new_file.replace("src/","")
                includes += "#include \"" + new_file_incl +  "\"\n"
                print(new_path)
                os.makedirs(new_path, exist_ok = True)
                execute_xxd(old_path, new_file)
                file_replace_text(new_file,"unsigned char","const unsigned char")
                file_replace_text(new_file,"[]","[] PROGMEM")
        f = open("src/espeak-ng-data.h", "w")
        f.write(includes)
        f.close()

##-----------------------
## Main logic starts here
res = execute_git("https://github.com/espeak-ng/espeak-ng.git", "original")
if res.exit==0:
    clean_src()
    copy_files()
    link_files()
    cleanup()
    # convert_double()
    # conflict with string
    file_replace_text("src/libespeak-ng/compilembrola.c","basename(","basefilename(")
    file_replace_text("src/libespeak-ng/speech.h","#include <endian.h>               // for BYTE_ORDER, BIG_ENDIAN","// for BYTE_ORDER, BIG_ENDIAN\n#if HAVE_ENDIAN_H\n#include <endian.h>\n#endif\n")
    file_replace_text("src/libespeak-ng/speech.h","strcpy(path_home, PATH_ESPEAK_DATA);","if (path==NULL) path = PATH_ESPEAK_DATA;\n\n	strcpy(path_home, path);")
    file_replace_text("src/libespeak-ng/spect.c","#include <endian.h>", "// for BYTE_ORDER, BIG_ENDIAN\n#if HAVE_ENDIAN_H\n#include <endian.h>\n#endif\n")
    file_replace_text("src/libespeak-ng/voices.c","#include <dirent.h>","#include \"direntx.h\"")

    # apply_patches()
    create_data()
    print("setup completed")
else:
    print("Could not execute git command")