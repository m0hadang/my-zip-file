#include <gtest/gtest-matchers.h>
#include <gmock/gmock-matchers.h>
#include <gmock/gmock-more-matchers.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../my_tar.h"
#include <sstream>
#include <vector>
#include <unordered_set>

using namespace mohadangkim;

#ifdef _WIN32
#define SPLIT_CHAR "\\"
#else
#define SPLIT_CHAR "/"
#endif

#define TEST_DATA_TAR_PATH  "." SPLIT_CHAR "test_data_tar" SPLIT_CHAR
#define TEST_DATA_DIR_PATH  "." SPLIT_CHAR "test_data_dir" SPLIT_CHAR

#define name_to_tar(X) L#X ## ".tar"
#define name_to_dir(X) L#X

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

  char num_invalid[buf_size] = "000900";//it not octal
  EXPECT_EQ(oct_to_size(num_invalid, test_size), -1);
}

TEST(SUT_UTILITY, convert_valid_dir_path) {
  std::wstring invalid_path = std::wstring(LR"(.\valid\dir\path)");
  std::wstring valid_path = std::wstring(LR"(.\valid\dir\path\)");

  EXPECT_EQ(convert_valid_dir_path(invalid_path), valid_path);
  EXPECT_EQ(convert_valid_dir_path(valid_path), valid_path);
}

void check_get_dir_name(
  const std::wstring& input,
  const std::wstring& ans_bas_path,
  const std::wstring& ans_name) {
  auto dir_name_info = get_dir_name_info(input);
  EXPECT_EQ(dir_name_info.base_path, ans_bas_path);
  EXPECT_EQ(dir_name_info.name, ans_name);
}

TEST(SUT_UTILITY, get_dir_name) {
  check_get_dir_name(L"", L"", L"/");
  check_get_dir_name(L"/", L"", L"/");
  check_get_dir_name(L"\\", L"", L"/");
  check_get_dir_name(L"a", L"", L"a/");
  check_get_dir_name(L"a/", L"", L"a/");
  check_get_dir_name(L"a\\", L"", L"a/");
  check_get_dir_name(L"dir_with_multi_dir_file", L"", L"dir_with_multi_dir_file/");
  check_get_dir_name(L"dir_with_multi_dir_file/", L"", L"dir_with_multi_dir_file/");
  check_get_dir_name(L"dir_with_multi_dir_file\\", L"", L"dir_with_multi_dir_file/");
  check_get_dir_name(
    L"dir_with_multi_dir_file/empty_dir", 
    L"dir_with_multi_dir_file/", L"empty_dir/");
  check_get_dir_name(
    L"dir_with_multi_dir_file\\empty_dir", 
    L"dir_with_multi_dir_file/", L"empty_dir/");  
  check_get_dir_name(
    L"dir_with_multi_dir_file/empty_dir/", 
    L"dir_with_multi_dir_file/", L"empty_dir/");    
  check_get_dir_name(
    L"dir_with_multi_dir_file\\empty_dir\\", 
    L"dir_with_multi_dir_file/", L"empty_dir/");
  check_get_dir_name(
    L"dir_with_multi_dir_file/한글", 
    L"dir_with_multi_dir_file/", L"한글/");
  check_get_dir_name(
    L"dir_with_multi_dir_file\\한글",
    L"dir_with_multi_dir_file/", L"한글/");  
  check_get_dir_name(
    L"dir_with_multi_dir_file/한글/",
    L"dir_with_multi_dir_file/", L"한글/");
  check_get_dir_name(
    L"dir_with_multi_dir_file\\한글\\",
    L"dir_with_multi_dir_file/", L"한글/");  
}

TEST(SUT_UTILITY, dir_listing) {
  std::wstringstream stm;
  stm << TEST_DATA_DIR_PATH << L"dir_with_multi_dir_file" << SPLIT_CHAR;

  auto test_dir_path = stm.str();

  std::vector<std::wstring> paths;
  
  auto dir_name_info = get_dir_name_info(test_dir_path);
  Tar::dir_listing(test_dir_path, dir_name_info.name, paths);

  std::unordered_set<std::wstring> compare_set;
  compare_set.emplace(L"dir_with_multi_dir_file/");
  compare_set.emplace(L"dir_with_multi_dir_file/empty_dir/");
  compare_set.emplace(L"dir_with_multi_dir_file/more_dir/");
  compare_set.emplace(L"dir_with_multi_dir_file/multi_file_1024byte.txt");
  compare_set.emplace(L"dir_with_multi_dir_file/multi_file_1025byte.txt");
  compare_set.emplace(L"dir_with_multi_dir_file/multi_file_512byte.txt");
  compare_set.emplace(L"dir_with_multi_dir_file/multi_file_513byte.txt");
  compare_set.emplace(L"dir_with_multi_dir_file/한글 디렉터리 이름/");
  compare_set.emplace(L"dir_with_multi_dir_file/more_dir/dir/");
  compare_set.emplace(L"dir_with_multi_dir_file/more_dir/single_file_512byte.txt");
  compare_set.emplace(L"dir_with_multi_dir_file/more_dir/single_file_513byte.txt");
  compare_set.emplace(L"dir_with_multi_dir_file/한글 디렉터리 이름/한글 파일 이름.txt");
  compare_set.emplace(L"dir_with_multi_dir_file/more_dir/dir/single_file_0byte.txt");
  compare_set.emplace(L"dir_with_multi_dir_file/more_dir/dir/single_file_1024byte.txt");
  compare_set.emplace(L"dir_with_multi_dir_file/more_dir/dir/single_file_1025byte.txt");

  EXPECT_EQ(paths.size(), compare_set.size());

  for (int i = 0; i < paths.size(); ++i) {
    auto& path = paths[i];
    if (compare_set.find(path) == compare_set.end()) {
      EXPECT_TRUE(false) << "==> cant find file : " << path << std::endl;
    }
    if (path[path.size() - 1] == L'/') {  // dir path
      for (int y = 0; y < i - 1; ++y) {
        std::wstring prev_path = paths[y];
        if (prev_path.find(path) != std::wstring::npos) {
          EXPECT_TRUE(false) << "==> invalid sequence, prev : " << prev_path
                             << ", cur : " << path << std::endl;
        }
      }
    }
  }
}

struct SUT_READ_TAR : public ::testing::Test {
  static std::vector<Tar*> tar_files;
  static Tar single_file_0byte;
  static Tar single_file_1byte;
  static Tar single_file_512byte;
  static Tar single_file_513byte;
  static Tar single_file_1024byte;
  static Tar single_file_1025byte;
  static Tar invalid_file_small_that_512;
  static Tar korea_file_name;
  static Tar multi_file_without_dir;
  static Tar multi_file_with_dir;

  static void SetUpTestCase() {
    std::wcout.imbue(std::locale( "kor" ));

    AddTestTarget(name_to_tar(single_file_0byte), single_file_0byte);
    AddTestTarget(name_to_tar(single_file_1byte), single_file_1byte);
    AddTestTarget(name_to_tar(single_file_512byte), single_file_512byte);
    AddTestTarget(name_to_tar(single_file_513byte), single_file_513byte);
    AddTestTarget(name_to_tar(single_file_1024byte), single_file_1024byte);
    AddTestTarget(name_to_tar(single_file_1025byte), single_file_1025byte);
    InitTestTarget(name_to_tar(invalid_file_small_that_512), invalid_file_small_that_512);
    AddTestTarget(L"한글 파일 이름.tar", korea_file_name);
    AddTestTarget(name_to_tar(multi_file_without_dir), multi_file_without_dir);
    AddTestTarget(name_to_tar(multi_file_with_dir), multi_file_with_dir);
  }
  virtual void SetUp() override {
  }
  virtual void TearDown() override {
  }
private:
  static void InitTestTarget(
    const wchar_t* tar_file_name,
    Tar& tar) {
      std::wstringstream stm;
      stm << TEST_DATA_TAR_PATH << tar_file_name;
      std::wcout << "test target : " << stm.str() << std::endl; 
      tar.init_tar(stm.str().c_str());
  }
  static void AddTestTarget(
    const wchar_t* tar_file_name,
    Tar& tar) {
      InitTestTarget(tar_file_name, tar);
      tar_files.emplace_back(&tar);
  }
};

std::vector<Tar*> SUT_READ_TAR::tar_files;
Tar SUT_READ_TAR::single_file_0byte;
Tar SUT_READ_TAR::single_file_1byte;
Tar SUT_READ_TAR::single_file_512byte;
Tar SUT_READ_TAR::single_file_513byte;
Tar SUT_READ_TAR::single_file_1024byte;
Tar SUT_READ_TAR::single_file_1025byte;
Tar SUT_READ_TAR::invalid_file_small_that_512;
Tar SUT_READ_TAR::korea_file_name;
Tar SUT_READ_TAR::multi_file_without_dir;
Tar SUT_READ_TAR::multi_file_with_dir;

TEST_F(SUT_READ_TAR, INIT__TEST) {
  for(const auto it : tar_files) {
    EXPECT_EQ(it->status(), TAR_ERR_CODE::OK);
  }
}

TEST_F(SUT_READ_TAR, FILE_SIZE__TEST) {
  EXPECT_EQ(invalid_file_small_that_512.status(), TAR_ERR_CODE::TAR_ERR_INVALID_FILE_SIZE);
}

TEST_F(SUT_READ_TAR, KOREA_FILE_NAME__TEST) {
  auto korea_file_name_header = korea_file_name.file_at(0);
  std::wstring str1 = to_wstr(korea_file_name_header->name);
  std::wstring str2 = L"한글 파일 이름.txt";
  EXPECT_EQ(str1, str2);
}

TEST_F(SUT_READ_TAR, SINGLE_FILE_COUNT__TEST) {
  EXPECT_EQ(single_file_0byte.file_count(), 1);
}

TEST_F(SUT_READ_TAR, MULTI_FILE_WITHOUT_DIR_COUNT__TEST) {
  EXPECT_EQ(multi_file_without_dir.file_count(), 4);
}

TEST_F(SUT_READ_TAR, MULTI_FILE_WITHOUT_DIR_LIST__TEST) {
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

TEST_F(SUT_READ_TAR, MULTI_FILE_WITH_DIR_COUNT__TEST) {
  EXPECT_EQ(multi_file_with_dir.file_count(), 4);
  EXPECT_EQ(multi_file_with_dir.dir_count(), 4);
}

TEST_F(SUT_READ_TAR, MULTI_FILE_WITH_DIR_LIST__TEST) {
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

TEST_F(SUT_READ_TAR, TAR_HEADER_FORMAT) {
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

struct SUT_WRITE_TAR : public ::testing::Test {  
  static std::vector<Tar*> tar_files;
  static Tar dir_empty;
  static Tar dir_with_multi_dir;
  static Tar dir_with_multi_dir_file;
  static Tar dir_with_single_dir;
  static Tar dir_with_single_file;
  static Tar korea_dir_name;

  static void SetUpTestCase() {
    std::wcout.imbue(std::locale( "kor" ));

    AddTestTarget(name_to_dir(dir_empty), dir_empty);
    AddTestTarget(name_to_dir(dir_with_multi_dir), dir_with_multi_dir);
    AddTestTarget(name_to_dir(dir_with_multi_dir_file), dir_with_multi_dir_file);
    AddTestTarget(name_to_dir(dir_with_single_dir), dir_with_single_dir);
    AddTestTarget(name_to_dir(dir_with_single_file), dir_with_single_file);
    AddTestTarget(L"한글 디렉터리 이름", korea_dir_name);
  }
  virtual void SetUp() override {
  }
  virtual void TearDown() override {
  }
private:
  static void InitTestTarget(
    const wchar_t* dir_name,
    Tar& tar) {
      std::wstringstream stm;
      stm << TEST_DATA_DIR_PATH << dir_name;
      std::wcout << "test target : " << stm.str() << std::endl; 
      tar.init_dir(stm.str().c_str());
  }
  static void AddTestTarget(
    const wchar_t* dir_name,
    Tar& tar) {
      InitTestTarget(dir_name, tar);
      tar_files.emplace_back(&tar);
  }
};

std::vector<Tar*> SUT_WRITE_TAR::tar_files;
Tar SUT_WRITE_TAR::dir_empty;
Tar SUT_WRITE_TAR::dir_with_multi_dir;
Tar SUT_WRITE_TAR::dir_with_multi_dir_file;
Tar SUT_WRITE_TAR::dir_with_single_dir;
Tar SUT_WRITE_TAR::dir_with_single_file;
Tar SUT_WRITE_TAR::korea_dir_name;

TEST_F(SUT_WRITE_TAR, INIT__TEST) {
  for(const auto it : tar_files) {
    EXPECT_EQ(it->status(), TAR_ERR_CODE::OK);
  }
}

TEST_F(SUT_WRITE_TAR, DIR_EMPTY__COUNT_TEST) {
  EXPECT_EQ(dir_empty.dir_count(), 0);
  EXPECT_EQ(dir_empty.file_count(), 0);
}
TEST_F(SUT_WRITE_TAR, DIR_WITH_MULTI_DIR__COUNT_TEST) {
  EXPECT_EQ(dir_with_multi_dir.dir_count(), 3);
  EXPECT_EQ(dir_with_multi_dir.file_count(), 0);  
}
// TEST_F(SUT_WRITE_TAR, DIR_WITH_MULTI_DIR_FILE__COUNT_TEST) {
//   EXPECT_EQ(dir_with_multi_dir_file.dir_count(), 0);
//   EXPECT_EQ(dir_with_multi_dir_file.file_count(), 0);  
// }
// TEST_F(SUT_WRITE_TAR, DIR_WITH_SINGLE_DIR__COUNT_TEST) {
//   EXPECT_EQ(dir_with_single_dir.dir_count(), 0);
//   EXPECT_EQ(dir_with_single_dir.file_count(), 0);  
// }
// TEST_F(SUT_WRITE_TAR, DIR_WITH_SINGLE_FILE__COUNT_TEST) {
//   EXPECT_EQ(dir_with_single_file.dir_count(), 0);
//   EXPECT_EQ(dir_with_single_file.file_count(), 0);  
// }
// TEST_F(SUT_WRITE_TAR, KOREA_DIR_NAME__COUNT_TEST) {
//   EXPECT_EQ(korea_dir_name.dir_count(), 0);
//   EXPECT_EQ(korea_dir_name.file_count(), 0);  
// }