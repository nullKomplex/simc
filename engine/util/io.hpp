// ==========================================================================
// Dedmonwakeen's Raid DPS/TPS Simulator.
// Send questions to natehieter@gmail.com
// ==========================================================================

#ifndef SC_IO_HPP
#define SC_IO_HPP

#include "config.hpp"
#include <string>
#include <vector>
#include <fstream>

#if USE_TR1_NAMESPACE
// Use TR1
#include <tr1/memory>
namespace std {using namespace tr1; }
#else
// Use C++11
#include <memory>
#endif

struct sim_t;

namespace io {
// Converts a wide (UTF-16 or UTF-32) string to narrow (UTF-8)
std::string narrow( const wchar_t* wstr );
inline std::string narrow( const std::wstring& wstr )
{ return narrow( wstr.c_str() ); }

// Converts a narrow (UTF-8) string to wide (UTF-16 or UTF-32)
std::wstring widen( const char* str );
std::wstring widen( const std::string& str );

std::string latin1_to_utf8( const std::string& str );
std::string utf8_to_latin1( const std::string& str );
std::string maybe_latin1_to_utf8( const std::string& str );

// Like std::fopen, but works with UTF-8 filenames on windows.
FILE* fopen( const std::string& filename, const char* mode );

// RAII wrapper for FILE*.
class cfile
{
  std::shared_ptr<FILE> file;

  static void dont_close( FILE* ) {}
  static void safe_close( FILE* f )
  { if ( f ) std::fclose( f ); }

public:
  struct no_close {};
  cfile() {} // = default;
  cfile( const std::string& filename, const char* mode ) :
    file( fopen( filename, mode ), safe_close ) {}
  explicit cfile( FILE* f ) : file( f, safe_close ) {}
  cfile( FILE* f, no_close ) : file( f, dont_close ) {}
  const cfile operator=( FILE* f )
  { file = std::shared_ptr<FILE>( f, safe_close ); return *this; }

  operator FILE* () const { return file.get(); }

  // Only closes if this is the last reference to the file.
  void close() { file.reset(); }
};

class ofstream : public std::ofstream
{
public:
  ofstream& printf( const char* format, ... );
  void open( const char* filename, openmode mode = out | trunc );
  void open( const std::string& filename, openmode mode = out | trunc )
  { return open( filename.c_str(), mode ); }
  bool open( const std::string& filename, const std::vector<std::string>& prefix, openmode mode = out | trunc );
};

class ifstream : public std::ifstream
{
public:
  void open( const char* filename, openmode mode = in );
  void open( const std::string& filename, openmode mode = in )
  { return open( filename.c_str(), mode ); }
  bool open( const std::string& filename, const std::vector<std::string>& prefix, openmode mode = in );
};

class utf8_args : public std::vector<std::string>
{
public:
  utf8_args( int argc, char** argv );
};

#ifndef SC_WINDOWS
inline FILE* fopen( const std::string& filename, const char* mode )
{ return std::fopen( filename.c_str(), mode ); }
#endif

inline int fclose( cfile& file ) { file.close(); return 0; }

std::string read_file_content( FILE* file );
} // namespace io

#endif // SC_IO_HPP
