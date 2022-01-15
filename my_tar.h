#ifndef MY_ARCHIVE_H_
#define MY_ARCHIVE_H_

#include <fstream>
#include <iomanip>
#include <vector>
#include <codecvt>
#include <iostream>

#include <windows.h>
#include <strsafe.h>

namespace mohadangkim {

#define PH_NAME_SIZE 100
#define PH_MODE_SIZE 8
#define PH_UID_SIZE 8
#define PH_GID_SIZE 8
#define PH_SIZE_SIZE 12
#define PH_MTIME_SIZE 12
#define PH_CHKSUM_SIZE 8
#define PH_LINKNAME_SIZE 100
#define PH_MAGIC_SIZE 6
#define PH_VERSION_SIZE 2
#define PH_UNAME_SIZE 32
#define PH_GNAME_SIZE 32
#define PH_DEVMAJOR_SIZE 8
#define PH_DEVMINOR_SIZE 8
#define PH_PREFIX_SIZE 155
#define PH_PADDING 12

#define PH_HEADER_SIZE 512

#define TYPE_FLAG_DIR '5'

struct posix_header { /* byte offset */
  /*
   * 파일명, 디렉터리명, 디렉터리인 경우 패스 경로 '/' 로 끝난다
   */
  char name[PH_NAME_SIZE]; /*   0 */
  /*
   * common : 0000755
   * windows : 000755, 7번째 자리는 ' ' 로 채우는 경우 있음
   */
  char mode[PH_MODE_SIZE]; /* 100 */
  char uid[PH_UID_SIZE];   /* 108 */
  char gid[PH_GID_SIZE];   /* 116 */
  char size[PH_SIZE_SIZE]; /* 124 */
  /*
   * 파일 수정 시간 : php filemtime()
   */
  char mtime[PH_MTIME_SIZE]; /* 136 */
  /*
   * [code php;gutter:false] function get_checksum() {
   *   global $HEADER;
   *
   *   $sum = 0;
   *
   *   for ($i=0;$i<512;$i++) {
   *     if ($i < 148 || 156 < $i) { // checksum 필드 제외하고 모두 합해서 계산
   *       $sum += ord($HEADER[$i]);
   *     } else {
   *       $sum += ord(" ");
   *     }
   *   }
   *
   *   return $sum;
   * } [/code]
   */
  char chksum[PH_CHKSUM_SIZE]; /* 148 */
  /*
   * "typeflag"는 파일 형식을 나타내는 필드로 디렉토리일 때는 "5"가 기록되며
   * 파일일 경우에는 "0"
   *
   * '0' or null Regular file
   * '1' Link to another file already archived, hard link
   * '2' Symbolic link
   * '3' Character special device
   * '4' Block special device
   * '5' Directory
   * '6' FIFO special file
   * '7' Reserved A-Z Available for custom usage
   */
  char typeflag; /* 156 */

  //^^^^ should fill right data ^^^^
  char linkname[PH_LINKNAME_SIZE]; /* 157 */
  /*
   * 압축 알고리즘 이름, 보통 ustar, GNUtar, 6자가 안되는 부분은 ' '로 채움
   */
  char magic[PH_MAGIC_SIZE];            /* 257 */
  char version[PH_VERSION_SIZE];        /* 263 */
  char uname[PH_UNAME_SIZE]; /* 265 */  // login user, login group name
  char gname[PH_GNAME_SIZE];            /* 297 */
  char devmajor[PH_DEVMAJOR_SIZE];      /* 329 */
  char devminor[PH_DEVMINOR_SIZE];      /* 337 */
  char prefix[PH_PREFIX_SIZE];          /* 345 */
  char padding[PH_PADDING];             /* 500 */
                                        /* 512 */

  /*
   * USTAR 포맷의 경우를 보면 prefix 필드의 값이 null이 아니라면 100자가 넘는
   * 파일명이 가능하도록 name 필드 앞에 붙게 됩니다. 그러나 이전의 유닉스 호환
   * 포맷이나 윈도우 시스템에서 TAR 아카이브를 만들 수 있는 윈도우커맨더 등과의
   * 호환성을 생각한다면 prefix 필드를 null로 남겨두는 것이 좋을 듯하며 100자가
   * 넘는 파일명인 경우 뒷쪽을 짤라내어 100자까지만 name 필드에 저장하는 것이
   * 좋을 듯합니다.
   */
};

class DirectoryNameInfo {
 public:
  std::wstring base_path;
  std::wstring name;
  DirectoryNameInfo(
    const std::wstring& base_path_, 
    const std::wstring& name_)
      : base_path(base_path_), name(name_) {}
};

std::wstring to_wstr(const std::string& t_str) {
  typedef std::codecvt_utf8<wchar_t> convert_type;
  std::wstring_convert<convert_type, wchar_t> converter;
  return converter.from_bytes(t_str);
}

int32_t oct_to_size(char* buf, size_t size){
  int32_t out = 0;
  size_t i = 0;
  for (size_t i = 0; i < size; ++i) {
    if (buf[i] == '\0') {
      break;
    }

    const bool is_oct_ch = ('0' <= buf[i] && buf[i] <= '7');
    if (is_oct_ch == false) {
      out = -1;
      break;
    }

    out = (out << 3);
    out = out | (buf[i] - '0');
  }

  return out;
}

std::wstring convert_stand_path(const std::wstring& path) {
  auto str = path;
  std::replace(str.begin(), str.end(), L'\\', L'/');
  return str;
}

std::wstring convert_valid_dir_path(const std::wstring& path) {
  return path;
}

DirectoryNameInfo get_dir_name_info(const std::wstring& path) {

  auto std_path = convert_stand_path(path);

  wchar_t split_ch = L'/';
  std::wstring split_str = L"/";
  if (std_path.empty() || std_path == split_str) {
    return DirectoryNameInfo(L"", split_str);
  }

  if (std_path.size() == 1) {
    return DirectoryNameInfo(L"", std_path + split_str);
  }

  size_t end_pivot = std_path.size() - 1;
  if (std_path[end_pivot] == split_ch) {
    end_pivot--;
  }

  size_t find_pos = std_path.rfind(split_ch, end_pivot);
  if (find_pos == std::wstring::npos) {
    find_pos = 0;
  } else {
    ++find_pos;
  }

  return DirectoryNameInfo(
    std_path.substr(0, find_pos), 
    std_path.substr(find_pos, end_pivot - find_pos + 1) + split_str);
}

enum class TAR_ERR_CODE {
  OK,
  TAR_ERR_FILE_OPEN_FAIL,
  TAR_ERR_DIR_OPEN_FAIL,
  TAR_ERR_INVALID_FILE_SIZE,
  TAR_FILE_NAME_IS_TOO_LONG,
  TAR_ERR_UNKNOWN,
};

class Tar {
private:
  std::wstring tar_file_path_;
  TAR_ERR_CODE status_ = TAR_ERR_CODE::TAR_ERR_UNKNOWN;
  size_t tar_file_size_ = 0;
  std::vector<std::shared_ptr<posix_header>> file_headers_;
  std::vector<std::shared_ptr<posix_header>> dir_headers_;

public:
  bool init_tar(const wchar_t* tar_file_path) {
    tar_file_path_ = std::wstring(tar_file_path);

    std::ifstream in;
    bool is_open = open_file(in);
    if (is_open == false) {
      status_ = TAR_ERR_CODE::TAR_ERR_FILE_OPEN_FAIL;
      return false;
    }

    tar_file_size_ = get_file_size(in);
    if (check_valid_file_size(tar_file_size_) == false) {
      status_ = TAR_ERR_CODE::TAR_ERR_INVALID_FILE_SIZE;
      return false;
    }

    const size_t tar_file_content_size = tar_file_size_ - (PH_HEADER_SIZE * 2);
    for (size_t total_read_size = 0; total_read_size < tar_file_content_size;) {
      auto tar_header = std::make_shared<posix_header>();

      read_header(in, PH_HEADER_SIZE, *tar_header);
      total_read_size += PH_HEADER_SIZE;

      const auto content_size = oct_to_size(tar_header->size, PH_SIZE_SIZE);
      if (content_size < 0) {
        status_ = TAR_ERR_CODE::TAR_ERR_INVALID_FILE_SIZE;
        return false;
      }
      if (is_dir_tar_header(tar_header) == false && content_size > 0) {
        size_t content_block_space_size = get_content_block_space_size(content_size);
        in.seekg(content_block_space_size, std::ios::cur);  // jump content
        total_read_size += content_block_space_size;
      }

      add_tar_header(tar_header);
    }

    in.close();
    status_ = TAR_ERR_CODE::OK;

    return true;
  }
  bool init_dir(const wchar_t* dir_path) {  

    auto dir_name_info = get_dir_name_info(dir_path);
    
    std::vector<std::wstring> dir_list;
    status_ = dir_listing(dir_path, dir_name_info.name, dir_list);
    if(status_ != TAR_ERR_CODE::OK) {
      return false;
    }

    // for(const auto& item : dir_list) {
    //   std::wcout << item << std::endl;
    // }

    return true;
  }
  std::shared_ptr<posix_header> file_at(size_t idx) {
    if (idx >= file_headers_.size()) {
      return nullptr;
    }
    return file_headers_[idx];
  }
  std::shared_ptr<posix_header> dir_at(size_t idx) {
    if (idx >= dir_headers_.size()) {
      return nullptr;
    }
    return dir_headers_[idx];
  }
  TAR_ERR_CODE status() { 
    return status_; 
  }
  size_t file_count() {
    return file_headers_.size(); 
  }
  size_t dir_count() {
    return dir_headers_.size(); 
  }  
  size_t tar_file_size() { 
    return tar_file_size_; 
  }
  void print() {
    std::wcout << "tar file path : " << tar_file_path_ << std::endl;
    std::wcout << "status_ : " << (int32_t)status() << std::endl;
    std::wcout << "file_size : " << tar_file_size_ << std::endl;
    std::wcout << "file_count : " << file_count() << std::endl;
  }
  static bool is_dir_tar_header(std::shared_ptr<posix_header> tar_header) {
    if (tar_header->typeflag != TYPE_FLAG_DIR) {
      return false;
    }

    size_t last_idx = strlen(tar_header->name) - 1;
    if (tar_header->name[last_idx] != '/') {//TODO : Not Cross platform
      return false;
    }

    return true;
  }
  static TAR_ERR_CODE dir_listing(//TODO : Not Cross platform
    const std::wstring& dir_path,
    const std::wstring& cur_dir_path,
    std::vector<std::wstring>& dir_list) {
    
    setlocale(LC_ALL, "");

    // Check that the input path plus 3 is not longer than MAX_PATH.
    // Three characters are for the "\*" plus NULL appended below.
    size_t length_of_arg;
    StringCchLengthW(dir_path.c_str(), MAX_PATH, &length_of_arg);
    if (length_of_arg > (MAX_PATH - 3)) {
      std::wcout << "directory path is too long" << std::endl;
      return TAR_ERR_CODE::TAR_FILE_NAME_IS_TOO_LONG;
    }

    // Prepare string for use with FindFile functions.  First, copy the
    // string to a buffer, then append '\*' to the directory name.
    wchar_t target_dir_path[MAX_PATH] = L"";
    StringCchCopyW(target_dir_path, MAX_PATH, dir_path.c_str());
    StringCchCatW(target_dir_path, MAX_PATH, L"\\*");

    // Find the first file in the directory.
    WIN32_FIND_DATAW ffd;
    HANDLE hwd_file = FindFirstFileW(target_dir_path, &ffd);

    if (INVALID_HANDLE_VALUE == hwd_file) {
      std::wcout << "find first file fail(1), " << GetLastError() << std::endl;
      std::wcout << "|_ " << target_dir_path << std::endl;
      return TAR_ERR_CODE::TAR_ERR_DIR_OPEN_FAIL;
    } 
    
    // List all the files in the directory with some info about them.
    do {
      std::wstring file_name = ffd.cFileName;
      if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) { //dir
        if (file_name == L"..") {
          continue;
        }
        if (file_name == L".") {
          dir_list.emplace_back(cur_dir_path);
          continue;
        }
        auto child = dir_listing(
          dir_path + file_name + L"\\",//TODO : Not Cross platform
          cur_dir_path + file_name + L"/",
          dir_list);
        // res.insert(child.begin(), child.end(), res.end());
      } else {
        LARGE_INTEGER filesize;
        filesize.LowPart = ffd.nFileSizeLow;
        filesize.HighPart = ffd.nFileSizeHigh;
        dir_list.emplace_back(cur_dir_path + file_name);
      }
    } while (FindNextFileW(hwd_file, &ffd) != 0);

    auto last_error_num = GetLastError();
    if (last_error_num != ERROR_NO_MORE_FILES) {
      std::wcout << "find first file fail(2)" << std::endl;
    }

    FindClose(hwd_file);

    return TAR_ERR_CODE::OK;
  }
private:
  void add_tar_header(std::shared_ptr<posix_header> tar_header) {
    if (is_dir_tar_header(tar_header)) {
      dir_headers_.emplace_back(tar_header);
    } else {
      file_headers_.emplace_back(tar_header);
    }
  }
  bool check_valid_file_size(size_t file_size) {    
    if (file_size < PH_HEADER_SIZE) {
      return false;
    }
    return (file_size % PH_HEADER_SIZE) == 0;
  }
  bool open_file(std::ifstream& in) {
    in.open(tar_file_path_.c_str(), std::ios::in | std::ios::binary);
    if (in.is_open() == false) {
      std::wcout << "open file error : " << tar_file_path_.c_str() << std::endl;
      std::wcout << strerror(errno) << std::endl;
      return false;
    }
    return true;
  }
  size_t get_file_size(std::ifstream& in) {
    auto file_size = in.tellg();
    in.seekg(0, std::ios::end);
    file_size = in.tellg() - file_size;
    in.seekg(0, std::ios::beg);
    return (size_t)file_size;
  }
  void read_header(std::ifstream& in, const size_t read_size, posix_header& tar_header) {
    in.read((char*)&tar_header, read_size);
  }
  size_t get_content_block_space_size(size_t content_size) {
    size_t content_alloc_space_size = 0;
    size_t block_count = (content_size / PH_HEADER_SIZE);
    const int remain_block_size = (content_size % PH_HEADER_SIZE);
    if (block_count < 1) {
      content_alloc_space_size = PH_HEADER_SIZE * 1;
    } else {
      if (remain_block_size != 0) {
        ++block_count;
      }
      content_alloc_space_size = PH_HEADER_SIZE * block_count;
    }

    return content_alloc_space_size;
  }

};

}  // namespace mohadangkim

#endif