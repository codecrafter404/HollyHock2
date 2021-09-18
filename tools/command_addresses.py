from pathlib import Path


# Extract the addresses from sdk folder(from the *.s File)
def find_all_addresses(folder):
    _list = []
    for path in Path(folder).rglob('*.s'):
        lines = path.read_text().split("\n")
        for line in lines:
            if line.startswith("DEFINE_OS_FUNC"):
                _list.append(
                    {
                        'file': str(path),
                        'FunctionName': line.split(" ")[1],
                        'startAddress': line.split(" ")[2]
                    }
                )
    return _list


# Extracts the Addresses and displays them with difference (using PrettyTable)
def extract_addresses_with_difference(directory):
    from prettytable import PrettyTable
    x = PrettyTable()
    x.field_names = ['Address', 'Function Name', 'File']

    directory.sort(key=lambda k: k['address'])
    for index, address in enumerate(directory):
        if len(directory) > index + 1:
            address['address'] += '(' + str(
                hex(int(directory[index + 1]['address'][2:], 16) - int(address['address'][2:], 16))) + ')'
        x.add_row([address['address'], address['function'], address['file']])
    print(x)


# Convert from 'FunktionName = 0xStart -> 0xEnd' to json format
def convert_file_to_json(input_file, out):
    import json
    lines = []
    with open(mode="r", file=input_file) as x:
        lines = x.readlines()
    with open(mode='w', file=out) as x:
        addresses = []
        for line in lines:
            split = line.split(' ')
            addresses.append({
                'FunctionName': split[0],
                'startAddress': split[2],
                'endAddress': split[4].replace('\n', '')
            })
        x.write(json.dumps({
            'isInMemory': True,
            'addresses': addresses
        }))


# Converts addresses from inMemory to file Offsets
def convert_to_file_addresses(json_file, memory_size='0x80000000'):
    import json
    file_content = {}
    with open(json_file, 'r') as x:
        file_content = json.loads(''.join(x.readlines()))

    if not file_content['isInMemory']:
        print('File is already converted to inFileAddress.')
        return

    for x in file_content['addresses']:
        x['startAddress'] = str(hex(int(x['startAddress'], 16) - int(memory_size, 16)))
        x['endAddress'] = str(hex(int(x['endAddress'], 16) - int(memory_size, 16)))

    file_content['isInMemory'] = False
    with open(json_file, 'w') as x:
        x.write(json.dumps(file_content))


# Converts addresses from inMemory to file Offsets
def convert_to_memory_address(json_file, memory_size='0x80000000'):
    import json
    file_content = {}
    with open(json_file, 'r') as x:
        file_content = json.loads(''.join(x.readlines()))

    if file_content['isInMemory']:
        print('File is already converted to inMemory.')
        return

    for x in file_content['addresses']:
        x['startAddress'] = str(hex(int(x['startAddress'], 16) + int(memory_size, 16)))
        x['endAddress'] = str(hex(int(x['endAddress'], 16) + int(memory_size, 16)))

    file_content['isInMemory'] = True
    with open(json_file, 'w') as x:
        x.write(json.dumps(file_content))


# Used to get binary segments -> they can be used to search in binary files
def get_search_binary_data(address_file, firmware_image):
    import json
    file_content = {}
    with open(address_file, 'r') as x:
        file_content = json.loads(''.join(x.readlines()))
    if file_content['isInMemory']:
        print("The address file is in inMemory Mode")
        return
    result = []
    for x in file_content['addresses']:
        data = {}
        with open(firmware_image, 'rb') as y:
            y.seek(int(x['startAddress'], 16), 1)
            data['FunctionName'] = x['FunctionName']
            data['binary'] = y.read(int(x['endAddress'], 16) - int(x['startAddress'], 16))
        result.append(data)
    return result


# Uses the binary segments from the old firmware to get the new addresses
def extract_addresses_from_binary_firmware(firmware_file, binary_search_data, address_file):
    import json
    file_content = {}
    with open(address_file, 'r') as x:
        file_content = json.loads(''.join(x.readlines()))

    firmware = b''
    with open(firmware_file, 'rb') as x:
        firmware = b''.join(x.readlines())

    result = []
    for i, x in enumerate(binary_search_data):
        if file_content['addresses'].__getitem__(i) is None:
            return
        data = {}

        try:
            data['FunctionName'] = file_content['addresses'][i]['FunctionName']
            data['startAddress'] = hex(firmware.index(x['binary']))
            data['endAddress'] = hex(int(file_content['addresses'][i]['endAddress'], 16) - int(
                file_content['addresses'][i]['startAddress'], 16) + int(data['startAddress'], 16)
                                     )
        except Exception as x:
            print('Can\'t find Address for Function:', file_content['addresses'][i]['FunctionName'])
        result.append(data)
    return (
        {
            'isInMemory': False,
            'addresses': result
        }
    )


def go_search(args):
    print(extract_addresses_from_binary_firmware(
        args.old_fw,
        get_search_binary_data(args.old_addresses, args.new_fw),
        args.old_addresses
    ))


def go_offset(args):
    import json
    file_content = ''
    with open(args.addresses, 'r') as x:
        file_content = json.loads(''.join(x.readlines()))
    dir = []
    for x in file_content['addresses']:
        file = ""
        if x.get('file') is not None:
            file = x['file']
        else:
            file = '...'
        dir.append({
            'address': x['startAddress'],
            'function': x['FunctionName'],
            'file': file
        })
    extract_addresses_with_difference(dir)


def generate_addresses_json(args):
    import json
    print(
        json.dumps(
            {
                'isInMemory': True,
                'addresses': json.dumps(find_all_addresses(args.directory))
            }
        )
    )


def go_convert(args):
    import json
    file_content = {}
    with open(args.addresses, 'r') as x:
        file_content = json.loads(''.join(x.readlines()))
    if file_content['isInMemory']:
        convert_to_file_addresses(args.addresses)
        print('Successfully converted to File offsets.')
    else:
        convert_to_memory_address(args.addresses)
        print('Successfully converted to inMemory offsets.')


def apply_addresses(args):
    import json
    file_content = {}
    with open(args.addresses, 'r') as x:
        file_content = json.loads(''.join(x.readlines()))
    if not file_content['isInMemory']:
        print('The addresses file is not converted to inMemory')
        return
    apply_count = 0
    all_count = 0
    for x in find_all_addresses(args.directory):
        all_count += 1
        for y in file_content['addresses']:
            if str(y['FunctionName']).lower() == str(x['FunctionName']).lower():
                print('Apply', x['FunctionName'])
                apply_count += 1
                data_write = []
                with open(x['file'], 'r') as z:
                    for line in z.readlines():
                        if line.startswith('DEFINE_OS_FUNC'):
                            if line.split(' ')[1] == y['FunctionName']:
                                data_write.append(line.replace(x['startAddress'], y['startAddress']))
                            else:
                                data_write.append(line)
                        else:
                            data_write.append(line)
                with open(x['file'], 'w') as z:
                    z.writelines(data_write)
    print('===[Applied {}/{}]==='.format(apply_count, all_count))
