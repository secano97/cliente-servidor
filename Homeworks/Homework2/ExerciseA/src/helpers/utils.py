'''
file : util.py
Description:Utilities for Python programs,  
            Computer Programming and other usage
feedback : hfjimenez@utp.edu.co 
year : 2016-2018
'''
from random import randint
import subprocess
import os
# Set of colors in order to check status in the terminal
reset = '\x1b[0m'    # reset all colors to white on black
bold = '\x1b[1m'     # enable bold text
uline = '\x1b[4m'    # enable underlined text
nobold = '\x1b[22m'  # disable bold text
nouline = '\x1b[24m'  # disable underlined text
red = '\x1b[31m'     # red text
green = '\x1b[32m'   # green text
blue = '\x1b[34m'    # blue text
cyan = '\x1b[36m'    # cyan text
white = '\x1b[37m'   # white text (use reset unless it's only temporary)
yellow = '\x1b[33m'
# Nomenclaturas :
# err : Mensaje que genera algun tipo de error
# ok : Mensaje que genera una operacion correcta
# warning : Mensaje que genera una operacion de atencion
# info : Mensaje que presenta una operacion de informacion
# atn : Mensaje puesto para establecer ingreso e datos
err = '{}{}[✘✘✘]{} '.format(bold, red, reset)
ok = '{}{}[✓]{} '.format(bold, cyan, reset)
warning = '{}{}[~]{} '.format(bold, yellow, reset)
info = '{}{}[+]{} '.format(bold, white, reset)
atn = '{}{}[!]{} '.format(bold, yellow, reset)
kbr = '{}{}[>>]{} '.format(bold, white, reset)
version__ = '{}0.1v{}'.format(cyan, reset)
authors = '{}{}Héctor F. Jimenez a.k.a c1b3rh4ck'.format(bold, red, reset)
emails = '{}\n\thfjimenez@utp.edu.co{}'.format(white, reset)
topic = '{}    Client Server Class\n{}'.format(white, reset)


def cls():
    """Screen cleaner.
      Clean the screen. 
      Returns :
        None
    """
    subprocess.call(['clear'], shell=False)
l_art = [
    """
    {}
    ███████╗██╗   ██╗██╗██╗         
    ██╔════╝██║   ██║██║██║         
    █████╗  ██║   ██║██║██║         
    ██╔══╝  ╚██╗ ██╔╝██║██║         {}
    ███████╗ ╚████╔╝ ██║███████╗    
    ╚══════╝  ╚═══╝  ╚═╝╚══════╝                                    
    ██╗      █████╗ ██████╗ ███████╗
    ██║     ██╔══██╗██╔══██╗██╔════╝
    ██║     ███████║██████╔╝███████╗{}
    ██║     ██╔══██║██╔══██╗╚════██║ 
    ███████╗██║  ██║██████╔╝███████║
    ╚══════╝╚═╝  ╚═╝╚═════╝ ╚══════╝
       Concurrent Chat Voice V0.1
    {}""".format(yellow, blue, red, reset), ]

client_art = """{}
  ,    ,    /\   /\

  /( /\ )\  _\ \_/ /_
  |\_||_/| < \_   _/ >
  \______/  \|0   0|/{}
    _\/_   _(_  ^  _)_
   ( () ) /`\|V'''V|/\

     ()   \  \_____/  /{}
     ()   /\   )=(   /\
    
     ()  /  \_/\=/\_/  \
     \n Client Chat Voice V0.1{}
""".format(yellow, blue, red, reset)
def banner():
    """ Banner for the server part
    Returns:
      None
    """
    cls()
    art = l_art[randint(0, len(l_art) - 1)]
    print('{}{} {}'.format(art, topic, emails))

def bannerc():
    """ Banner for the client part
    Returns:
      None
    """
    cls()
    art = client_art
    print('{}{} {}'.format(art, topic, emails))

def loadFiles(path):
    """LoadFiles
    Args:
        path: your absolute working path

    Returns:
        a dictionary with the list of file names

    """
    files = {}
    dataDir = os.fsencode(path)
    for file in os.listdir(dataDir):
        filename = os.fsdecode(file)
        print ("{} loading {}".format(info,filename))
        files[filename] = files
    return files