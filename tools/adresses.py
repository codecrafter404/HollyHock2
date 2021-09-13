from pathlib import Path
from prettytable import PrettyTable
x = PrettyTable()


_list = []


def find_all_addresses(folder):
    for path in Path(folder).rglob('*.s'):
        lines = path.read_text().split("\n")
        for line in lines:
            if line.startswith("DEFINE_OS_FUNC"):
                _list.append(
                    {
                        'file': path.absolute(),
                        'function': line.split(" ")[1],
                        'address': line.split(" ")[2]
                    }
                )


def main():
    x.field_names = ['Address', 'Function Name', 'File']
    find_all_addresses("../sdk/")
    _list.append({
        'address': '0x80128018',
        'function': 'Patch_FileLoader',
        'file': '...'
    })
    _list.append({
        'address': '0x808FCC10',
        'function': 'Patch_ModMarker',
        'file': '...'
    })
    _list.append({
        'address': '0x80065998',
        'function': 'PrintStandardErrorString',
        'file': '...'
    })

    print(_list)
    _list.sort(key=lambda x: x['address'])
    for index, address in enumerate(_list):
        if len(_list) > index + 1:
            address['address'] += '(' + str(hex(int(_list[index + 1]['address'][2:], 16) - int(address['address'][2:], 16))) + ')'
        x.add_row([address['address'], address['function'], address['file']])
    print(x)


if __name__ == '__main__':
    main()
