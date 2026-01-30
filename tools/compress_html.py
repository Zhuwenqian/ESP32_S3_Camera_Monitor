#!/usr/bin/env python3
"""
Gzip压缩工具 / Gzip Compression Tool
用于将HTML文件压缩为gzip格式，以节省ESP32程序存储空间
Used to compress HTML files to gzip format to save ESP32 program storage space

使用方法 / Usage:
    python compress_html.py input.html output.h
"""

import gzip
import sys
import os

def compress_to_header(input_file, output_file, var_name="index_ov2640_html_gz"):
    """
    将HTML文件压缩并转换为C头文件格式
    Compress HTML file and convert to C header format
    
    参数 / Parameters:
        input_file: 输入HTML文件路径 / Input HTML file path
        output_file: 输出头文件路径 / Output header file path
        var_name: 变量名 / Variable name
    """
    
    # 读取HTML文件 / Read HTML file
    with open(input_file, 'rb') as f:
        html_data = f.read()
    
    print(f"原始大小 / Original size: {len(html_data)} bytes")
    
    # 使用gzip压缩（最高压缩级别9）/ Compress with gzip (maximum compression level 9)
    compressed_data = gzip.compress(html_data, compresslevel=9)
    
    print(f"压缩后大小 / Compressed size: {len(compressed_data)} bytes")
    print(f"压缩率 / Compression ratio: {(1 - len(compressed_data) / len(html_data)) * 100:.1f}%")
    
    # 生成C头文件内容 / Generate C header content
    header_content = f"""/*
 * 文件名 / Filename : {os.path.basename(output_file)}
 * 
 * 文件用途 / File Purpose :
 * 本文件包含Web界面的HTML代码（gzip压缩格式），提供完整的监控控制界面
 * This file contains HTML code for the web interface (gzip compressed), providing a complete monitoring control interface
 * 
 * 技术说明 / Technical Notes:
 * - 使用gzip压缩以节省程序存储空间 / Uses gzip compression to save program storage space
 * - 原始大小: {len(html_data)} bytes / Original size: {len(html_data)} bytes
 * - 压缩后大小: {len(compressed_data)} bytes / Compressed size: {len(compressed_data)} bytes
 * - 压缩率: {(1 - len(compressed_data) / len(html_data)) * 100:.1f}% / Compression ratio: {(1 - len(compressed_data) / len(html_data)) * 100:.1f}%
 * - 节省空间: {len(html_data) - len(compressed_data)} bytes / Space saved: {len(html_data) - len(compressed_data)} bytes
 * 
 * 功能模块 / Function Modules:
 * 1. 视频显示区域：显示实时视频流 / Video display area: displays real-time video stream
 * 2. 拍照控制区域：控制拍照功能 / Photo capture control area: controls photo capture function
 * 3. SD卡信息区域：显示SD卡存储信息（GB单位）/ SD card info area: displays SD card storage information (GB unit)
 * 4. 运行时长区域：显示系统运行时长（HH:MM:SS格式）/ Uptime area: displays system uptime (HH:MM:SS format)
 * 5. 摄像头设置区域：调节分辨率、图像质量、亮度、对比度、饱和度 / Camera settings area: adjusts resolution, quality, brightness, contrast, saturation
 * 6. 云台控制区域：控制舵机旋转 / Pan-tilt control area: controls servo rotation
 * 7. 语言选择功能：支持英语和中文切换，默认为英语 / Language selection: supports English and Chinese switching, default is English
 * 8. 主题选择功能：支持深色和浅色主题切换，默认为浅色 / Theme selection: supports dark and light theme switching, default is light
 * 
 * 创建日期 / Creation Date : 2026-01-30
 */

// Gzip compressed HTML data / Gzip压缩的HTML数据
#define {var_name.upper()}_LEN {len(compressed_data)}
const uint8_t {var_name}[] = {{
"""

    # 将压缩数据转换为十六进制数组 / Convert compressed data to hex array
    for i in range(0, len(compressed_data), 12):
        chunk = compressed_data[i:i+12]
        hex_values = ', '.join(f'0x{b:02X}' for b in chunk)
        header_content += f" {hex_values},\n"
    
    header_content += "};\n"
    
    # 写入输出文件 / Write to output file
    with open(output_file, 'w', encoding='utf-8') as f:
        f.write(header_content)
    
    print(f"成功生成头文件 / Successfully generated header file: {output_file}")

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("使用方法 / Usage: python compress_html.py input.html output.h")
        sys.exit(1)
    
    input_file = sys.argv[1]
    output_file = sys.argv[2]
    
    if not os.path.exists(input_file):
        print(f"错误 / Error: 输入文件不存在 / Input file does not exist: {input_file}")
        sys.exit(1)
    
    compress_to_header(input_file, output_file)
