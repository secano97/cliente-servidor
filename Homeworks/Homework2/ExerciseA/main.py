import argparse
import os
import sys
import logging
from time import time
from ipaddress import ip_address
from src.helpers.utils import *


def main():
    """ Server of the Chatvoice application
        Evil-Labs, UTP
    By default, this python file will start the development server 
    on the internal IP at a give port, if not port is supplied port 4000 will be
    used.

    Args:
          --port: An integer representing your Port on the localhost.  
          --ip_address: be considered to be IPv4 by default 0.0.0.0 or localhost.
    Returns:
        An open socket/threated connection, listening on port parameter and ipv4 address.
    Raises:
        ValueError: if there are too many connections or reach the 
        limit of 30 concurrent clients, Raise and phthreat error, given by the 
        zmq address
    """

    # Implement a Python arg parser using the standard lib
    parser = argparse.ArgumentParser(
        description=banner(),
        prog='ChatVoice-Server',
        epilog="Written by Hector F. Jimenez and Sebastian Cano"
    )
    parser.add_argument(
        '-l', '--log-file', help='Save a file with log entries in ascendent way \
        during session start')
    parser.add_argument('--ip', help='Displays top T IP adresses', type=int)
    parser.add_argument('-v','--verbose', help='Verbose output')
    args = parser.parse_args()
    print(args)

if __name__ == '__main__':
    main()
