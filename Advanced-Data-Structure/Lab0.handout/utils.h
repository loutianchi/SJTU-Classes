 #ifndef UTILS_H
#define UTILS_H

#include <cstring>
#include <sstream>
#include <bitset>
#include <fstream>
#include <iostream>
#include <cassert>

namespace utils
{
    std::string getFileName(const std::string &fileName, const std::string &suffix)
    {
        size_t final_pos = fileName.find_last_of("/\\");
        size_t suffix_pos = fileName.find_last_of('.');
        return final_pos == std::string::npos ? "./output/" + fileName.substr(0, suffix_pos) + suffix : "./output/" + fileName.substr(final_pos + 1, suffix_pos - final_pos - 1) + suffix;
    }

    std::string parseText(const std::string &input)
    {
        std::ifstream in(input);
        if (!in.is_open()) {
            puts("File name is wrong");
            return "";
        }
        std::string content;
        content = "";
        char c;
        while (in.get(c)) {
            content += c;
        }
        in.close();
        std::cout << "input length: " << content.length() * 8 << std::endl;
        return content;
    }

    void output(const std::string &output, const std::string &data)
    {
        std::ofstream out(output);
        out << data;
        out.close();        
    }

    std::string codingTable2String(const std::map<std::string, std::string> &codingTable)
    {
        std::string result = "";
        for (auto it = codingTable.begin(); it != codingTable.end(); ++it) {
            result += it->first + " " + it->second + "\n";
        }
        return result;
    }

    void loadCodingTable(const std::string &input, std::map<std::string, std::string> &codingTable)
    {
        std::ifstream in(input);
        if (!in.is_open()) {
            puts("File name is wrong");
            return;
        }
        std::string content;
        content = "";
        char c;
        while (in.get(c)) {
            content += c;
        }
        in.close();

        std::string ch, code;
        int pt = 0;
        while (pt < content.length()) {
            code = "";
            ch = content.substr(pt, 1); pt++;
            if (content.substr(pt, 2) != " 0" && content.substr(pt, 2) != " 1") {
                ch = ch + content[pt];
                pt++;
            }
            pt++;
            while (content[pt] == '0' || content[pt] == '1') {
                code = code + content[pt];
                pt++;
            }
            pt++;
            codingTable[ch] = code;
        }
    }

    std::string compress
        (const std::map<std::string, std::string> &codingTable, const std::string &text)
    {
        std::string result = "";

        char c = (char) 0;
        unsigned long long len = 0;
        std::string ch, code;
        
        // 遍历整个字符串
        for (int i = 0; i < text.length(); i++) {
            // 首先判断是否是属于multichar的情况
            if (i < text.length() - 1) {
                ch = text.substr(i, 2);
                i++;
            }
            else
                ch = text.substr(i, 1);
            // 若不是multichar的情况，则取一个字符（一定满足）
            if (codingTable.find(ch) == codingTable.end()) {
                i--;
                ch = text.substr(i,1);
            }
            auto it = codingTable.find(ch);
            // 如果发现字符（或字符组合）不存在，则报错。
            // 但是由于codingTable是为当前文本生成的，这种情况不应该存在）
            assert(it != codingTable.end());
            if (it != codingTable.end())
                code = it->second;
            for (int k = 0; k < code.length(); k++) {
                len++;
                // 按位插入到后面
                c = (c << 1) | (code[k]=='0'?0:1);
                if (len % 8 == 0) {
                    result += c;
                    c = (char) 0;
                }
            }
        }
        // 在结尾补全 8 位并转为字符
        if (len % 8 != 0) {
           c = (c << (8 - len % 8));
           result += c;
        }
        
        std::cout<< "tot length: " << len << std::endl;
        // 生成总字符数的小端表示，并加到结果的前面
        std::string lengthLittleEndian = "";
        for (int i = 1; i <= 8; i++) {
            c = (char) len & 0xFF;
            lengthLittleEndian = lengthLittleEndian + c;
            len >>= 8;
        }
        result = lengthLittleEndian + result;
        return result;
    }
};

#endif