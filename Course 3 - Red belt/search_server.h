#pragma once

#include "iterator_range.h"

#include <istream>
#include <ostream>
#include <iterator>
#include <set>
#include <vector>
#include <map>
#include <unordered_map>
#include <string>
#include <shared_mutex>
#include <future>
#include <functional>

using namespace std;

//=====================================================================================
class InvertedIndex {
public:
  void Add(const string& document);
  const vector<pair<size_t, size_t>>& Lookup(const string& word) const;

  const string& GetDocument(size_t id) const {
    return docs[id];
  }
  size_t GetCountDocs() {
      return docs.size();
  }
  void FillToIndex();  // ������� �������� index_fill � index
  void SetBaseLoaded() {
      BaseLoaded = true;
  }
  bool GetBaseLoaded() {
      return BaseLoaded;
  }

private:
    //���� - �����, ������� ���� � ���������
    //�������� - ������ �� ���: 
    //  <������ ��������� � ������� ����������, ���������� ��������� ����� � ��������>
    unordered_map<string, vector<pair<size_t, size_t>>> index;

    //��������� ������� ������������ ��� ���������� ���� ����������
    //��� ������������ ���-�� ������ ������������� ��������
    unordered_map<string, unordered_map<size_t, size_t>> index_fill;

    //� ������� - ��� ��������� (������ - ��������)
    vector<string> docs; 

    //������ - ��������
    vector<pair<size_t, size_t>> empty;
    bool BaseLoaded = false;
};
//=====================================================================================
class SearchServer {
public:
  SearchServer() = default;
  explicit SearchServer(istream& document_input);
  ~SearchServer();
  //--------------------------------------------- ���������� ������
  void UpdateStream(istream& document_input);
  void QueriesStream(istream& query_input, ostream& search_results_output);
  void StreamSeparator(istream& input_stream, ostream& output_stream, bool update_flag);
  //--------------------------------------------- ������� ���������
  void UpdateDocumentBase(istream& document_input);
  void AddQueriesStream(istream& query_input, ostream& search_results_output);
  //--------------------------------------------- 
  vector<future<void>> futures;

private:  
  InvertedIndex index;
  shared_mutex door;  
};
//=====================================================================================