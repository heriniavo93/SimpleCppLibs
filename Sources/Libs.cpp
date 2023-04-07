#include "Libs.h"


Libs::FileIo::FileIo( std::string filepath )
{
    if( fileExists( filepath ) )
        stream_ = new std::fstream( filepath );
}

Libs::FileIo::~FileIo()
{
    if( stream_ )
        stream_->close();
}

/// File Io
bool Libs::FileIo::fileExists( const std::string& path )
{
    struct stat path_stat;
    stat( path.c_str(), &path_stat );
    
    return S_ISREG( path_stat.st_mode ) == 1;
}

bool Libs::FileIo::directoryExists( const std::string& path )
{
    struct stat info;
    
    if( stat( path.c_str(), &info ) != 0 )
    {
        std::cout << "Impossible d'accéder le chemin :" << path << std::endl;
        return false;
    }
    
    else if( info.st_mode & S_IFDIR )
        return true;
        
    return false;
}

bool Libs::FileIo::asciiFileContent( std::string& content, std::string path )
{
    if( fileExists( path ) == false )
    {
        content = "";
        return false;
    }
    
    std::ifstream file( path );
    
    if( file )
    {
        content = std::string( ( std::istreambuf_iterator<char>( file ) ),
                std::istreambuf_iterator<char>() );
        file.close();
        return true;
    }
    
    return false;
}

std::vector<std::string> Libs::FileIo::readlines( std::string const& filename )
{
    std::vector<std::string> lines;
    std::string linestr;
    std::ifstream file( filename );
    
    if( file )
    {
        while( std::getline( file, linestr ) )
            lines.emplace_back( linestr );
    }
    
    return lines;
}

std::vector<std::string> Libs::FileIo::directoryList( const std::string& dir, const bool& recursive )
{
    std::vector<std::string> dirs;
    
    if( directoryExists( dir ) == false )
        return dirs;
        
    DIR* dire;
    struct dirent* ent;
    
    if( ( dire = opendir( dir.c_str() ) ) == NULL )
    {
        std::cout << "Impossible d'accéder au dossier." << std::endl;
        return dirs;
    }
    
    while( ( ent = readdir( dire ) ) != NULL )
    {
        std::string temp = ent->d_name;
        
        if( temp == "." || temp == ".." )
            continue;
            
        std::string abspath = dir + "\\" + temp;
        
        if( isDirectory( abspath ) )
            dirs.emplace_back( abspath );
    }
    
    if( recursive == true )
    {
        for( int i = 0; i < dirs.size(); i++ )
        {
            auto temp = directoryList( dirs[i], true );
            
            if( temp.size() > 0 )
            {
                dirs.erase( dirs.begin() + i );
                dirs.insert( dirs.end(), temp.begin(), temp.end() );
                i--;
            }
        }
    }
    
    return dirs;
}

std::vector<std::string> Libs::FileIo::fileList( const std::string& dir, std::string ext, const bool& recursive )
{
    //check extension mask ext1,ext2,ext3
    std::vector<std::string> dirs, files;
    
    if( fileExists( dir ) == true )
        return { dir };
        
    if( directoryExists( dir ) == false )
        return {};
        
    DIR* dire;
    
    struct dirent* ent;
    
    if( ( dire = opendir( dir.c_str() ) ) == NULL )
    {
        std::cout << "Impossible d'accéder au dossier." << std::endl;
        return files;
    }
    
    if( recursive == false )
    {
        while( ( ent = readdir( dire ) ) != NULL )
        {
            std::string temp = ent->d_name;
            
            if( temp == "." || temp == ".." )
                continue;
                
            std::string abspath = dir + "\\" + temp;
            
            if( isFile( abspath ) == true )
            {
                if( ext == "" )
                    files.emplace_back( abspath );
                else
                {
                    auto extension = *StringOp::split( abspath, "." ).rbegin();
                    
                    if( ext.find( extension ) != std::string::npos )
                        files.emplace_back( abspath );
                }
                
                
            }
        }
        
        return files;
    }
    
    dirs = directoryList( dir, true );
    
    for( const std::string d : dirs )
    {
        auto temp = fileList( d, ext );
        files.insert( files.end(), temp.begin(), temp.end() );
    }
    
    return files;
}

bool Libs::FileIo::isFile( const std::string& path )
{
    struct stat path_stat;
    stat( path.c_str(), &path_stat );
    return S_ISREG( path_stat.st_mode ) == 1;
}

bool Libs::FileIo::isDirectory( const std::string& path )
{
    struct stat path_stat;
    stat( path.c_str(), &path_stat );
    
    return S_ISDIR( path_stat.st_mode ) == 1;
}

Libs::path_params_ Libs::FileIo::pathParams( const std::string& path )
{
    Libs::path_params_ param;
    param.abs_path_ = path;
    auto res = StringOp::split( path, "." );
    param.ext_ = *res.rbegin();
    res = StringOp::split( path, "\\" );
    param.filename_ = *res.rbegin();
    
    for( int i = 0; i < res.size() - 1; i++ )
        param.dir_ += res[i] + "\\";
        
    param.dir_.pop_back();
    return param;
}

Libs::path_datas_ Libs::FileIo::walk( const std::string& path )
{
    path_datas_ data;
    data.dir_ = path;
    data.dirs_ = directoryList( path, true );
    data.files_ = fileList( path, "", true );
    return data;
}

bool Libs::FileIo::rename( std::string pathFrom, std::string pathTo )
{
    if( std::rename( pathFrom.c_str(), pathTo.c_str() ) )
        return false;
        
    return true;
}

bool Libs::FileIo::remove( std::string path )
{
    if( std::remove( path.c_str() ) != 0 )
        return false;
        
    return true;
}

bool Libs::FileIo::copy( std::string pathFrom, std::string pathTo )
{
    if( fileExists( pathFrom ) == false )
        return false;
        
    std::ifstream  src( pathFrom, std::ios::binary );
    bool success = false;
    
    if( src )
    {
        std::ofstream  dst( pathTo, std::ios::binary );
        
        if( dst.is_open() )
        {
            dst << src.rdbuf();
            success = true;
        }
        
        src.close();
    }
    
    if( success )
        return true;
        
    return false;
}

std::ifstream::pos_type Libs::FileIo::size( std::string filename )
{
    if( fileExists( filename ) )
    {
        std::ifstream in( filename, std::ifstream::ate | std::ifstream::binary );
        
        if( in )
            return in.tellg();
    }
    
    return 0;
}

std::string Libs::FileIo::joinPath( std::string dir, std::string filename )
{
    if( dir.back() == '\\' )
        return dir + filename;
    else
        return dir + "\\" + filename;
}

Libs::Timer::Timer( std::string msg )
{
    msg_ = msg;
    time_started_ = std::chrono::high_resolution_clock::now();
}

Libs::Timer::~Timer()
{
    pause();
    auto d = std::chrono::duration<double>( duration_ ).count();
    std::cout << msg_ << " :" << std::fixed << d << " secondes." << std::endl;
}

void Libs::Timer::resume()
{
    if( !paused_ ) return;
    
    paused_ = false;
    time_started_ = std::chrono::high_resolution_clock::now();
}

void Libs::Timer::pause()
{
    if( paused_ ) return;
    
    std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
    duration_ += ( now - time_started_ ) ;
    paused_ = true;
}

std::vector<std::string> Libs::StringOp::split( std::string str, std::string delimiter )
{
    std::vector<std::string> list;
    size_t pos = 0;
    std::string token;
    
    while( ( pos = str.find( delimiter ) ) != std::string::npos )
    {
        token = str.substr( 0, pos );
        str.erase( 0, pos + delimiter.length() );
        list.emplace_back( token );
    }
    
    list.emplace_back( str );
    
    return list;
}

std::wstring Libs::StringOp::toWstring( std::string const& in )
{
    std::wstring_convert<std::codecvt_utf8_utf16<unsigned short>, unsigned short> convert;
    auto s = convert.from_bytes( in );
    auto p = reinterpret_cast<wchar_t const*>( s.data() );
    return std::wstring( p, p + s.size() );
}

std::string Libs::StringOp::fromWstring( std::wstring const& in )
{
    std::wstring_convert<std::codecvt_utf8_utf16<unsigned short>, unsigned short> convert;
    auto p = reinterpret_cast<unsigned short const*>( in.data() );
    return convert.to_bytes( p, p + in.size() );
}

std::string Libs::StringOp::trim( const std::string& str, const char& c )
{
    std::string result = str;
    result = trimLeft( str, c );
    result = trimRight( result, c );
    return result;
}

std::wstring Libs::StringOp::trim( const std::wstring& str, const char& c )
{
    auto result = fromWstring( str );
    result = trimLeft( result, c );
    result = trimRight( result, c );
    return toWstring( result );
}


std::vector<int> Libs::StringOp::getIntegers( std::string sentence )
{
    std::vector<int> integers;
    int size = sentence.size();
    
    if( size < 1 )
        return {};
        
    std::string integer = "";
    
    for( int i = 0; i < size; i++ )
    {
        if( std::isdigit( sentence[i] ) )
            integer += sentence[i];
        else
        {
            if( integer.size() > 0 )
                integers.emplace_back( std::stoi( integer ) );
                
            integer = "";
        }
    }
    
    if( integer != "" )
        integers.emplace_back( std::stoi( integer ) );
        
    return integers;
}

std::vector<double> Libs::StringOp::getDoubles( std::string sentence )
{
    std::vector<double> doubles;
    int size = sentence.size();
    
    if( size < 1 )
        return {};
        
    std::string item = "";
    
    for( int i = 0; i < size; i++ )
    {
        if( std::isdigit( sentence[i] ) || sentence[i] == '.' )
            item += sentence[i];
        else
        {
            if( item.size() > 0 )
            {
                if( item != "." )
                {
                    if( item.back() == '.' )
                        item.pop_back();
                        
                    if( item.size() > 0 )
                        doubles.emplace_back( std::stod( item ) );
                }
            }
            
            item = "";
        }
    }
    
    if( item != "" && item != "." )
    {
        if( item.back() == '.' )
            item.pop_back();
            
        if( item.size() > 0 )
            doubles.emplace_back( std::stod( item ) );
            
    }
    
    return doubles;
}

bool Libs::StringOp::containsNumber( std::string sentence )
{
    return ( sentence.find_first_of( "0123456789" ) != std::string::npos );
}

std::string Libs::StringOp::trimLeft( std::string source, const char& c )
{
    if( source.size() < 1 )
        return "";
        
    while( source.size() > 0 && source.front() == c )
        source.erase( source.begin() );
        
    return source;
}

std::string Libs::StringOp::trimRight( std::string source, const char& c )
{
    if( source.size() < 1 )
        return "";
        
    while( source.size() > 0 && source.back() == c )
        source.pop_back();
        
    return source;
}

std::string Libs::StringOp::toLower( std::string source )
{
    std::transform( source.begin(), source.end(), source.begin(), ::tolower );
    return source;
    
}

std::string Libs::StringOp::toUpper( std::string source )
{
    std::transform( source.begin(), source.end(), source.begin(), ::toupper );
    return source;
}

bool Libs::StringOp::isUpper( const std::string& word )
{
    if( word.empty() )
        return true;
        
    for( size_t i = 0; i < word.length(); i++ )
        if( !isupper( static_cast<unsigned char>( word[i] ) ) )
            return false;
            
    return true;
}

bool Libs::StringOp::isLower( const std::string& word )
{
    if( word.empty() )
        return true;
        
    for( size_t i = 0; i < word.length(); i++ )
        if( !islower( static_cast<unsigned char>( word[i] ) ) )
            return false;
            
    return true;
}

bool Libs::StringOp::isNumeric( std::string source )
{
    for( auto liter : source )
        if( isdigit( liter ) == false )
            return false;
            
    return true;
}

std::vector<double> Libs::StringOp::doubles( std::string buffer )
{
    std::vector<double> res;
    std::regex rx( R"((?:^|\s)([+-]?[[:digit:]]+(?:\.[[:digit:]]+)?)(?=$|\s))" );
    std::smatch m;
    std::string str = buffer;
    
    while( regex_search( str, m, rx ) )
    {
        auto in = std::stod( m[1] );
        res.emplace_back( in );
        str = m.suffix().str();
    }
    
    return res;
}

