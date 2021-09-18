import argparse
import command_extract
import command_pack
import command_patch
import command_addresses


def parse_args():
    parser = argparse.ArgumentParser(
        description='hollyhock: tools and software for the fx-CP400'
    )
    subparsers = parser.add_subparsers(title='command')
    subparsers.required = True
    subparsers.dest = 'command'

    parser_extract = subparsers.add_parser(
        'extract',
        description='Extract firmware images from an OSupdateDLL.dll file.'
    )
    parser_extract.set_defaults(func=command_extract.go)
    parser_extract.add_argument(
        'dll_path',
        help='Path to the OSupdateDLL.dll file to extract from.'
    )
    parser_extract.add_argument(
        'fw3069_path',
        help='Path to save the extracted 3069 image to.'
    )
    parser_extract.add_argument(
        'fw3070_path',
        help='Path to save the extracted 3070 image to.'
    )

    parser_patch = subparsers.add_parser(
        'patch',
        description='Patch a firmware image.'
    )
    parser_patch.set_defaults(func=command_patch.go)
    parser_patch.add_argument(
        'patches_dir',
        help='Path to the directory containing the patch information.'
    )
    parser_patch.add_argument(
        'fw3070_path',
        help='Path to the 3070 firmware image to patch.'
    )
    parser_patch.add_argument(
        'patched_fw3070_path',
        help='Path to save the patched 3070 firmware image to.'
    )

    parser_pack = subparsers.add_parser(
        'pack',
        description='Pack a modified firmware image ready to be embedded in an OSupdateDLL.dll file.'
    )
    parser_pack.set_defaults(func=command_pack.go)
    parser_pack.add_argument(
        'fw3070_path',
        help='Path to the 3070 image to pack into the format required by the DLL.'
    )
    parser_pack.add_argument(
        'packed_fw3070_path',
        help='Path to save the packed 3070 image to.'
    )
    parser_pack.add_argument(
        'dll_path',
        help='Path to the DLL which the packed image will be embedded in (will not be modified).'
    )
    parser_addresses = subparsers.add_parser(
        'addresses',
        description='Calculate/Search for address offsets(in file and Memory)'
    )
    parser_addresses_subparser = parser_addresses.add_subparsers()
    parser_addresses_search = parser_addresses_subparser.add_parser(
        'search',
        description='Search for the address.'
    )
    parser_addresses_search.set_defaults(func=command_addresses.go_search)
    parser_addresses_search.add_argument(
        'old_fw',
        help='Path to the old firmware image(to extract binary segments used for search)'
    )
    parser_addresses_search.add_argument(
        'old_addresses',
        help='Path to the old addresses.json file'
    )
    parser_addresses_search.add_argument(
        'new_fw',
        help='Path to the "new" firmware image to search in.'
    )
    parser_addresses_offset = parser_addresses_subparser.add_parser(
        'offset',
        description='Sort the addresses in addresses.json and prints them pretty with offsets'
                    '(Useful for manual address searching) '
    )
    parser_addresses_offset.set_defaults(func=command_addresses.go_offset)
    parser_addresses_offset.add_argument(
        'addresses',
        help='Path to addresses.json'
    )
    parser_addresses_json = parser_addresses_subparser.add_parser(
        'json',
        description='Walk through all dirs and search for addresses in *.s files'
    )
    parser_addresses_json.set_defaults(func=command_addresses.generate_addresses_json)
    parser_addresses_json.add_argument(
        'directory',
        help='Directory to search in.'
    )
    parser_addresses_apply = parser_addresses_subparser.add_parser(
        'apply',
        description='Walk through all dirs and apply addresses in addresses.json'
    )
    parser_addresses_apply.set_defaults(func=command_addresses.apply_addresses)
    parser_addresses_apply.add_argument(
        'addresses',
        help='Addresses file with addresses to replace with.'
    )
    parser_addresses_apply.add_argument(
        'directory',
        help='Directory with *.s file to apply to'
    )
    parser_addresses_convert = parser_addresses_subparser.add_parser(
        'convert',
        description='Use this to convert between inMemory addresses or file offsets(+/- 80.000.000)'
    )
    parser_addresses_convert.set_defaults(func=command_addresses.go_convert)
    parser_addresses_convert.add_argument(
        'addresses',
        help='Addresses file to convert to.'
    )

    return parser.parse_args()


def main():
    args = parse_args()
    args.func(args)


if __name__ == '__main__':
    main()
