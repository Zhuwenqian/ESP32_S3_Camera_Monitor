#!/usr/bin/env python3
"""
Gzip验证工具 / Gzip Verification Tool
用于验证gzip压缩数据是否正确
Used to verify if gzip compressed data is correct
"""

import gzip
import sys

def verify_gzip(header_file):
    """
    验证gzip压缩数据
    Verify gzip compressed data
    
    参数 / Parameters:
        header_file: 头文件路径 / Header file path
    """
    
    # 读取头文件 / Read header file
    with open(header_file, 'r', encoding='utf-8') as f:
        content = f.read()
    
    # 提取压缩数据 / Extract compressed data
    lines = content.split('\n')
    compressed_data = bytearray()
    
    in_array = False
    for line in lines:
        if 'const uint8_t index_ov2640_html_gz[]' in line:
            in_array = True
            continue
        if in_array and line.strip() == '};':
            break
        if in_array:
            # 提取十六进制数据 / Extract hex data
            hex_values = [x.strip() for x in line.split(',') if x.strip() and x.strip().startswith('0x')]
            for hv in hex_values:
                compressed_data.append(int(hv, 16))
    
    print(f"压缩数据大小 / Compressed data size: {len(compressed_data)} bytes")
    
    # 尝试解压 / Try to decompress
    try:
        decompressed_data = gzip.decompress(compressed_data)
        print(f"解压后大小 / Decompressed size: {len(decompressed_data)} bytes")
        
        # 检查是否为有效的HTML / Check if valid HTML
        html_text = decompressed_data.decode('utf-8')
        if '<!DOCTYPE html>' in html_text or '<html' in html_text:
            print("✓ 解压成功，数据有效 / Decompression successful, data is valid")
            
            # 检查HTML结构 / Check HTML structure
            if '</html>' in html_text:
                print("✓ HTML结构完整 / HTML structure is complete")
            else:
                print("✗ HTML结构不完整 / HTML structure is incomplete")
                
            return True
        else:
            print("✗ 解压后的数据不是有效的HTML / Decompressed data is not valid HTML")
            return False
            
    except Exception as e:
        print(f"✗ 解压失败 / Decompression failed: {e}")
        return False

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("使用方法 / Usage: python verify_gzip.py header_file.h")
        sys.exit(1)
    
    header_file = sys.argv[1]
    verify_gzip(header_file)
