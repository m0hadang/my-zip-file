#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../my_archive.h"
#include <strstream>

using namespace mohadangkim;

#define input_path "/test/input/"
#define tar_1_path input_path "tar_1.tar"

struct MyArchiveFixture : public ::testing::Test {
  static Tar tar;
  static bool is_open;
  static void SetUpTestCase() {
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
      std::cout << "current working dir => " << cwd << std::endl;
    }
    
    std::strstream stm;
    stm << cwd << tar_1_path;

    is_open = tar.read(stm.str());
  }  
  virtual void SetUp() override {
  }
  virtual void TearDown() override {
  }
};

bool MyArchiveFixture::is_open;
Tar MyArchiveFixture::tar;


TEST_F(MyArchiveFixture, ReadTar) {
  EXPECT_TRUE(is_open);
}

TEST_F(MyArchiveFixture, TarHeaderFormat) {
  std::shared_ptr<posix_header> first_tar_header = tar.at(0);
  EXPECT_STREQ(first_tar_header->name, "a.txt");
//   print_str_should_fill_null_but_no_need_end_null(tar_header.name);
//   print_num_must_end_null(tar_header.mode);
//   print_num_must_end_null(tar_header.uid);
//   print_num_must_end_null(tar_header.gid);
//   print_num_recommend_end_null_(tar_header.size);
//   print_num_recommend_end_null_(tar_header.mtime);
//   print_num_must_end_null(tar_header.chksum);
//   print_ch(tar_header.typeflag);
//   print_str_should_fill_null_but_no_need_end_null(tar_header.linkname);
//   print_str_must_end_null(tar_header.magic);
//   print_num_recommend_end_null_(tar_header.version);
//   print_str_must_end_null(tar_header.uname);
//   print_str_must_end_null(tar_header.gname);
//   print_num_must_end_null(tar_header.devmajor);
//   print_num_must_end_null(tar_header.devminor);
//   print_str_should_fill_null_but_no_need_end_null(tar_header.prefix);  
}