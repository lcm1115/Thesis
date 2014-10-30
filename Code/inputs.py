# File: inputs.py
# Author: Liam Morris
# Description: Given output from a blockparser txinfo command, determine which
#              input addresses were associated with that transaction.
import re
import sys

def findInputs(strm):
    output = []
    for line in strm:
        output.append(line.strip().replace(' ', ''))
    output = ''.join(output)
    # Find all sections of the output that correspond to an input address
    inputs = re.findall('input\[\d+\]={.*?}', output)
    # Determine the number of total inputs
    num_inputs = [int(s.split('=')[1]) for s in re.findall('nbInputs=\d+', output)]
    input_addresses = []
    index = 0
    num_transactions = 0
    # Build list of input addresses
    for num in num_inputs:
        curr_input_addresses = []
        for i in range(num):
            # Extract only the address hash and append it to the list
            curr_input_addresses.append(re.sub('.*scriptpaystoaddress(.*)}', '\g<1>', inputs[index]))
            index += 1
        input_addresses.append(curr_input_addresses)
        num_transactions += 1
    return input_addresses

if __name__ == '__main__':
    inputs = findInputs(sys.stdin)
    for i in inputs:
        for h in i:
            print(h)
        if i != inputs[-1]:
            print()
