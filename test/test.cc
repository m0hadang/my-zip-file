#include <gtest/gtest-matchers.h>
#include <gmock/gmock-matchers.h>
#include <gmock/gmock-more-matchers.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../my_tar.h"
#include <sstream>
#include <vector>

using namespace mohadangkim;

#ifdef _WIN32
#define TEST_DATA_TAR_PATH "\\test\\test_data_tar\\"
#else
#define TEST_DATA_TAR_PATH "/test/test_data_tar/"
#endif

#define name_to_tar(X) #X ## ".tar"
#define FILE_PATH_SIZE 10240

using testing::StartsWith;
using testing::Matcher;

TEST(SUT_UTILITY, to_wstr) {
	char buf[1024] = {
		(char)0xED,(char)0x95,(char)0x9C,(char)0xEA,(char)0xB8,
		(char)0x80,(char)0x20,(char)0xED,(char)0x8C,(char)0x8C,
		(char)0xEC,(char)0x9D,(char)0xBC,(char)0x20,(char)0xEC,
		(char)0x9D,(char)0xB4,(char)0xEB,(char)0xA6,(char)0x84,
		(char)0x2E,(char)0x74,(char)0x78,(char)0x74,
		(char)0x00 };//L"한글 파일 이름.txt"
  
  std::wstring str1 = to_wstr(buf);
  std::wstring str2 = L"한글 파일 이름.txt";
  EXPECT_EQ(str1, str2);
}

TEST(SUT_UTILITY, oct_to_size) {
  const size_t buf_size = 7;

  char num0[buf_size] = "000000";
  char num1[buf_size] = "000001";
  char num2[buf_size] = "000002";
  char num7[buf_size] = "000007";
  char num8[buf_size] = "000010";
  char num9[buf_size] = "000011";
  char num15[buf_size] = "000017";
  char num16[buf_size] = "000020";
  char num63[buf_size] = "000077";
  char num64[buf_size] = "000100";

  size_t test_size = buf_size;
  EXPECT_EQ(oct_to_size(num0, test_size), 0);
  EXPECT_EQ(oct_to_size(num1, test_size), 1);
  EXPECT_EQ(oct_to_size(num2, test_size), 2);
  EXPECT_EQ(oct_to_size(num7, test_size), 7);
  EXPECT_EQ(oct_to_size(num8, test_size), 8);
  EXPECT_EQ(oct_to_size(num9, test_size), 9);
  EXPECT_EQ(oct_to_size(num15, test_size), 15);
  EXPECT_EQ(oct_to_size(num16, test_size), 16);
  EXPECT_EQ(oct_to_size(num63, test_size), 63);
  EXPECT_EQ(oct_to_size(num64, test_size), 64);
  
  test_size = buf_size - 1;
  EXPECT_EQ(oct_to_size(num0, test_size), 0);
  EXPECT_EQ(oct_to_size(num1, test_size), 1);
  EXPECT_EQ(oct_to_size(num2, test_size), 2);
  EXPECT_EQ(oct_to_size(num7, test_size), 7);
  EXPECT_EQ(oct_to_size(num8, test_size), 8);
  EXPECT_EQ(oct_to_size(num9, test_size), 9);
  EXPECT_EQ(oct_to_size(num15, test_size), 15);
  EXPECT_EQ(oct_to_size(num16, test_size), 16);
  EXPECT_EQ(oct_to_size(num63, test_size), 63);
  EXPECT_EQ(oct_to_size(num64, test_size), 64);

  char num_invalid[buf_size] = "000900";
  EXPECT_EQ(oct_to_size(num_invalid, test_size), -1);
}

struct SUT_READ_TAR : public ::testing::Test {
  static std::vector<Tar*> single_file_vec_;
  static Tar single_file_0byte_;
  static Tar single_file_1byte_;
  static Tar single_file_512byte_;
  static Tar single_file_513byte_;
  static Tar single_file_1024byte_;
  static Tar single_file_1025byte_;
  static Tar invalid_file_small_that_512;
  static Tar koread_file_name;
  static Tar multi_file_without_dir;
  static Tar multi_file_with_dir;

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
    InitTestTarget(cwd, name_to_tar(invalid_file_small_that_512), invalid_file_small_that_512);
    {
      std::wstringstream stm;
      stm << cwd << TEST_DATA_TAR_PATH << L"한글 파일 이름.tar";
      koread_file_name.init(stm.str().c_str());      
      single_file_vec_.emplace_back(&koread_file_name);
    }
    AddTestTarget(cwd, name_to_tar(multi_file_without_dir), multi_file_without_dir);
    AddTestTarget(cwd, name_to_tar(multi_file_with_dir), multi_file_with_dir);
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
      stm << current_working_dir_path << TEST_DATA_TAR_PATH << tar_file_name;
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

std::vector<Tar*> SUT_READ_TAR::single_file_vec_;
Tar SUT_READ_TAR::single_file_0byte_;
Tar SUT_READ_TAR::single_file_1byte_;
Tar SUT_READ_TAR::single_file_512byte_;
Tar SUT_READ_TAR::single_file_513byte_;
Tar SUT_READ_TAR::single_file_1024byte_;
Tar SUT_READ_TAR::single_file_1025byte_;
Tar SUT_READ_TAR::invalid_file_small_that_512;
Tar SUT_READ_TAR::koread_file_name;
Tar SUT_READ_TAR::multi_file_without_dir;
Tar SUT_READ_TAR::multi_file_with_dir;

TEST_F(SUT_READ_TAR, FileOpenTest) {
  for(const auto it : single_file_vec_) {
    EXPECT_EQ(it->status(), TAR_ERR_CODE::OK);
  }
}

TEST_F(SUT_READ_TAR, FileSizeTest) {
  EXPECT_EQ(invalid_file_small_that_512.status(), TAR_ERR_CODE::INVALID_FILE_SIZE);
}

TEST_F(SUT_READ_TAR, TestKoreaFileName) {
  auto koread_file_name_header = koread_file_name.file_at(0);
  std::wstring str1 = to_wstr(koread_file_name_header->name);
  std::wstring str2 = L"한글 파일 이름.txt";
  EXPECT_EQ(str1, str2);
}

TEST_F(SUT_READ_TAR, SingleFileCount) {
  EXPECT_EQ(single_file_0byte_.file_count(), 1);
}

TEST_F(SUT_READ_TAR, MultiFileWithOutDirCount) {
  EXPECT_EQ(multi_file_without_dir.file_count(), 4);
}

TEST_F(SUT_READ_TAR, MultiFileWithOutDirList) {
  auto item1 = multi_file_without_dir.file_at(0);
  auto item2 = multi_file_without_dir.file_at(1);
  auto item3 = multi_file_without_dir.file_at(2);
  auto item4 = multi_file_without_dir.file_at(3);
  
  const Matcher<const std::string&> m = StartsWith("multi_file_");

  EXPECT_TRUE(m.Matches(item1->name));
  EXPECT_TRUE(m.Matches(item2->name));
  EXPECT_TRUE(m.Matches(item3->name));
  EXPECT_TRUE(m.Matches(item4->name));
}

TEST_F(SUT_READ_TAR, MultiFileWithDirCount) {
  EXPECT_EQ(multi_file_with_dir.file_count(), 4);
  EXPECT_EQ(multi_file_with_dir.dir_count(), 4);
}

TEST_F(SUT_READ_TAR, MultiFileWithDirList) {
  auto file_item1 = multi_file_with_dir.file_at(0);
  auto file_item2 = multi_file_with_dir.file_at(1);
  auto file_item3 = multi_file_with_dir.file_at(2);
  auto file_item4 = multi_file_with_dir.file_at(3);

  auto dir_item1 = multi_file_with_dir.dir_at(0);
  auto dir_item2 = multi_file_with_dir.dir_at(1);
  auto dir_item3 = multi_file_with_dir.dir_at(2);
  auto dir_item4 = multi_file_with_dir.dir_at(3);

  EXPECT_STREQ(file_item1->name, "multi_file_with_dir/multi_file_1024byte.txt");
  EXPECT_STREQ(file_item2->name, "multi_file_with_dir/dir1/multi_file_512byte.txt");
  EXPECT_STREQ(file_item3->name, "multi_file_with_dir/dir2/multi_file_513byte.txt");
  EXPECT_STREQ(file_item4->name, "multi_file_with_dir/dir2/dir3/multi_file_1025byte.txt");
  EXPECT_STREQ(dir_item1->name, "multi_file_with_dir/");
  EXPECT_STREQ(dir_item2->name, "multi_file_with_dir/dir1/");
  EXPECT_STREQ(dir_item3->name, "multi_file_with_dir/dir2/");
  EXPECT_STREQ(dir_item4->name, "multi_file_with_dir/dir2/dir3/");
}

TEST_F(SUT_READ_TAR, TarHeaderFormat) {
  // std::shared_ptr<posix_header> first_tar_header = VERY_LONG.at(0);
  // std::cout << first_tar_header->name << std::endl;
  // print_hex(first_tar_header->linkname, PH_LINKNAME_SIZE);
  // print_hex(first_tar_header->prefix, PH_PREFIX_SIZE);
  // std::cout << first_tar_header->typeflag << std::endl;
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