#include "StringPage.h"

void StringPage::FitToPageLen(unsigned int Len)						//fit to page of 'n' width
{
        std::vector<std::string>::iterator Previous = CurLine;
        for(std::vector<std::string>::iterator i = StringArray.begin(); i != StringArray.end(); i++)
        {
                //per line
                if((*i).length() > Len)
                {
                        std::string TempString = (*i).substr(Len);
                        (*i).resize(Len);
                        CurLine = i;
                        AddLine(TempString);
                }
        }
        CheckSize();
        CurLine = Previous;
}
void StringPage::DelLine()
{
        StringArray.erase(CurLine);
        CheckSize();
}
void StringPage::DelLine(unsigned int line)								//remove specific line
{
        std::vector<std::string>::iterator i= StringArray.begin() + line;
        if(i <= StringArray.end())
        {
                StringArray.erase(i);
                CheckSize();
        }
        else 
        {
//			ERRSYS.Log.Write(20,"StringPage Bounds Check failed!: Last line was:", &(*StringArray.end())[0]);
        }
}
void StringPage::DelLineChar(char KillChar)
{//remove lines starting with passed char
        for(std::vector<std::string>::iterator i = StringArray.begin(); i <= StringArray.end(); i++)
        {
                //per line
                unsigned int count =0;
                while(count < (*i).length() && (*i)[count]==' ')
                {
                        count++;
                        //per char
                        if((*i)[count]==KillChar)
                        {
                                StringArray.erase(i);
                                break;
                        }
                }
        }
        CheckSize();
}
StringPage::StringPage()
{
        CurLine = StringArray.begin();
        SheetLen = 0;
        CheckEOF = false;
}
StringPage::StringPage(std::string Lines)
{
        CheckEOF = false;
        AddLines(Lines);
        SheetLen = StringArray.size();
        CurLine = StringArray.begin();
}
void StringPage::PreAlloc(unsigned int LineLen, unsigned int NumLines)
{
        StringArray.resize(NumLines);
        for(unsigned int i=0; i < StringArray.size(); i++)
        {
                StringArray.at(i).reserve(LineLen);
        }
        return;
}
std::string* StringPage::ReadStr()
{
        if(CurLine <= StringArray.end())
        {
                std::string* RetVal = &(*CurLine);
                CurLine++;
                EOFCheck();
                return RetVal;
        }
        else 
        {
//			ERRSYS.Log.Write(20,"StringPage Bounds Check failed!: Last line was:", &(*StringArray.end())[0]);
                return NULL;
        }

}
std::string* StringPage::ReadStr(bool Hold)
{
        if(CurLine < StringArray.end())
        {
                std::string* RetVal = &(*CurLine);

                if(!Hold)
                {
                        CurLine++;
                        EOFCheck();
                }

                return RetVal;
        }
        EOFCheck();
        return NULL;
}
unsigned int StringPage::GetLen()
{
        return SheetLen;
}
void StringPage::GoTo(unsigned int Line)
{
        if(Line < SheetLen)
                CurLine = StringArray.begin() + Line;
        EOFCheck();
        return;
}
void StringPage::Rewind()
{
        CurLine = StringArray.begin();
        EOFCheck();
        return;
}
void StringPage::NextLine()
{
        CurLine++;
        EOFCheck();
        return;
}
void StringPage::EOFCheck()
{
        if(CurLine > StringArray.end())
        {
                CurLine = StringArray.end();
                CheckEOF = true;
        }
        else
        {
                CheckEOF = false;
        }
        return;
}
void StringPage::AddLine(std::string AddLine)
{
        StringArray.insert(CurLine+1,AddLine);
        CurLine++;
        CheckSize();
}
void  StringPage::AddLines(std::string New)
{
        size_t found;
        //remove ending \n if appropriate
        found = New.find_last_of('\n');
        if(found >= New.length()-1)
        {
                New[found]='\0';
        }

        found=New.find_first_of('\n');
        while (found!=std::string::npos)
        {
                found=New.find_first_of('\n');
                AddLine(New.substr(0,found)); 
                New = New.substr(found+1);
        }
        AddLine(New);
        CheckSize();
        return;
}
bool StringPage::QueryEOF()
{
        return CheckEOF;
}
void StringPage::CheckSize()
{
        SheetLen = StringArray.size();
        EOFCheck();
}
unsigned int StringPage::CurrentLine()
{
        //return SheetLen - StringArray.begin;
        std::vector<std::string>::iterator Start = StringArray.begin();
        return std::distance(CurLine,Start);
}