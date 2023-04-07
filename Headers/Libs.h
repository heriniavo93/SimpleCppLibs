#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sys/stat.h>
#include "dirent.h"
#include <sstream>
#include <codecvt>
#include <chrono>
#include <algorithm>
#include <map>
#include <iterator>
#include <iomanip> // put_time
#include <cctype>
#include <Windows.h>
#include <Commdlg.h>
#include <Shlobj_core.h>
#include <regex>




namespace Libs {
    struct path_params_
    {
        std::string dir_;
        std::string abs_path_;
        std::string filename_;
        std::string ext_;
    };
    
    struct path_datas_
    {
        std::string dir_;
        std::vector<std::string> files_;
        std::vector<std::string> dirs_;
    };
    
    class FileIo {
    
        public:
            FileIo() = delete;
            FileIo( std::string filepath );
            ~FileIo();
            static bool fileExists( const std::string& path );
            static bool directoryExists( const std::string& path );
            static bool asciiFileContent( std::string& content, std::string path );
            static std::vector<std::string> readlines( std::string const& filename );
            static std::vector<std::string> directoryList( const std::string& dir, const bool& recursive = false );
            static std::vector<std::string> fileList( const std::string& dir, std::string ext = "", const bool& recursive = false );
            static bool isFile( const std::string& path );
            static bool isDirectory( const std::string& path );
            static path_params_ pathParams( const std::string& path );
            static path_datas_ walk( const std::string& path );
            template<typename T>
            static bool write( const std::string& filepath, std::vector<T>& data );
            static bool rename( std::string pathFrom, std::string pathTo );
            static bool remove( std::string path );
            static bool copy( std::string pathFrom, std::string pathTo );
            static std::ifstream::pos_type size( std::string filename );
            static std::string joinPath( std::string dir, std::string filename );
            
        private:
            std::fstream* stream_;
            
            
    };
    
    template<typename T>
    inline bool FileIo::write( const std::string& filepath, std::vector<T>& content )
    {
        std::ofstream output( filepath );
        
        if( output.is_open() )
        {
            // créer un itérateur d'interface de sortie
            std::ostream_iterator<T> output_iterator( output, "\n" );
            std::copy( content.begin(), content.end(), output_iterator );
            output.close();
            return true;
        }
        
        std::cout << "Impossible d'enregistrer le fichier" << std::endl;
        return false;
    }
    
    class Timer {
        public:
            Timer( std::string msg = "Execution time" );
            ~Timer();
            void resume();
            void pause();
        private:
            std::chrono::high_resolution_clock::time_point time_started_ = std::chrono::high_resolution_clock::now();
            std::chrono::high_resolution_clock::duration duration_ = {};
            bool paused_ = false;
            std::string msg_;
    };
    
    class StringOp {
        public:
            static std::vector<std::string> split( std::string str, std::string delimiter = " " );
            static std::wstring toWstring( std::string const& in );
            static std::string fromWstring( std::wstring const& in );
            static std::string trim( const std::string& str, const char& c = ' ' );
            static std::wstring trim( const std::wstring& str, const char& c = ' ' );
            static std::vector<int> getIntegers( std::string sentence );
            static std::vector<double> getDoubles( std::string sentence );
            static bool containsNumber( std::string sentence );
            static std::string trimLeft( std::string source, const char& c = ' ' );
            static std::string trimRight( std::string source, const char& c = ' ' );
            static std::string toLower( std::string source );
            static std::string toUpper( std::string source );
            static bool isUpper( const std::string& word );
            static bool isLower( const std::string& word );
            static bool isNumeric( std::string source );
            static std::vector<double> doubles( std::string buffer );
        private:
    };
    
}


