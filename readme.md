[![GitHub issues](https://img.shields.io/github/issues/h3ct0rjs/cliente-servidor.svg)](https://github.com/h3ct0rjs/cliente-servidor/issues)
[![GitHub forks](https://img.shields.io/github/forks/h3ct0rjs/cliente-servidor.svg)](https://github.com/h3ct0rjs/cliente-servidor/network)
[![GitHub stars](https://img.shields.io/github/stars/h3ct0rjs/cliente-servidor.svg)](https://github.com/h3ct0rjs/cliente-servidor/stargazers)
[![GitHub license](https://img.shields.io/github/license/h3ct0rjs/cliente-servidor.svg)](https://github.com/h3ct0rjs/cliente-servidor)
[![Twitter](https://img.shields.io/twitter/url/https/github.com/h3ct0rjs/cliente-servidor.svg?style=social)](https://twitter.com/intent/tweet?text=Wow:&url=https%3A%2F%2Fgithub.com%2Fh3ct0rjs%2Fcliente-servidor)
# Exercises of Client-Server Class
[![Python](https://www.python.org/static/community_logos/python-powered-w-100x40.png)](https://www.python.org/)
Exploring different Distributed Messaging libs for python
## Exercises
The following list is a set of exercises that we work in the class

| Exercise/Homework   |      Deadline      |  Status |
|----------|:-------------:|------:|
| Homework1:Basic Client-Server, explore times with multiple clients-ØMQ |  14-Feb | Delivered[OK] |
| Homework2:WhatsappClone,ExerciseA -ØMQ|  21-Feb | Delivered[-] |
| Homework2:WhatsappClone,ExerciseB -ØMQ|  21-Feb | Delivered[-] |
| Homework2:WhatsappClone,ExerciseC -ØMQ|  28-Feb | Delivered[-] |

# Running
Every exercise will contain the source code, if the source code use snippets from other proggrammer they will have the reference. In order to avoid problems with dependencies I'll suggest you use Virtualenv or the official python virtualenv(pipenv)
## Step 1. Install and Create a Virtualenv
```bash
user@pc$ sudo apt-get install python-virtualenv python3.6-venv
user@pc$ virtualenv --python=python3.6 clientserver
user@pc$ cd clientserver && source bin/activate 
```
with the official [pipenv](https://github.com/pypa/pipenv)

```bash
user@pc$ pip install pipenv
user@pc$ pipenv --python 3.6
```

## step 2. Clone the Github repository 
```bash
user@pc (clientserver)$ git clone git@github.com:h3ct0rjs/cliente-servidor.git
user@pc (clientserver)$ cd cliente-servidor
user@pc/cliente-servidor (clientserver)$ pip install -r requirements.txt    #This will install all the requirements
```
## Step 5. Run python Script
You will have two python scripts, client and server. Start the server and the start the client.
```bash
user@pc/cliente-servidor (clientserver)$ python server.py 
```
# Getting Help
You will have always an argument with ***-h*** showing the description and explaining the arguments.

```bash
user@pc/cliente-servidor (clientserver)$ python server.py  -h 
user@pc/cliente-servidor (clientserver)$ python client.py  -h 
```

### About 
Coded by
* Hector F. Jimenez S. , hfjimenez@utp.edu.co 
**Collaborators ** 

* Sebastian Cano U., sebastiancanouribe@utp.edu.co
* Rodrigo
License
----

MIT
