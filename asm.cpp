// Authorship Declaration
// Harshvardhan Singh
// 2201CS92
// I declare that this asm.cpp is my original work.All sources used are properly cited, and any assistance received is acknowledged.This work has not been submitted elsewhere.

#include <iostream>
#include <bits/stdc++.h>
using namespace std;

// Function to trim leading and trailing spaces from a string
string trim(const string &str)
{
    size_t start = str.find_first_not_of(" \t");
    size_t end = str.find_last_not_of(" \t");
    return (start != string::npos && end != string::npos) ? str.substr(start, end - start + 1) : "";
}

// Struct to store processed lines
struct ProcessedLines
{
    vector<pair<string, int>> lines;
};

// Function to read and process ASM file
ProcessedLines readAndProcessAsmFile(const string &fileName)
{
    ProcessedLines result;
    ifstream file(fileName);

    if (!file.is_open())
    {
        cerr << "Error: Unable to open file " << fileName << endl;
        return result;
    }
    int linenumber = 0;
    string line;
    while (getline(file, line))
    {
        linenumber++;
        // Remove comments
        size_t commentPos = line.find(';');
        if (commentPos != string::npos)
        {
            line.erase(commentPos);
        }

        // Trim leading and trailing spaces
        line = trim(line);

        // Ignore blank lines
        if (!line.empty())
        {
            result.lines.push_back({line, linenumber});
        }
    }

    file.close();
    return result;
}

struct Instructionset // element for loookup table
{
    string mnemonic;
    int opcode;
    string operand;
};
vector<Instructionset> instructionSet = { // lookup table
    {"data", -1, "value"},                // assigned opcode value of 19 to data mnemonic
    {"ldc", 0, "value"},
    {"adc", 1, "value"},
    {"ldl", 2, "offset"},
    {"stl", 3, "offset"},
    {"ldnl", 4, "offset"},
    {"stnl", 5, "offset"},
    {"add", 6, ""},
    {"sub", 7, ""},
    {"shl", 8, ""},
    {"shr", 9, ""},
    {"adj", 10, "value"},
    {"a2sp", 11, ""},
    {"sp2a", 12, ""},
    {"call", 13, "offset"},
    {"return", 14, ""},
    {"brz", 15, "offset"},
    {"brlz", 16, "offset"},
    {"br", 17, "offset"},
    {"HALT", 18, ""},
    {"SET", -1, "value"}}; //-1 for no opcode

bool label_checker(string a, const vector<pair<string, int>> &labels) // duplicate label  checker function
{
    for (const auto &label : labels)
    {
        if (a == label.first)
            return true;
    }
    return false;
}

bool mnemonic_checker(string a, const vector<Instructionset> &instructionSet) // mnemonic checker function
{
    for (const auto &instruction : instructionSet)
    {
        if (a == instruction.mnemonic)
            return true;
    }
    return false;
}

int getNumberBase(const char *str) // return 0 for invalid and 8,10,16 for oct,dec,hex
{
    char *endptr;

    // Check for empty string
    if (*str == '\0')
    {
        return 0; // Invalid
    }

    // Determine the base of the number (detecting '0x' or '0' prefixes)
    if (str[0] == '0')
    {
        if (str[1] == 'x' || str[1] == 'X')
        {
            // Hexadecimal
            strtol(str, &endptr, 16);
            return (*endptr == '\0') ? 16 : 0;
        }
        else
        {
            // Octal
            strtol(str, &endptr, 8);
            return (*endptr == '\0') ? 8 : 0;
        }
    }

    // Decimal
    strtol(str, &endptr, 10);
    return (*endptr == '\0') ? 10 : 0;
}

// error flags and containers
int flag = 0; // no error
vector<string> errors;
vector<pair<string, int>> labels;                // storing labels
vector<pair<string, string>> BufferInstructions; // to store raw instruction

// FIRST PASS
void FirstPass(ProcessedLines &processedLines)
{
    // FIRST PASS

    for (const auto &line : processedLines.lines)
    {
        int labelpos = line.first.find(':');

        if (labelpos != string::npos) // line has label
        {
            string label = line.first.substr(0, labelpos);

            if (isalpha(label[0])) // check for bogus labelname
            {
                if (label_checker(label, labels)) // error check for duplicate label
                {
                    flag = 1;
                    string error = " error at line " + to_string(line.second) + " - " + label + " - " + "Duplicate Label";
                    errors.push_back(error);
                }
                else
                {
                    labels.push_back({label, BufferInstructions.size()});
                }
            }

            else
            {
                flag = 1;
                string error = " error at line " + to_string(line.second) + " - " + label + " - " + "Bogus label name";
                errors.push_back(error); // error for bogus labelname
            }

            if (labelpos != (line.first.length() - 1)) // if instruction is just right of label and push it
            {
                // implementing SET instruction
                string instruct = trim(line.first.substr(labelpos + 1));
                int middlepos = instruct.find(' ');
                if (instruct.substr(0, middlepos) == "SET")
                {
                    string num = trim(instruct.substr(middlepos));
                    int number = getNumberBase(num.c_str()); // return base else return 0 for invalid number
                    if (number > 0)
                    {
                        number = strtol(num.c_str(), nullptr, number);
                        labels[labels.size() - 1].second = number; // changing label from pc to specified value
                    }
                    else
                    {
                        flag = 1;
                        string error = " error at line " + to_string(line.second) + " - " + line.first + " - " + "Invalid number for set instruction";
                        errors.push_back(error); // error for invalid number
                        break;
                    }
                }
                // else normal instruction
                else
                    BufferInstructions.push_back({instruct, to_string(line.second)});
            }
        }

        else // line dont have label so push it
        {
            BufferInstructions.push_back({line.first, to_string(line.second)});
        }
    }
    // first pass end;
}

// SECOND PASS
void SecondPass(string filename)
{

    // second pass
    int index = 0;                                        // keep track of instruction adress
    ofstream outputFile1(filename + ".obj", ios::binary); // created bin file
    ofstream outputFile2(filename + ".lst");              // created listing file
    int labelcnt = 0;                                     // label count (required for desired printing of listing file)

    // Check if both files are successfully opened
    if (!outputFile1.is_open() || !outputFile2.is_open())
    {
        cerr << "Unable to open one or both of the files for writing.\n";
        return;
    }

    // iterating line starts
    for (const auto &line : BufferInstructions)
    {
        pair<string, string> instruction; // breaking instruction into opcode and address/operand
        int spacepos = line.first.find(' ');
        if (spacepos == string::npos)
            instruction = {line.first, ""};
        else
            instruction = {line.first.substr(0, spacepos), trim(line.first.substr(spacepos))};
        int operand; // store the operand/address of that instruction

        // taking out the instruction format of that instrcution
        Instructionset instruction_format;

        // checking error in first part of instruction (opcode)
        if (isalpha(instruction.first[0]))
        {
            if (mnemonic_checker(instruction.first, instructionSet))
            {
                // now mnemonic is checked taking out the instruction format of that instrcution
                for (const auto &i : instructionSet)
                {
                    if (instruction.first == i.mnemonic)
                    {
                        instruction_format = i;
                        break;
                    }
                }

                // Operand/address checking starts
                if (instruction_format.operand == "") // no operand is expected
                {
                    if (instruction.second != "")
                    {
                        flag = 1;
                        string error = " error at line " + line.second + " - " + line.first + " - " + " Unexpected oprerand , check instruction format for " + instruction.first;
                        errors.push_back(error); // error for unexpected operand
                        break;
                    }
                }
                else // operand expected
                {
                    if (instruction.second == "")
                    {
                        flag = 1;
                        string error = " error at line " + line.second + " - " + line.first + " - " + " missing operand ";
                        errors.push_back(error); // error for missing operand
                        break;
                    }

                    else
                    {
                        if (isalpha(instruction.second[0])) // check for label if exist
                        {
                            if (label_checker(instruction.second, labels)) // label exist then set label to its value/offset
                            {
                                int offset;
                                for (const auto &label : labels)
                                {
                                    if (instruction.second == label.first)
                                    {
                                        offset = label.second - index - 1;
                                        break;
                                    }
                                }
                                if (instruction_format.operand == "offset") // storing the offset or absolute adress in place of label
                                    operand = offset;
                                else
                                    operand = (offset + index + 1);
                            }
                            else // label not exist
                            {
                                flag = 1;
                                string error = " error at line " + line.second + " - " + line.first + " - " + " NO such label ";
                                errors.push_back(error); // error for missing label
                                break;
                            }
                        }
                        else // check for  validity of number
                        {
                            if (getNumberBase(instruction.second.c_str()) == 0) // c_str to convert from string to char pointer
                            {
                                flag = 1;
                                string error = " error at line " + line.second + " - " + line.first + " - " + " NOT a valid number( also only one operand is expected) ";
                                errors.push_back(error); // error for missing label
                                break;
                            }
                            else
                                operand = strtol(instruction.second.c_str(), nullptr, getNumberBase(instruction.second.c_str())); // set operand to its value
                        }
                    }
                }
            }
            else // mnemonic not found
            {
                flag = 1;
                string error = " error at line " + line.second + " - " + line.first + " - " + "Invalid Mnemonic";
                errors.push_back(error); // error for invalid mnemonic
                break;
            }
        }
        else // bogus mnemonic name
        {
            flag = 1;
            string error = " error at line " + line.second + " - " + line.first + " - " + "Bogus mnemonic";
            errors.push_back(error); // error for bogus labelname
            break;
        }

        // PRINTING FILE ...
        // PRINTING FILE ....

        if (index == labels[labelcnt].second) // printing label at that address
        {
            outputFile2 << hex << setw(8) << setfill('0') << index;
            outputFile2 << "            " << labels[labelcnt].first << ":\n";
            labelcnt++; // increase the label index
        }

        outputFile2 << hex << setw(8) << setfill('0') << index << "  "; // instruction address

        if (instruction_format.mnemonic == "data") // data allocating instruction
        {
            outputFile1.write(reinterpret_cast<const char *>(&operand), 4);
            outputFile2 << hex << setw(8) << setfill('0') << operand; // writing data
        }
        else // normal instruction
        {
            stringstream stream;
            if (instruction_format.operand == "")
                stream << "000000"; // no operand exected so put 0
            else
            {
                if (operand >= 0)
                    stream << hex << setw(6) << setfill('0') << operand; // writing address/operand 3 byte
                else
                {
                    stream << hex << setw(6) << setfill('0') << 16777216 - abs(operand); // 2^24-absolute( for negative number);
                }
            }
            stream << hex << setw(2) << setfill('0') << instruction_format.opcode; // now complete instruction in hex is written in stream
            string abc = stream.str();
            outputFile2 << abc; // writing in listing file
            int stringtohex = stoll(abc, nullptr, 16);
            outputFile1.write(reinterpret_cast<const char *>(&stringtohex), 4); // writing instruction into object file
        }
        outputFile2 << "  " << line.first << "\n";

        index++; // index to keep track of instruction address
    }

    // before closing file  we add the remaining labels to our listing files
    auto compareSecond = [](const auto &a, const auto &b)
    {
        return a.second < b.second;
    };
    sort(labels.begin(), labels.end(), compareSecond);
    for (const auto &a : labels)
    {
        if (a.second >= index)
        {
            outputFile2 << hex << setw(8) << setfill('0') << a.second;
            outputFile2 << "            " << a.first << ":\n";
        }
    }
    // Close both file streams
    outputFile1.close();
    outputFile2.close();
}

int main(int argc, char *argv[])
{
    // Check if the correct number of command-line arguments is provided
    if (argc != 2)
    {
        cerr << "Usage: " << argv[0] << " <filename>" << endl;
        return 1; // Exit with an error code
    }

    // Retrieve the filename from the command-line argument
    string fileName = argv[1];
    int endpt = fileName.find('.');
    string file = fileName.substr(0, endpt);
    if (fileName.substr(endpt + 1) != "asm")
    {
        cerr << "Invalid file provided \n file should be of .asm type";
        return 1; // Exit with an error code
    }

    // Read and process ASM file
    ProcessedLines processedLines = readAndProcessAsmFile(fileName);

    // first pass
    FirstPass(processedLines);

    // second pass if no errors in first
    if (!flag)
        SecondPass(file);

    // Writing LOGfile
    ofstream LOGFILE(file + ".log");
    if (!flag)
        LOGFILE << "Assembled Successfully";
    else
        LOGFILE << " Assembling Stopped with errors\n";
    for (const auto &a : errors)
        LOGFILE << a << "\n";
    LOGFILE.close();

    // general instructions-
    cout << "Listing file in " << file << ".lst\n";
    cout << "Object file in " << file << ".obj\n";
    cout << "Logfile in " << file << ".log\n";
    return 0;
}
