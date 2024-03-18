// Authorship Declaration
// Harshvardhan Singh
// 2201CS92
// I declare that this asm.cpp is my original work.All sources used are properly cited, and any assistance received is acknowledged.This work has not been submitted elsewhere.
#include <bits/stdc++.h>
using namespace std;
int Memory[1 << 13]; // It is the main memory of code 2^13 ~= 8000 word space is enough
vector<string> code; // This stores the code
int A = 0, B = 0, SP = 0, PC = 0;

void ldc(int value)
{ // Functions to perform as per Mnemonic
    // Load accumulator with the value specified
    B = A;
    A = value;
}
void adc(int value)
{
    A += value;
}
void ldl(int offset)
{
    B = A;
    A = Memory[SP + offset];
}
void stl(int offset)
{
    Memory[SP + offset] = A;
    A = B;
}
void ldnl(int offset)
{
    A = Memory[A + offset];
}
void stnl(int offset)
{
    Memory[A + offset] = B;
}
void add()
{
    A += B;
}
void sub()
{
    A = B - A;
}
void shl()
{
    A = B << A;
}
void shr()
{
    A = B >> A;
}
void adj(int value)
{
    SP = SP + value;
}
void a2sp()
{
    SP = A;
    A = B;
}
void sp2a()
{
    B = A;
    A = SP;
}
void call(int offset)
{
    B = A;
    A = PC;
    PC += offset;
}
void ret()
{
    PC = A;
    A = B;
}
void brz(int offset)
{
    if (A == 0)
        PC = PC + offset;
}
void brlz(int offset)
{
    if (A < 0)
        PC = PC + offset;
}
void br(int offset)
{
    PC = PC + offset;
}

string retrievename(string a)
{
    int endpt = a.find('.');
    return a.substr(0, endpt);
}

int signedhextoint(string a)
{
    int b = stoi(a, nullptr, 16);
    if (b >= 8388608)         // if number >= 2^(24-1) ie.max signed limit
        b = -((1 << 24) - b); // then b will be negative;
    return b;
}

string decToHex(unsigned int value) // inputs a int and convert it into string of hex of 8 characters
{
    stringstream stream;
    stream << hex << setw(8) << setfill('0') << value;
    return stream.str();
}

void Open_Read_File(int argc, char *argv[])
{
    // Check if the correct number of command-line arguments is provided
    if (argc != 2)
    {
        cerr << "Usage: " << argv[0] << " <filename>.obj" << endl;
        return; // Exit with an error code
    }
    // Retrieve the filename from the command-line argument
    string fileName = argv[1];
    int endpt = fileName.find('.');

    if (fileName.substr(endpt + 1) != "obj")
    {
        cerr << "Invalid file provided \n file should be of .obj type";
        return;
    }
    // Open the binary file in input mode
    ifstream Objfile(fileName, ios::binary);

    if (!Objfile.is_open())
    {
        cerr << "Error opening the file!" << endl;
        return;
    }

    // Now reading and writing to memory
    int read;
    int pos = 0;
    while (Objfile.read((char *)&read, 4)) // reading 4 bytes chunks
    {
        Memory[pos++] = read;
        code.push_back(decToHex(read));
    }
}

void RunEmu(string filename)
{
    filename = retrievename(filename);
    ofstream LOGFILE(filename + "_emu.log");
    int flag = 0, errflag = 0; // flags for error and halts
    while (PC < code.size())   // if Pc goes beyond code (ie in memory area then breaks)
    {
        string instruct = code[PC++];                          // increment pc as well
        int opcode = stoi(instruct.substr(6, 2), nullptr, 16); // opcode extract
        int operand = signedhextoint(instruct.substr(0, 6));   // oerand/address extract
        try
        { // enclosing withing try catch for errant programs
            switch (opcode)
            {
            case 0:
                ldc(operand);
                break;
            case 1:
                adc(operand);
                break;
            case 2:
                ldl(operand);
                break;
            case 3:
                stl(operand);
                break;
            case 4:
                ldnl(operand);
                break;
            case 5:
                stnl(operand);
                break;
            case 6:
                add();
                break;
            case 7:
                sub();
                break;
            case 8:
                shl();
                break;
            case 9:
                shr();
                break;
            case 10:
                adj(operand);
                break;
            case 11:
                a2sp();
                break;
            case 12:
                sp2a();
                break;
            case 13:
                call(operand);
                break;
            case 14:
                ret();
                break;
            case 15:
                brz(operand);
                break;
            case 16:
                brlz(operand);
                break;
            case 17:
                br(operand);
                break;
            case 18:
                flag = 1;
            default:
                flag = 1;
            }
        }
        catch (const exception &e) // catch error for errant programs
        {
            LOGFILE << "Error: " << e.what() << endl;
            errflag = 1;
        }

        if (flag || errflag)
            break; // to break loop ie halt occured or error occured
    }
    if (!errflag)
        LOGFILE << "Program executed Successfully";
    LOGFILE.close();
}

void memdump(string filename)
{
    filename = retrievename(filename);
    ofstream memdump(filename + "_emu_memdump.lst");

    for (int i = 0; i < (1 << 13); i++)
    {
        memdump << hex << setw(8) << setfill('0') << i << "   ";
        memdump << hex << setw(8) << setfill('0') << Memory[i] << "\n";
    }
    memdump.close();
}
int main(int argc, char *argv[])
{
    Open_Read_File(argc, argv); // Opens file and read it into memory and code arrays

    RunEmu(argv[1]);

    memdump(argv[1]);

    cout << "memdump in " << retrievename(argv[1]) << "_emu_memdump.lst\n";
    cout << "running log in " << retrievename(argv[1]) << "_emu.log";
    return 0;
}