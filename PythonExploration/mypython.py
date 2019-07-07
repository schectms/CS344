import random
from random import randint
import string


random1 = ''.join([random.choice(string.ascii_letters[0:26])for n in range(10)])

file1 = open("file1.txt", "w")
file1.write('%s\n' % random1)
file1.close()

print(random1)


random2 = ''.join([random.choice(string.ascii_letters[0:26])for n in range(10)])

file2 = open("file2.txt", "w")
file2.write('%s\n' % random2)
file2.close()

print(random2)

random3 = ''.join([random.choice(string.ascii_letters[0:26])for n in range(10)])

file3 = open("file3.txt", "w")
file3.write('%s\n' % random3)
file3.close()

print(random3)


ri = randint(1, 42)
print(ri)
ri2 = randint(1, 42)
print(ri2)

print(ri*ri2)

