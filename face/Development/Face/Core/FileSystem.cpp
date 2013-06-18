#pragma warning(disable : 4995 4244)

#include <locale>
#include <iostream>
#include <string>
#include <sstream>

#include <windows.h>
#include <stdio.h>
#include <tchar.h> 
#include <strsafe.h>
#pragma comment(lib, "User32.lib")

#include "FileSystem.h"
#include "Tracer.h"

using namespace std;


FileSystem::FileSystem(void)
{
}

FileSystem::~FileSystem(void)
{
}


void FileSystem::MakeDirectory(const string& directory)
{
	wstring wDir = StringToWstring(directory);
	CreateDirectory(wDir.c_str(), NULL);
}


bool FileSystem::FileExists(const std::string& filePath)
{
	wstring wDir = StringToWstring(filePath);
	DWORD attr = GetFileAttributes(wDir.c_str());
	return 0xffffffff != attr && ((attr & FILE_ATTRIBUTE_DIRECTORY) == false);
}


bool FileSystem::DirectoryExists(const string& directory)
{
	wstring wDir = StringToWstring(directory);
	DWORD attr = GetFileAttributes(wDir.c_str());
	return 0xffffffff != attr && (attr & FILE_ATTRIBUTE_DIRECTORY);
}


vector<string> FileSystem::GetFiles(const string& directory)
{
	return GetDirectoryContent(directory, true);
}


vector<string> FileSystem::GetSubdirectories(const string& directory)
{
	return GetDirectoryContent(directory, false);
}


vector<string> FileSystem::GetDirectoryContent(const string& directory, bool files)
{
	vector<string> list;

	if (DirectoryExists(directory) == false)
		return list;
	
	
	WIN32_FIND_DATA ffd;
	TCHAR szDir[MAX_PATH];
	size_t length_of_arg;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	DWORD dwError=0;


	wstring pathW = StringToWstring(directory);
	STRSAFE_LPCWSTR path = pathW.c_str();
	StringCchLength(path, MAX_PATH, &length_of_arg);

	StringCchCopy(szDir, MAX_PATH, path);
	StringCchCat(szDir, MAX_PATH, TEXT("\\*"));

	hFind = FindFirstFile(szDir, &ffd);

	if (INVALID_HANDLE_VALUE != hFind) 
	{
		do
		{
			bool isDirectory = (0L != long(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY));
			if (isDirectory != files)
			{
				string name = WstringToString(ffd.cFileName);
				if (name != "." && name != "..")
					list.push_back(name);
			}
		}
		while (FindNextFile(hFind, &ffd) != 0);
		FindClose(hFind);
	}

	return list;
}


wstring FileSystem::StringToWstring(const string& str)
{
	wostringstream wstm ;
	const ctype<wchar_t>& ctfacet =
		use_facet< ctype<wchar_t> >( wstm.getloc() ) ;
	for( size_t i=0 ; i<str.size() ; ++i )
		wstm << ctfacet.widen( str[i] ) ;
	return wstm.str() ;
}


string FileSystem::WstringToString(const wstring& str)
{
	ostringstream stm ;
	const ctype<char>& ctfacet =
		use_facet< ctype<char> >( stm.getloc() ) ;
	for( size_t i=0 ; i<str.size() ; ++i )
		stm << ctfacet.narrow( str[i], 0 ) ;
	return stm.str() ;
}
