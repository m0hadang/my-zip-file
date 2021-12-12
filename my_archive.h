#ifndef MY_ARCHIVE_H_
#define MY_ARCHIVE_H_

#include <vector>
#include <fstream>
#include <iomanip>

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
   *
   * 출처: https://joy24.tistory.com/86?category=391670 [항상 기뻐하라(Be joyful
   * always;)]
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

class Tar {
private:
  std::vector<std::shared_ptr<posix_header>> tar_headers_;

public:
  bool read(const char* file_path) { 
    std::ifstream in(file_path, std::ios::in | std::ios::binary);
    if (in.is_open() == false) {
      std::cout << "open file error : " << file_path << std::endl;
      std::cout << strerror(errno) << std::endl;
      return false;
    }

    while (in.eof() == false) {
      auto tar_header = std::make_shared<posix_header>();
      init_header(in, *tar_header);
      tar_headers_.emplace_back(tar_header);
    }

    return true;
  }
  std::shared_ptr<posix_header> at(size_t idx) {
    if (idx >= tar_headers_.size()) {
      return nullptr;
    }
    return tar_headers_[idx];
  }
private:
  void init_header(std::ifstream& in, posix_header& tar_header) {
    in.read((char*)&tar_header, sizeof(posix_header));
  }
};

}  // namespace mohadangkim

#endif