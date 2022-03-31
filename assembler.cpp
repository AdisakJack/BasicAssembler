// Adisak Sangiamputtakoon
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <map>
#include <sstream>
#include <iomanip>

using namespace std;
void addToBytes(string binary, vector<string> &bytes);
void convertToBinary(unsigned int num, int binaryNum[32]);
string twosComplement(int n);
void convertToBinary(int num, int result[32]);
void printArray(int array[]);
int firstPass(string currentLine);
void stringSplitter(string currentLine, vector<string> &operands);
void secondPass(string currentLine, vector<string> &bytes, map<string, int> &varAddress);
int main(int argc, char **argv)
{

    map<string, int> varAddress;
    vector<string> bytes;
    vector<string> lines;
    if (argc != 2)
    {
        cerr << "Wrong number of arguments." << endl;
        return -1;
    }
    string fileName = argv[1];
    ifstream file(fileName);
    if (!file.good())
    {
        cerr << "Failed to open file." << endl;
        throw invalid_argument("Failed to open file.");
        return -1;
    }
    string line;
    while (getline(file, line))
    {
        lines.push_back(line);
    }
    file.close();
    int address = 0;
    //  string test = "    mov R2 $arr";
    // int testNum = firstPass(test);
    // cout << "Test is: " << testNum << endl;
    int length = lines.size();
    for (int i = 0; i < length; i++)
    {
        string currentLine = lines.at(i);
        if (currentLine.find("    ") == string::npos)
        {

            varAddress.insert(pair<string, int>(currentLine, address));
        }
        address += firstPass(currentLine);
    }

    for (int i = 0; i < length; i++)
    {
        string currentLine = lines.at(i);
        if (currentLine.find("    ") != string::npos)
        {
            secondPass(currentLine, bytes, varAddress);
        }
        address += firstPass(currentLine);
    }

    int size = bytes.size();
    for (int i = 0; i < size; i++)
    {
        if (i < 10 && size > 10)
        {
            cout << "Byte  " << i << ": ";
        }
        else
        {
            cout << "Byte " << i << ": ";
        }
        cout << setw(log10(size) + 1);

        cout << bytes.at(i) << endl;
    }
    map<string, int>::iterator itr;
    //     for (itr = varAddress.begin(); itr != varAddress.end(); ++itr)
    //     {
    //         cout << itr->first << "  " << itr->second << endl;
    //     }
}
void addToBytes(string binary, vector<string> &bytes)
{
    string byte;
    int length = binary.length();
    for (int i = 0; i < length; i++)
    {
        if (byte.length() == 8)
        {
            bytes.push_back(byte);
            byte = "";
        }
        byte += binary.at(i);
    }
    bytes.push_back(byte);
}

void secondPass(string currentLine, vector<string> &bytes, map<string, int> &varAddress)
{
    map<string, int>::iterator itr;

    int index = 0;
    vector<string> operands;
    stringSplitter(currentLine, operands);
    string binary = "";
    map<string, string> symbolTable;
    symbolTable.insert(pair<string, string>("R1", "000"));
    symbolTable.insert(pair<string, string>("R2", "001"));
    symbolTable.insert(pair<string, string>("R3", "010"));
    symbolTable.insert(pair<string, string>("R4", "011"));
    symbolTable.insert(pair<string, string>("R5", "100"));
    symbolTable.insert(pair<string, string>("R6", "101"));
    symbolTable.insert(pair<string, string>("RS", "110"));
    symbolTable.insert(pair<string, string>("RB", "111"));
    symbolTable.insert(pair<string, string>("mov", "0000"));
    symbolTable.insert(pair<string, string>("add", "0001"));
    symbolTable.insert(pair<string, string>("cmp", "0010"));
    symbolTable.insert(pair<string, string>("push", "0011"));
    symbolTable.insert(pair<string, string>("pop", "0100"));
    symbolTable.insert(pair<string, string>("call", "0101"));
    symbolTable.insert(pair<string, string>("je", "0110"));
    symbolTable.insert(pair<string, string>("jge", "0111"));
    symbolTable.insert(pair<string, string>("jl", "1000"));
    symbolTable.insert(pair<string, string>("j", "1001"));
    symbolTable.insert(pair<string, string>("ret", "1010"));
    symbolTable.insert(pair<string, string>("nop", "1011"));
    int size = operands.size();

    // cout << currentLine << endl;

    if (currentLine.find("var") != string::npos)
    {
        index = currentLine.find("var");
        string numString = currentLine.substr(index + 3);
        int value = stoi(numString);
        binary = twosComplement(value);
    }
    else if (size > 1 && currentLine.find("    ") != string::npos)
    {
        binary += symbolTable.find(operands.at(0))->second;

        if (operands.at(1).find("(") != string::npos)
        {
            binary += "11";
            index = operands.at(2).find("(");
            string numString = operands.at(1).substr(0, index);
            int value = stoi(numString);
            binary += twosComplement(value);
            string reg;
            reg = operands.at(1).substr(index + 1, 2);
            binary += symbolTable.find(reg)->second;
        }

            else if(operands.at(1).find("R") == 0){
                binary += "01";
                binary += symbolTable.find(operands.at(1))->second;
            }
            else if (operands.at(1).find("$") == 0)
            {
                binary += "00";

                string numString = operands.at(1).substr(1);
                itr = varAddress.find(numString);
                if (itr != varAddress.end())
                {
                    int value = varAddress.find(operands.at(1))->second;
                    binary += twosComplement(value);
                }
                else
                {
                    int value = stoi(numString);
                    binary += twosComplement(value);
                }
            }
            else
            {
                binary += "10";
                int value = varAddress.find(operands.at(1))->second;
                binary += twosComplement(value);
            }
            if (size == 3)
            {
                index += 2;
                if (operands.at(2).find("(") != string::npos)
                {
                    binary += "11";
                    index = operands.at(2).find("(");
                    string numString = operands.at(2).substr(0, index);
                    int value = stoi(numString);
                    binary += twosComplement(value);
                    string reg;
                    reg = operands.at(2).substr(index + 1, 2);
                    binary += symbolTable.find(reg)->second;
                }
                else if (operands.at(2).find("R") == 0)
                {
                    binary += "01";
                    binary += symbolTable.find(operands.at(2))->second;
                }
                else if (operands.at(2).find("$") == 0)
                {
                    binary += "00";
                    string numString = operands.at(2).substr(1);
                    itr = varAddress.find(numString);
                    if (itr != varAddress.end())
                    {
                        int value = varAddress.find(operands.at(2))->second;
                        binary += twosComplement(value);
                    }
                    else
                    {
                        int value = stoi(numString);
                        binary += twosComplement(value);
                    }
                }
                else
                {
                    binary += "10";
                    int value = varAddress.find(operands.at(2))->second;
                    binary += twosComplement(value);
                }
            }
            int length = binary.length();
            while (length % 8 != 0)
            {
                binary += "0";
                length = binary.length();
            }
            // addToBytes(binary, bytes);
        }
        else if (currentLine.find("    ") != string::npos && size == 1)
        {
            //    cout << "HITTTT" << endl;
            binary += symbolTable.find(operands.at(0))->second;
            //   cout << operands.at(0) << endl;
            //    cout << "Look up : " << symbolTable.find(operands.at(0))->second << endl;
            int length = binary.length();
            while (length < 8 && length % 8 != 0)
            {
                binary += "0";
                length = binary.length();
            }
        }

        // << binary << endl;
        addToBytes(binary, bytes);
    }
    int firstPass(string currentLine)
    {
        int index = 0;
        vector<string> operands;
        stringSplitter(currentLine, operands);
        int size = operands.size();
        if (currentLine.find("var") != string::npos)
        {
            return 4;
        }
        else if (size > 1 && currentLine.find("    ") != string::npos)
        {
            index += 6;
            if (operands.at(1).find("(") != string::npos)
            {
                index += 35;
            }
            else if (operands.at(1).find("R") == 0)
            {
                index += 3;
            }
            else
            {
                index += 32;
            }
            if (size == 3)
            {
                index += 2;
                if (operands.at(2).find("(") != string::npos)
                {
                    index += 35;
                }
                else if (operands.at(2).find("R") == 0)
                {
                    index += 3;
                }
                else
                {
                    index += 32;
                }
            }
            while (index % 8 != 0)
            {
                index += 1;
            }
            index = index / 8;

            return index;
        }
        else if (currentLine.find("    ") != string::npos && size == 1)
        {
            return 1;
        }
        return 0;
    }

    void stringSplitter(string currentLine, vector<string> & operands)
    {
        istringstream ss(currentLine);
        string word;
        while (ss >> word)
        {
            operands.push_back(word);
        }
    }
    void convertToBinary(int num, int result[32])
    {
        num = abs(num);
        int i = 0;
        while (num != 0)
        {
            result[i] = num % 2;
            num = num / 2;
            i++;
        }
    }

    string twosComplement(int n)
    {
        unsigned i;
        string result = "";
        for (i = 1 << 31; i > 0; i = i / 2)
        {
            if (n & i)
            {
                result += "1";
            }
            else
            {
                result += "0";
            }
        }
        return result;
    }

    void printArray(int array[])
    {
        for (int i = 0; i < 32; i++)
        {
            cout << array[i];
        }
        cout << endl;
    }
