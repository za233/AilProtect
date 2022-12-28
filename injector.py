import pefile
import mmap
import os

def align(val_to_align, alignment):
    return ((val_to_align + alignment - 1) // alignment) * alignment

def patch_bytes(filepath, out_filepath, rva, bytes):
    pe = pefile.PE(filepath)
    pe.set_bytes_at_rva(rva, bytes)
    pe.write(out_filepath)

def inject_data_to_section(filepath, out_filepath, section_name, code):
    original_size = os.path.getsize(filepath)
    pe = pefile.PE(filepath)
    raw_name = section_name + b'\x00' * (8 - len(section_name))
    number_of_section = pe.FILE_HEADER.NumberOfSections
    file_alignment = pe.OPTIONAL_HEADER.FileAlignment
    section_alignment = pe.OPTIONAL_HEADER.SectionAlignment
    new_section_offset = (pe.sections[number_of_section - 1].get_file_offset() + 40)
    

    characteristics = 0xE0000020
    virtual_offset = pe.sections[-1].VirtualAddress + pe.sections[-1].Misc_VirtualSize
    raw_offset = pe.sections[-1].PointerToRawData + pe.sections[-1].SizeOfRawData

    virtual_offset = align(virtual_offset, section_alignment)
    raw_offset = align(raw_offset, file_alignment)

    virtual_size = align(len(code), section_alignment)
    raw_size = align(len(code), file_alignment)
    
    pe.set_bytes_at_offset(new_section_offset, raw_name)

    pe.set_dword_at_offset(new_section_offset + 8, virtual_size)
    pe.set_dword_at_offset(new_section_offset + 12, virtual_offset)
    pe.set_dword_at_offset(new_section_offset + 16, raw_size)
    pe.set_dword_at_offset(new_section_offset + 20, raw_offset)
    pe.set_bytes_at_offset(new_section_offset + 24, (12 * b'\x00'))
    pe.set_dword_at_offset(new_section_offset + 36, characteristics)
    
    pe.set_bytes_at_offset(raw_offset, code)
    pe.FILE_HEADER.NumberOfSections += 1
    pe.OPTIONAL_HEADER.SizeOfImage = virtual_size + virtual_offset
    pe.write(out_filepath)

    fd = open(out_filepath, 'a+b')
    map = mmap.mmap(fd.fileno(), 0, access=mmap.ACCESS_WRITE)
    map.resize(original_size + align(len(code), file_alignment))
    map.close()
    fd.close()

    pe = pefile.PE(out_filepath)
    raw_offset = pe.sections[-1].PointerToRawData
    pe.set_bytes_at_offset(raw_offset, code)
    pe.write(out_filepath)
    return virtual_offset

    
