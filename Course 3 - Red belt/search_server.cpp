#include "search_server.h"
#include "iterator_range.h"
#include "profile.h"

#include <algorithm>
#include <sstream>
#include <iostream>

//=====================================================================================
vector<string> SplitIntoWords(const string& line) {
  istringstream words_input(line);
  return {istream_iterator<string>(words_input), istream_iterator<string>()};
}
//=====================================================================================
//����������� ������ SearchServer ��������� ����� �����, ���������� ���� ����������
SearchServer::SearchServer(istream& document_input) {
    UpdateDocumentBase(document_input);
}
//=====================================================================================
SearchServer::~SearchServer() {
    for (auto& f : futures) { f.get(); }
}
//=====================================================================================
void InvertedIndex::FillToIndex() {
    for (const auto& word : index_fill) {
        vector<pair<size_t, size_t>> tmp(word.second.begin(), word.second.end());
        index[word.first] = move(tmp);        
    }    
}
//=====================================================================================
void SearchServer::UpdateStream(istream& document_input) {
    // ����� ������ InvertedIndex
    InvertedIndex new_index;
    // � ���� ����������� ������ �� ���������
    for (string current_document; getline(document_input, current_document); ) {
        new_index.Add(move(current_document));
    }
    // ������� �������� �� ���������� ������� index_fill � ������ index
    new_index.FillToIndex();     
    // ���� ��� ���������
    unique_lock<shared_mutex> ul(door);
    swap(index, new_index);
    index.SetBaseLoaded();
}
//=====================================================================================
void SearchServer::StreamSeparator(istream& input_stream, ostream& output_stream, bool update_flag) {    
    //���� ���� ���������� �� ��������� - ������ ����������� ���������
    if (!index.GetBaseLoaded()) {
        if (update_flag) {
            UpdateStream(input_stream);
        } else {
            QueriesStream(input_stream, output_stream);
        }
    // ����� - ����������
    } else {
        if (update_flag) {
            futures.push_back(async(launch::async, &SearchServer::UpdateStream,
                this, ref(input_stream)));
        } else {
            futures.push_back(async(launch::async, &SearchServer::QueriesStream,
                this, ref(input_stream), ref(output_stream)));
        }
    }  
}
//=====================================================================================
void SearchServer::QueriesStream(istream& query_input, ostream& search_results_output) {
    size_t count_doc_inbase = index.GetCountDocs();

    //������ �� ���: <������ ��������� � ������� ����������, ���������� ��������� ����� � ��������>
    //������������� ��� �������� � ���������� ����������� ������ � ����������� ������� ���������
    vector<pair<size_t, size_t>> search_results(count_doc_inbase, { 0, 0 });

    for (string current_query; getline(query_input, current_query); ) {
        //��������� ������ ������� �� ������ ���� (�����)
        const auto words = SplitIntoWords(current_query);
        {
            shared_lock<shared_mutex> sl(door);    
                        
            for (const auto& word : words) {
                //������� ����� � ������� index � �������� ������ ���������� ��� ��� ����
                //�������� �� ����� ������� � ������� ���������� ��������� ����� � ������ ���������
                for (const pair<size_t, size_t> docid : index.Lookup(word)) {
                    search_results[docid.first].first = docid.first;
                    search_results[docid.first].second += docid.second;
                }
            }
        
            // ��������� ��� ���������� 5 �������� ����������� ��������
            //   � ��������� �� ������ ���� �� ������
            auto count_sort = min(5, int(search_results.size()));
            partial_sort(
                search_results.begin(),
                search_results.begin() + count_sort,
                search_results.end(),
                [](pair<size_t, size_t> lhs, pair<size_t, size_t> rhs) {
                    int64_t lhs_docid = lhs.first;
                    auto lhs_hit_count = lhs.second;
                    int64_t rhs_docid = rhs.first;
                    auto rhs_hit_count = rhs.second;
                    return make_pair(lhs_hit_count, -lhs_docid) > make_pair(rhs_hit_count, -rhs_docid);
                }
            );      
            
            //������� � ����� ������ 5 ������� �������
            search_results_output << current_query << ':';
            for (auto [docid, hitcount] : Head(search_results, 5)) {
                if (hitcount != 0) {
                    search_results_output << " {"
                        << "docid: " << docid << ", "
                        << "hitcount: " << hitcount << '}';
                }
            }            
            search_results_output << endl;
        }       
        //������ ������
        search_results.assign(count_doc_inbase, { 0,0 });
    }
}
//=====================================================================================
void SearchServer::UpdateDocumentBase(istream& document_input) {
    ostringstream empty_output;
    StreamSeparator(document_input, empty_output, true);
}
//=====================================================================================
void SearchServer::AddQueriesStream(istream& query_input, ostream& search_results_output) {
    StreamSeparator(query_input, search_results_output, false);
}
//=====================================================================================
void InvertedIndex::Add(const string& document) {    
    //unordered_map<string, unordered_map<size_t, size_t>> index_fill;
    //vector<string> docs;
    docs.push_back(document);

    const size_t docid = docs.size() - 1;
    for (const auto& word : SplitIntoWords(document)) {        
        index_fill[word][docid]++;
    }
}
//=====================================================================================
const vector<pair<size_t, size_t>>& InvertedIndex::Lookup(const string& word) const {
  if (auto it = index.find(word); it != index.end()) {
    return it->second;
  } else {
    return empty;
  }
}
