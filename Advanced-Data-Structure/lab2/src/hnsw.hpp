#pragma once
#include <iostream>
#include "base.hpp"
#include <vector>
#include <cstring>

#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <random>
#include <cassert>
#include <queue>

#include "../util/parameter.hpp"
#include "../util/util.hpp"

using namespace std;

namespace HNSWLab {
    const int len = 128;
    const int m_L = 4;

    class HNSW : public AlgorithmInterface {
        struct node{
            int label;
            const int *item;
            int level;
            unordered_map<int, vector<node*> > edges;
            
            node(const int *_item, const int _label, const int _level)
            {
                label = _label;
                item = _item;
                level = _level;
            }
        };

        node *entrance;
        std::vector<node *> search_layer(node *q, node *ep, int ef, int lc);
    public:
        HNSW() {
            entrance = nullptr;
        }

        void insert(const int *item, int label);

        std::vector<int> query(const int *query, int k);

        ~HNSW() {}
    };

    // aid function
    std::vector<HNSW::node*> HNSW::search_layer(node *q, node *ep, int ef, int lc)
    {
        unordered_set<node *> visited;
        visited.insert(ep);
        priority_queue< pair<long, node*> > C;
        priority_queue< pair<long, node*> > W;
        long dis = l2distance(q->item, ep->item, len);
        C.push(make_pair(-dis, ep));
        W.push(make_pair(dis, ep));

        pair<long, node*> now;
        while (C.size() > 0) {
            now = C.top(); C.pop();
            if (!W.empty() && W.top().first < -now.first)
                break;
            
            for (int i = 0; i < now.second->edges[lc].size(); i++) {
                node* cur = now.second->edges[lc][i];
                auto it = visited.find(cur);
                if (it != visited.end()) continue;
                visited.insert(cur);
                dis = l2distance(cur->item, q->item, len);
                if (dis < W.top().first || W.size() < ef_search) {
                    C.push(make_pair(-dis, cur));
                    W.push(make_pair(dis, cur));
                    if (W.size() > ef_search)
                        W.pop();
                }
            }
        }
        vector<node*> w;
        while (!W.empty())
        {
            w.push_back(W.top().second);
            W.pop();
        }
        return w;
    }

    /**
     * input:
     * iterm: the vector to be inserted
     * label: the label(id) of the vector
    */
    void HNSW::insert(const int *item, int label) {
        int level = min(get_random_level(), m_L - 1);
        node* now = new node(item, label, level);
        if (entrance == nullptr) {
            entrance = now;
            return;
        }

        // find a suitable place
        node *ep = entrance;
        int maxL = ep->level;

        vector<node*> W;
        for (int i = maxL; i > level; i--) {
            W = search_layer(now, ep, 1, i);
            ep = W[W.size() - 1];
        }
        
        for (int lc = min(maxL, level); lc >= 0; lc--) {
            W = search_layer(now, ep, ef_construction, lc);

            // add edges
            for (int i = W.size() - 1, j = M; i >= 0 && j > 0; --i, --j) {
                now->edges[lc].push_back(W[i]);
                if (W[i]->edges[lc].size() == M_max) {
                    int index = -1;
                    long maxD = l2distance(now->item, W[i]->item, len), curD;
                    for (int k = 0; k < W[i]->edges[lc].size(); k++)
                        if ((curD = l2distance(W[i]->edges[lc][k]->item, W[i]->item, len)) > maxD) {
                            maxD = curD;
                            index = k;
                        }
                    if (index != -1)
                        W[i]->edges[lc][index] = now;
                }
                else
                    W[i]->edges[lc].push_back(now);
            }
            ep = W[W.size() - 1];
        }

        if (level > maxL)
            entrance = now;
    }

    /**
     * input:
     * query: the vector to be queried
     * k: the number of nearest neighbors to be returned
     * 
     * output:
     * a vector of labels of the k nearest neighbors
    */
    std::vector<int> HNSW::query(const int *query, int k) {
        std::vector<int> res;
        
        node *ep = entrance;
        int maxL = ep->level;

        vector<node*> W;
        node *now = new node(query, -1, 0);
        for (int i = maxL; i > 0; i--) {
            W = search_layer(now, ep, 1, i);
            ep = W[W.size() - 1];
        }
        
        W = search_layer(now, ep, ef_search, 0);

        for (int i = W.size() - 1, j = k; i >= 0, j >= 1; i--, j--)
            res.push_back(W[i]->label);

        delete now;

        return res;
    }

}