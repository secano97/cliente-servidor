import zmq
import math
import sys
import os

tam = 64  #Tamaño de la hash table

def menu():
    table = [None]*tam
    print(table)

    print(Hashing_Function(64))

    insert(table,64,'dog')

    print(table)


def Hashing_Function(x):
    return x%tam

def insert(table, key, value):
    table[Hashing_Function(key)] = value



if __name__== '__main__':
	menu()
