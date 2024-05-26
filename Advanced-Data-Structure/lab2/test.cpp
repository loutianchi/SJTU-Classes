#include "src/hnsw.hpp"
#include "util/vecs_io.hpp"
#include "util/ground_truth.hpp"
#include <thread>
#include <mutex>
#include <functional>

using namespace HNSWLab;

// 任务函数，调用 query 函数
void task_function(int st, int n, int *query, unordered_map<int, vector<int>>& results, std::mutex& mtx, HNSW &hnsw, const int query_vec_dim, const int gnd_vec_dim) {

    for (int i = st; i < st + n; i++) {
        vector<int> test_gnd = hnsw.query(query + i * query_vec_dim, gnd_vec_dim);

        // 对共享数据进行操作时需要加锁
        //std::lock_guard<std::mutex> lock(mtx);
        results[i] = test_gnd;
    }
}

int main() {
    
    std::printf("load ground truth\n");
    int gnd_n_vec = 100;
    int gnd_vec_dim = 10;
    char *path = "./data/siftsmall/gnd.ivecs";
    int *gnd = read_ivecs(gnd_n_vec, gnd_vec_dim, path);

    std::printf("load query\n");
    int query_n_vec = 100;
    int query_vec_dim = 128;
    path = "./data/siftsmall/query.bvecs";
    int *query = read_bvecs(query_n_vec, query_vec_dim, path);

    std::printf("load base\n");
    int base_n_vec = 10000;
    int base_vec_dim = 128;
    path = "./data/siftsmall/base.bvecs";
    int *base = read_bvecs(base_n_vec, base_vec_dim, path);

    HNSW hnsw;

    size_t report_every = 1000;
    TimeRecord insert_record, insert_tot_record;

    for (int i = 0; i < base_n_vec; i++) {
        hnsw.insert(base + base_vec_dim * i, i);

        if (i % report_every == 0) {
            printf("single insert time %.3f ms\n", insert_record.get_elapsed_time_micro() / report_every * 1e-3);
            insert_record.reset();
        }
    }
    printf("tot average single insert time %.3f ms\n", insert_tot_record.get_elapsed_time_micro() / base_n_vec * 1e-3);

    printf("querying\n");
    vector <vector<int>> test_gnd_l;
    double single_query_time;
    TimeRecord query_record;
    for (int i = 0; i < gnd_n_vec; i++) {
        vector<int> test_gnd = hnsw.query(query + i * query_vec_dim, gnd_vec_dim);
        test_gnd_l.push_back(test_gnd);
    }
    single_query_time = query_record.get_elapsed_time_micro() / query_n_vec * 1e-3;

    double recall = count_recall(gnd_n_vec, gnd_vec_dim, test_gnd_l, gnd);
    printf("seq: average recall: %.3f, single query time %.3f ms\n", recall, single_query_time);

    // 并行部分
    unordered_map <int, vector<int>> test_gnd_l_pal;
    mutex mtx;
    vector<thread> threads;
    TimeRecord query_record_pal;
    for (int i = 0; i < threadNum; ++i) {
        threads.emplace_back(bind(task_function, gnd_n_vec / threadNum * i, 
         (i != threadNum - 1 ? gnd_n_vec / threadNum: gnd_n_vec -  gnd_n_vec / threadNum * i),
         query, ref(test_gnd_l_pal), ref(mtx), ref(hnsw), query_vec_dim, gnd_vec_dim));
    }

    for (auto& thread : threads) {
        thread.join();
    }
    single_query_time = query_record_pal.get_elapsed_time_micro() / query_n_vec * 1e-3;
    vector< vector<int> > res;
    for (int i = 0; i < gnd_n_vec; ++i)
        res.push_back(test_gnd_l_pal[i]);
    double recall_pal = count_recall(gnd_n_vec, gnd_vec_dim, res, gnd);
    printf("parallel %d : average recall: %.3f, single query time %.3f ms\n", threadNum, recall_pal, single_query_time);

    return 0;
}