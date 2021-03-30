#ifndef StringPage_H
#define StringPage_H

#include <string>
#include <vector>
//#include "ErrorSystem_s.h"
/*	The following is the 'StringPage' definition, effectively just an array of strings.
	
	These should be used to:
	Hold pages of text-information
	load ascii-files

	Thse should NOT be used to:
	generate output streams
	store non-text data. 
*/
class StringPage
{
public:
        StringPage();												//default constructor
        StringPage(std::string);									//String constructor (newlines split strings)
        void PreAlloc(unsigned int LineLen, unsigned int NumLines);	//pre-allocate sheet for set quantities
        std::string* ReadStr();										//read a string (default, moves ahead)
        std::string* ReadStr(bool Hold);							//read a string (option to hold)
        unsigned int GetLen();										//get length of sheet
        void GoTo(unsigned int);									//goto a specific line
        void Rewind();												//rewind to the first line
        void NextLine();											//go to the next line
        void AddLine(std::string);									//add the line at the current position (moves ahead)
        void AddLines(std::string);									//add the line at the current position (newlines split strings)(moves ahead)
        bool QueryEOF();											//query if the end of file has been reached
        void FitToPageLen(unsigned int);							//fit to page of 'n' width
        void DelLine(unsigned int);									//remove specific line
        void DelLine();												//remove the current line
        void DelLineChar(char);										//remove lines starting with passed char
        unsigned int CurrentLine();									//returns the current line number
protected:
        void EOFCheck();
        void CheckSize();
private:
        bool CheckEOF;
        std::vector<std::string> StringArray;				//array of strings
        std::vector<std::string>::iterator CurLine;			//current line
        unsigned int SheetLen;								//number of lines in this sheet
};
#endif