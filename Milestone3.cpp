// The encryption program in C++ and ASM with a very simple encryption method - it simply adds 1 to the character.
// The encryption method is written in ASM. You will replace this with your allocated version for the assignment.
// In this version parameters are passed via registers (see 'encrypt' for details).
// Filename: "4473 FoCA Assignment 2 2017 - Encryption Original with ASM.cpp"
// Last revised Feb 2017 by A.Oram

char EKey = 'u';			// Replace x with your Encryption key.

#define StudentName "Liberty Ware"

//  *** PLEASE CHANGE THE NAME IN QUOTES ABOVE TO YOUR NAME  ***
//  *** KEEP ALL COMMENTS UP-TO-DATE. COMMENT USEFULLY ALL CODE YOU PRODUCE. ***

#define MAXCHARS 10		// feel free to alter this, but 6 is the minimum

using namespace std;
#include <conio.h>		// for kbhit
#include <windows.h>
#include <string>       // for strings
#include <fstream>		// file I/O
#include <iostream>		// for cin >> and cout <<
#include <iomanip>		// for fancy output
#include "TimeUtils.h"  // for GetTime, GetDate, etc.

#define dollarchar '$'  // string terminator

char OChars[MAXCHARS],
EChars[MAXCHARS],
DChars[MAXCHARS];	// Global Original, Encrypted, Decrypted character strings

//----------------------------- C++ Functions ----------------------------------------------------------

void get_char(char& a_character)
{
	cin >> a_character;
	while (((a_character < '0') | (a_character > 'z')) && (a_character != dollarchar))
	{
		cout << "Alphanumeric characters only, please try again > ";
		cin >> a_character;
	}
}
//-------------------------------------------------------------------------------------------------------------

void get_original_chars(int& length)
{
	char next_char;
	length = 0;
	get_char(next_char);

	while ((length < MAXCHARS) && (next_char != dollarchar))
	{
		OChars[length++] = next_char;
		get_char(next_char);
	}
}

//---------------------------------------------------------------------------------------------------------------
//----------------- ENCRYPTION ROUTINES -------------------------------------------------------------------------

void encrypt_chars(int length, char EKey)
{
	char temp_char;						// Character temporary store

	for (int i = 0; i < length; i++)	// Encrypt characters one at a time
	{
		temp_char = OChars[i];			// Get the next char from Original Chars array

		__asm {
			push   eax					// save the value of EAX on the stack
				push   ecx					// save the value of ECX on the stack
				movzx  ecx, temp_char		// store the temp character in the ECX register
				lea    eax, EKey				// store encryption key's effective address in EAX
				push ecx					// push the temp character to the stack as a parameter
				push eax					// push the EKey's effective address to the stack as a parameter
				call   encrypt_20			// encrypt the character using routine 20
				add esp, 8					// clear the stack
				mov    temp_char, al			// move encrypted character (stored in the last 8 bits of EAX) to 'temp_char'

				pop    ecx					// put the original ECX value back
				pop    eax					// put the original EAX value back
		}
		EChars[i] = temp_char;			// Store encrypted char in the Encrypted Chars array
	}
	return;

	__asm {

	encrypt_20:
		push ebp					// save the initial address of the base pointer
			mov ebp, esp				// move the base pointer to the stack pointer's location
			mov eax, [ebp + 8]			// move a copy of the EKey's effective address to EAX
			mov ecx, [ebp + 12]			// move a copy of the temp character to ECX

			push[eax]					// save the EKey to the stack
			and dword ptr[eax], 0x3B	// use a bitwise 'and' operation on the EKey with the ';' character						
			not byte ptr[eax]			// Invert the new EKey's hex value																
			mov edx, [eax]				// save the mangled EKey to EDX
			pop eax						// put the original EKey value back in EAX		
			ror al, 1					// move the bits of the EKey right														
			and eax, 0x5F				// 'and' the result of the last operation with the '_' character								
			xor ecx, edx				// use a 'xor' bitwise operation on the temp character using the first mangled version of the Ekey	
			xor ecx, eax				// 'xor' the result of that operation with the second mangled version							
			rol cl, 2					// the bits of the temp character are rotated left twice																	
			mov eax, ecx				// then the result is saved to the EAX register
			sub al, 0x20				// then the hex value of the space character is subtracted form the hex value of the temp character

			pop ebp						// put the base pointer back where it was originally
			ret							// the result is passed back to the calling function
	}

	//--- End of Assembly code
}
// end of encrypt_chars function
//---------------------------------------------------------------------------------------------------------------




//---------------------------------------------------------------------------------------------------------------
//----------------- DECRYPTION ROUTINES -------------------------------------------------------------------------
//
void decrypt_chars(int length, char EKey)
{


	char temp_char;							// Character temporary store

	for (int i = 0; i < length; i++)		// Decrypt characters one at a time
	{
		temp_char = EChars[i];				// Get the next char from Original Chars array

		__asm {
			push   eax					// save the value of EAX on the stack
				push   ecx					// save the value of ECX on the stack
				movzx  ecx, temp_char		// store the temp character in the ECX register
				lea    eax, EKey			// store encryption key's effective address in EAX
				push ecx					// push the temp character to the stack as a parameter
				push eax					// push the EKey's effective address to the stack as a parameter
				call	decrypt_20			// decrypt the character
				add esp, 8					// clear the stack
				mov		temp_char, al		// move encrypted character (stored in the last 8 bits of EAX) to 'temp_char'
				// ---conversion to lower case---
				cmp temp_char, 41h			// if temp_char < A
				jl skipConversion			// skip conversion
				cmp temp_char, 5Ah			// if temp_char > Z
				jg skipConversion			// skip conversion
				add temp_char, 20h			// else add 32 to temp_char to convert to lower case

			skipConversion:
			pop    ecx					// put the original ECX value back
				pop    eax					// put the original EAX value back
		}
		DChars[i] = temp_char;				// Store encrypted char in the Encrypted Chars array
	}
	return;

	__asm {

	decrypt_20:


		push ebp					// save the initial address of the base pointer
			mov ebp, esp				// move the base pointer to the stack pointer's location
			mov eax, [ebp + 8]			// move a copy of the EKey's effective address to EAX
			mov ecx, [ebp + 12]			// move a copy of the temp character to ECX
			// ---get the mangled characters---
			push[eax]					// save the EKey to the stack
			and dword ptr[eax], 0x3B	// use a bitwise 'and' operation on the EKey with the ';' character						
			not byte ptr[eax]			// Invert the new EKey's hex value																
			mov edx, [eax]				// save the mangled EKey to EDX
			pop eax						// put the original EKey value back in EAX		
			ror al, 1					// move the bits of the EKey right														
			and eax, 0x5F				// 'and' the result of the last operation with the '_' character
			// ---reverse the temp char encryption---
			add ecx, 0x20				// add the hex value of the space character to the hex value of the temp character
			ror cl, 1					// the bits of the temp char are rotated right
			ror cl, 1					// twice
			xor ecx, eax				// 'xor' the temp char with the second mangled EKey
			xor ecx, edx				// 'xor' the temp char with the first mangled EKey
			mov eax, ecx				// save the decrypted character to the EAX register

			pop ebp						// put the base pointer back where it was originally
			ret							// the result is passed back to the calling function
	}

}
// end of decrypt_chars function
//---------------------------------------------------------------------------------------------------------------


int main(void)
{
	int char_count(0);  // The number of actual characters entered (upto MAXCHARS limit).

	cout << "\nPlease enter upto " << MAXCHARS << " alphanumeric characters:  ";
	get_original_chars(char_count);

	ofstream EDump;
	EDump.open("EncryptDump.txt", ios::app);
	EDump << "\n\nFoCA Encryption program results (" << StudentName << ") Encryption key = '" << EKey << "'";
	EDump << "\nDate: " << GetDate() << "  Time: " << GetTime();

	// Display and save initial string
	cout << "\n\nOriginal string =  " << OChars << "\tHex = ";
	EDump << "\n\nOriginal string =  " << OChars << "\tHex = ";
	for (int i = 0; i < char_count; i++)
	{
		cout << hex << setw(2) << setfill('0') << ((int(OChars[i])) & 0xFF) << "  ";
		EDump << hex << setw(2) << setfill('0') << ((int(OChars[i])) & 0xFF) << "  ";
	};

	//*****************************************************
	// Encrypt the string and display/save the result
	encrypt_chars(char_count, EKey);

	cout << "\n\nEncrypted string = " << EChars << "\tHex = ";
	EDump << "\n\nEncrypted string = " << EChars << "\tHex = ";
	for (int i = 0; i < char_count; i++)
	{
		cout << ((int(EChars[i])) & 0xFF) << "  ";
		EDump << ((int(EChars[i])) & 0xFF) << "  ";
	}

	//*****************************************************
	// Decrypt the encrypted string and display/save the result
	decrypt_chars(char_count, EKey);

	cout << "\n\nDecrypted string = " << DChars << "\tHex = ";
	EDump << "\n\nDecrypted string = " << DChars << "\tHex = ";
	for (int i = 0; i < char_count; i++)
	{
		cout << ((int(DChars[i])) & 0xFF) << "  ";
		EDump << ((int(DChars[i])) & 0xFF) << "  ";
	}
	//*****************************************************

	cout << "\n\n\n";
	EDump << "\n\n-------------------------------------------------------------";
	EDump.close();
	system("PAUSE");
	return (0);


} // end of whole encryption/decryption program --------------------------------------------------------------------


