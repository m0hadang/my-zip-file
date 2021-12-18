#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../my_archive.h"
#include <sstream>
#include <vector>

using namespace mohadangkim;

#ifdef _WIN32
#define INPUT_PATH "\\test\\input\\"
#else
#define INPUT_PATH "/test/input/"
#endif

#define name_to_tar(X) #X ## ".tar"
#define FILE_PATH_SIZE 10240

#define VERY_LONG vecry_longggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggg

struct MyArchiveFixture : public ::testing::Test {
  static std::vector<Tar*> single_file_vec_;
  static Tar single_file_0byte_;
  static Tar single_file_1byte_;
  static Tar single_file_512byte_;
  static Tar single_file_513byte_;
  static Tar single_file_1024byte_;
  static Tar single_file_1025byte_;
  static Tar invalid_file_small_that_512;
  static Tar koread_file_name;
  static Tar VERY_LONG;

  static void SetUpTestCase() {
    std::wcout.imbue(std::locale( "kor" ));

    char cwd[FILE_PATH_SIZE] = "";
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
      std::cout << "current working dir => " << cwd << std::endl;
    }

    AddTestTarget(cwd, name_to_tar(single_file_0byte), single_file_0byte_);
    AddTestTarget(cwd, name_to_tar(single_file_1byte), single_file_1byte_);
    AddTestTarget(cwd, name_to_tar(single_file_512byte), single_file_512byte_);
    AddTestTarget(cwd, name_to_tar(single_file_513byte), single_file_513byte_);
    AddTestTarget(cwd, name_to_tar(single_file_1024byte), single_file_1024byte_);
    AddTestTarget(cwd, name_to_tar(single_file_1025byte), single_file_1025byte_);
    AddTestTarget(cwd, name_to_tar(single_file_1025byte), single_file_1025byte_);
    InitTestTarget(cwd, name_to_tar(invalid_file_small_that_512), invalid_file_small_that_512);
    {
      std::wstringstream stm;
      stm << cwd << INPUT_PATH << L"한글 파일 이름.tar";
      koread_file_name.init(stm.str().c_str());      
      single_file_vec_.emplace_back(&koread_file_name);
    }
    AddTestTarget(
      cwd, 
      name_to_tar(vecry_longggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggg), 
      VERY_LONG);
  }  
  virtual void SetUp() override {
  }
  virtual void TearDown() override {
  }
private:
  static void InitTestTarget(
    const char* current_working_dir_path, 
    const char* tar_file_name,
    Tar& tar) {
      std::wstringstream stm;
      stm << current_working_dir_path << INPUT_PATH << tar_file_name;
      tar.init(stm.str().c_str());
  }
  static void AddTestTarget(
    const char* current_working_dir_path, 
    const char* tar_file_name,
    Tar& tar) {
      InitTestTarget(current_working_dir_path, tar_file_name, tar);
      single_file_vec_.emplace_back(&tar);
  }
};

std::vector<Tar*> MyArchiveFixture::single_file_vec_;
Tar MyArchiveFixture::single_file_0byte_;
Tar MyArchiveFixture::single_file_1byte_;
Tar MyArchiveFixture::single_file_512byte_;
Tar MyArchiveFixture::single_file_513byte_;
Tar MyArchiveFixture::single_file_1024byte_;
Tar MyArchiveFixture::single_file_1025byte_;
Tar MyArchiveFixture::invalid_file_small_that_512;
Tar MyArchiveFixture::koread_file_name;
Tar MyArchiveFixture::VERY_LONG;

TEST_F(MyArchiveFixture, FileOpenTest) {
  for(const auto it : single_file_vec_) {
    EXPECT_EQ(it->status(), TAR_ERR_CODE::OK);
  }
}

TEST_F(MyArchiveFixture, FileSizeTest) {
  EXPECT_EQ(invalid_file_small_that_512.status(), TAR_ERR_CODE::INVALID_FILE_SIZE);
}

TEST_F(MyArchiveFixture, TestKoreaFileName) {
  auto koread_file_name_header = koread_file_name.at(0);
  std::wstring str1 = to_wstr(koread_file_name_header->name);
  std::wstring str2 = L"한글 파일 이름.txt";
  EXPECT_EQ(str1, str2);
}

void print_hex(const char* data, size_t size) {
  for (size_t i = 0; i < size; ++i) {
    std::cout << "0x" << std::hex << (int)data[i] << ",";
  }
  std::cout << std::endl;
}

TEST_F(MyArchiveFixture, TarHeaderFormat) {
  std::shared_ptr<posix_header> first_tar_header = VERY_LONG.at(0);
  std::cout << first_tar_header->name << std::endl;
  // print_hex(first_tar_header->linkname, PH_LINKNAME_SIZE);
  // print_hex(first_tar_header->prefix, PH_PREFIX_SIZE);
  std::cout << first_tar_header->typeflag << std::endl;
//EXPECT_STREQ(first_tar_header->name, "a.txt");
//print_str_should_fill_null_but_no_need_end_null(tar_header.name);
//print_num_must_end_null(tar_header.mode);
//print_num_must_end_null(tar_header.uid);
//print_num_must_end_null(tar_header.gid);
//print_num_recommend_end_null_(tar_header.size);
//print_num_recommend_end_null_(tar_header.mtime);
//print_num_must_end_null(tar_header.chksum);
//print_ch(tar_header.typeflag);
//print_str_should_fill_null_but_no_need_end_null(tar_header.linkname);
//print_str_must_end_null(tar_header.magic);
//print_num_recommend_end_null_(tar_header.version);
//print_str_must_end_null(tar_header.uname);
//print_str_must_end_null(tar_header.gname);
//print_num_must_end_null(tar_header.devmajor);
//print_num_must_end_null(tar_header.devminor);
//print_str_should_fill_null_but_no_need_end_null(tar_header.prefix);  
}