import zmq
import math
import sys
import os

encryptation = 6 #number of bit of encryptation

class Hash_table:
    def __init__(self, k):
        self.k = pow(2,k)

        self.table = [None]*self.k


    def insert(self, key, value):
        self.table[key] = value

    def delete(self, key):
        self.table[key] = None

def Hashing_Function(x):
    return x%pow(2,encryptation)

def menu():
    x = Hash_table(encryptation)
    #print(x.table)

    x.insert(Hashing_Function(64),'dog')

    print(x.table)

    x.delete(Hashing_Function(64))

    print(x.table)


if __name__== '__main__':
	menu()
