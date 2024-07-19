/* vim: set tabstop=4 shiftwidth=4 softtabstop=4 expandtab smarttab : */
/**
 * @file {file}
 * @author Soo Han, Kim (princeb612.kr@gmail.com)
 * @desc
 *
 * Revision History
 * Date         Name                Description
 *
 */

#ifndef __HOTPLACE_SDK_BASE_NOSTD_PATTERN__
#define __HOTPLACE_SDK_BASE_NOSTD_PATTERN__

#include <functional>
#include <map>
#include <queue>
#include <sdk/base/error.hpp>
#include <sdk/base/syntax.hpp>
#include <sdk/base/types.hpp>
#include <set>
#include <string>
#include <vector>

namespace hotplace {

// Data Structures & Algorithms 12.3.2 The Boyer-Moore Algorithm
//  O(nm+|Sigma|)
//  Algorithm BMMatch(T,P):
//      Input: Strings T (text) with n characters and P (pattern) with m characters
//      Output: Starting index of the first substring of T matching P, or an indication
//              that P is not a substring of T
//      compute function last
//      i←m−1
//      j←m−1
//      repeat
//          if P[ j] = T[i] then
//              if j = 0 then
//                  return i {a match!}
//              else
//                  i←i−1
//                  j ← j−1
//          else
//              i←i+m−min(j,1+last(T[i])) {jump step}
//              j ←m−1
//      until i > n−1
//      return “There is no substring of T matching P.”

// Data Structures & Algorithms 12.3.3 The Knuth-Morris-Pratt Algorithm
//  O(n+m)
//
//  Algorithm KMPMatch(T,P):
//      Input: Strings T (text) with n characters and P (pattern) with m characters
//      Output: Starting index of the first substring of T matching P, or an indication
//              that P is not a substring of T
//      f ←KMPFailureFunction(P) {construct the failure function f for P}
//      i←0
//      j←0
//      while i < n do
//          if P[ j] = T[i] then
//              if j = m−1 then
//                  return i−m+1 {a match!}
//              i←i+1
//              j ← j+1
//          else if j > 0 {no match, but we have advanced in P} then
//              j ← f ( j−1) { j indexes just after prefix of P that must match}
//          else
//              i←i+1
//      return “There is no substring of T matching P.”
//
//  Algorithm KMPFailureFunction(P):
//      Input: String P (pattern) with m characters
//      Output: The failure function f for P, which maps j to the length of the longest
//              prefix of P that is a suffix of P[1.. j]
//          i←1
//          j←0
//          f (0)←0
//          while i < m do
//              if P[ j] = P[i] then
//                  {we have matched j+1 characters}
//                  f (i)← j+1
//                  i←i+1
//                  j ← j+1
//              else if j > 0 then
//                  { j indexes just after a prefix of P that must match}
//                  j ← f ( j−1)
//              else
//                  {we have no match here}
//                  f (i)←0
//                  i←i+1

template <typename T = char>
class t_kmp_pattern {
   public:
    /**
     * comparator for pointer type - t_kmp_pattern<object*>
     *
     * struct object {
     *      int value;
     *      friend bool operator==(const object& lhs, const object& rhs) { return lhs.value == rhs.value; }
     * }
     * auto comparator = [](const object* lhs, const object* rhs) -> bool {
     *      return (lhs->value == rhs->value);
     * };
     *
     * std::vector<objec*> data1; // 1 2 3 4 5 by new object
     * std::vector<objec*> data2; // 3 4 by new object
     *
     * t_kmp_pattern<object*> search;
     * search.match(data1, data2);
     *      // if (pattern[j] == data[i]) - incorrect
     *      // return -1
     *
     * search.match(data1, data2, 0, comparator);
     *      // if (comparator(pattern[j], data[i])) - correct
     *      // return 2
     */
    typedef typename std::function<bool(const T&, const T&)> comparator_t;

    t_kmp_pattern() {}

    int match(const std::vector<T>& data, const std::vector<T>& pattern, unsigned int pos = 0, comparator_t comparator = nullptr) {
        return match(&data[0], data.size(), &pattern[0], pattern.size(), pos, comparator);
    }

    /**
     * @brief   match
     * @return  index, -1 (not found)
     */
    int match(const T* data, size_t size_data, const T* pattern, size_t size_pattern, unsigned int pos = 0, comparator_t comparator = nullptr) {
        int ret = -1;
        if (data && pattern && size_pattern) {
            unsigned int n = size_data;
            unsigned int m = size_pattern;
            std::vector<int> fail = failure(pattern, m, comparator);
            unsigned int i = pos;
            unsigned int j = 0;
            while (i < n) {
                bool test = false;
                if (comparator) {
                    test = comparator(pattern[j], data[i]);
                } else {
                    test = (pattern[j] == data[i]);
                }
                if (test) {
                    if (j == m - 1) {
                        ret = i - m + 1;
                        break;
                    }
                    i++;
                    j++;
                } else if (j > 0) {
                    j = fail[j - 1];
                } else {
                    i++;
                }
            }
        }
        return ret;
    }

   protected:
    std::vector<int> failure(const T* pattern, size_t size, comparator_t comparator = nullptr) {
        std::vector<int> fail(size);
        fail[0] = 0;
        size_t m = size;
        size_t j = 0;
        size_t i = 1;
        while (i < m) {
            bool test = false;
            if (comparator) {
                test = comparator(pattern[j], pattern[i]);
            } else {
                test = (pattern[j] == pattern[i]);
            }
            if (test) {
                fail[i] = j + 1;
                i++;
                j++;
            } else if (j > 0) {
                j = fail[j - 1];
            } else {
                fail[i] = 0;
                i++;
            }
        }
        return fail;
    }
};

/**
 * @brief   return array[index]
 * @sa      t_trie, t_aho_corasick
 */
template <typename BT = char, typename T = BT>
BT memberof_defhandler(const T* source, size_t idx) {
    return source ? source[idx] : BT();
}

/**
 * @brief   Trie Data Structure
 *          A Trie, also known as a prefix tree, is a tree-like data structure used to store a dynamic set of strings.
 * @refer   https://www.geeksforgeeks.org/trie-data-structure-in-cpp/
 *          https://www.geeksforgeeks.org/auto-complete-feature-using-trie/
 * @sample
 *          t_trie<char> trie;
 *          trie.add("hello", 5).add("dog", 3).add("help", 4);
 *          result = trie.search("hello", 5); // true
 *          result = trie.prefix("he", 2); // true
 *          auto handler = [](const char* p, size_t size) -> void {
 *              if (p) {
 *                  printf("%.*s\n", (unsigned)size, p);
 *              }
 *          };
 *          trie.dump(handler); // dog, hello, help
 *          result = trie.suggest("he", 2, handler); // hello, help
 *          trie.erase("help", 4);
 *          result = trie.search("help", 4); // false
 */
template <typename BT = char, typename T = BT>
class t_trie {
   public:
    typedef typename std::function<BT(const T* source, size_t idx)> memberof_t;
    typedef typename std::function<void(const BT* t, size_t size)> dump_handler;

    /**
     * @brief   trie node structure
     */
    struct trienode {
        std::map<BT, trienode*> children;
        bool eow;  // end of  word

        trienode() : eow(false) {}
        ~trienode() {
            for (auto item : children) {
                delete item.second;
            }
        }
        bool islast() { return children.empty(); }
    };

    t_trie(memberof_t memberof = memberof_defhandler<BT, T>) : _root(new trienode), _memberof(memberof) {}
    virtual ~t_trie() { delete _root; }

    t_trie<BT, T>& add(const T* pattern, size_t size) {
        if (pattern) {
            trienode* current = _root;
            for (size_t i = 0; i < size; ++i) {
                const BT& t = _memberof(pattern, i);
                trienode* child = current->children[t];
                if (nullptr == child) {
                    child = new trienode;
                    current->children[t] = child;
                }
                current = child;
            }
            current->eow = true;
        }

        return *this;
    }
    bool search(const T* pattern, size_t size) {
        bool ret = false;
        if (pattern) {
            trienode* current = _root;
            for (size_t i = 0; i < size; ++i) {
                const BT& t = _memberof(pattern, i);
                trienode* child = current->children[t];
                if (nullptr == child) {
                    return false;
                }
                current = child;
            }
            ret = current->eow;
        }
        return ret;
    }
    bool prefix(const T* pattern, size_t size) {
        bool ret = true;
        if (pattern) {
            trienode* current = _root;
            for (size_t i = 0; i < size; ++i) {
                const BT& t = _memberof(pattern, i);
                trienode* child = current->children[t];
                if (nullptr == child) {
                    return false;
                }
                current = child;
            }
        }
        return ret;
    }
    void erase(const T* pattern, size_t size) {
        if (pattern) {
            trienode* current = _root;
            for (size_t i = 0; i < size; ++i) {
                const BT& t = _memberof(pattern, i);
                trienode* child = current->children[t];
                if (nullptr == child) {
                    return;
                }
                current = child;
            }
            if (current->eow) {
                current->eow = false;
            }
        }
    }

    t_trie<BT, T>& reset() {
        if (_root.children.size()) {
            delete _root;
            _root = new trienode;
        }
        return *this;
    }

    bool suggest(const T* pattern, size_t size, dump_handler handler) {
        bool ret = true;
        if (pattern && handler) {
            trienode* current = _root;
            for (size_t i = 0; i < size; ++i) {
                const BT& t = _memberof(pattern, i);
                trienode* child = current->children[t];
                if (nullptr == child) {
                    return false;
                }
                current = child;
            }
            if (current->islast()) {
                handler(pattern, size);
            } else {
                dump(current, pattern, size, handler);
            }
        } else {
            ret = false;
        }
        return ret;
    }

    void dump(dump_handler handler) const { dump(_root, nullptr, 0, handler); }

   protected:
    void dump(trienode* node, const T* pattern, size_t size, dump_handler handler) const {
        if (node && handler) {
            if (node->eow) {
                handler(pattern, size);
            }
            for (auto item : node->children) {
                std::vector<BT> v;
                v.insert(v.end(), pattern, pattern + size);
                v.insert(v.end(), item.first);
                dump(item.second, &v[0], v.size(), handler);
            }
        }
    }

   private:
    trienode* _root;
    memberof_t _memberof;
};

/**
 * @brief   suffix trie
 * @refer   https://www.geeksforgeeks.org/pattern-searching-using-trie-suffixes/
 * @sample
 *          // construct
 *          t_suffixtree<char> suffixtree("geeksforgeeks.org", 17);
 *          std::set<unsigned> result = suffixtree.search("ee", 2);       // 1, 9
 *          std::set<unsigned> result = suffixtree.search("geek", 4);     // 0, 8
 *          std::set<unsigned> result = suffixtree.search("quiz", 4);     // not found
 *          std::set<unsigned> result = suffixtree.search("forgeeks", 8); // 5
 *
 *          //
 *          t_suffixtree<char> suffixtree;
 *          suffixtree.add("geeksforgeeks.org", 17);
 *          std::set<unsigned> result = suffixtree.search("ee", 2);       // 1, 9
 */
template <typename BT = char, typename T = BT>
class t_suffixtree {
   public:
    typedef typename std::function<BT(const T* source, size_t idx)> memberof_t;

    struct trienode {
        std::map<BT, trienode*> children;
        std::set<unsigned> index;

        trienode() {}
        ~trienode() {
            for (auto item : children) {
                delete item.second;
            }
        }
    };

    t_suffixtree(memberof_t memberof = memberof_defhandler<BT, T>) : _root(new trienode), _memberof(memberof) {}
    t_suffixtree(const T* pattern, size_t size, memberof_t memberof = memberof_defhandler<BT, T>) : _root(new trienode), _memberof(memberof) {
        add(pattern, size);
    }
    virtual ~t_suffixtree() { delete _root; }

    std::set<unsigned> search(const T* pattern, size_t size) {
        std::set<unsigned> index;
        if (pattern) {
            trienode* current = _root;
            for (size_t i = 0; i < size; ++i) {
                const BT& t = _memberof(pattern, i);
                auto item = current->children.find(t);
                if (current->children.end() == item) {
                    return std::set<unsigned>();
                }
                current = item->second;
            }
            for (auto item : current->index) {
                index.insert(item - size + 1);
            }
        }
        return index;
    }

    t_suffixtree<BT, T>& add(const T* pattern, size_t size) {
        if (pattern) {
            for (size_t i = 0; i < size; ++i) {
                const BT& t = _memberof(pattern, i);
                _source.insert(_source.end(), t);
            }
            size_t size_source = _source.size();
            for (size_t i = 0; i < size_source; ++i) {
                add(&_source[i], size_source - i, i);
            }
        }
        return *this;
    }

    t_suffixtree<BT, T>& reset() {
        if (_root->children.size()) {
            delete _root;
            _root = new trienode;
        }
        return *this;
    }

   protected:
    void add(const BT* pattern, size_t size, unsigned idx) {
        if (pattern) {
            trienode* current = _root;
            for (size_t i = 0; i < size; ++i) {
                const BT& t = pattern[i];
                trienode* child = current->children[t];
                if (nullptr == child) {
                    child = new trienode;
                    current->children[t] = child;
                }
                current = child;
                current->index.insert(idx + i);
            }
        }
    }

   private:
    trienode* _root;
    std::vector<BT> _source;
    memberof_t _memberof;
};

/**
 * @brief   Aho-Corasick algorithm
 * @remarks
 *          multiple-patterns
 *              KMP O(n*k + m)
 *              Aho-Corasick O(n + m + z) ; z count of matches
 * @refer   https://www.javatpoint.com/aho-corasick-algorithm-for-pattern-searching-in-cpp
 * @sample
 *          // search
 *          {
 *              t_aho_corasick ac;
 *              ac.insert("abc", 3).insert("ab", 2).insert("bc", 2).insert("a", 1);
 *              ac.build_state_machine();
 *              const char* text = "abcaabc";
 *              std::multimap<unsigned, size_t> result;
 *              result = ac.search(text, strlen(text));
 *              for (auto item : result) {
 *                  _logger->writeln("pattern[%i] at [%zi]", item.first, item.second);
 *              }
 *          }
 *          // using pointer
 *          {
 *              struct token { int type; };
 *              // lambda conversion - const T* to T* const*
 *              auto memberof = [](token* const* source, size_t idx) -> int {
 *                  const token* p = source[idx];
 *                  return p->type;
 *              };
 *              t_aho_corasick<int, token*> ac(memberof);
 *          }
 */
template <typename BT = char, typename T = BT>
class t_aho_corasick {
   public:
    typedef typename std::function<BT(const T* source, size_t idx)> memberof_t;

    /**
     * @brief   trie node structure
     */
    struct trienode {
        std::map<BT, trienode*> children;
        trienode* fail;
        std::vector<int> output;

        trienode() : fail(nullptr) {}
        ~trienode() {
            for (auto item : children) {
                delete item.second;
            }
        }
    };

   public:
    t_aho_corasick(memberof_t memberof = memberof_defhandler<BT, T>) : _root(new trienode), _memberof(memberof) {}
    ~t_aho_corasick() { delete _root; }

    /**
     * @brief   insert a pattern into the trie
     */
    t_aho_corasick<BT, T>& insert(const std::vector<T>& pattern) { return insert(&pattern[0], pattern.size()); }
    t_aho_corasick<BT, T>& insert(const T* pattern, size_t size) {
        if (pattern) {
            trienode* current = _root;
            std::vector<T> p;

            p.insert(p.end(), pattern, pattern + size);

            for (size_t i = 0; i < size; ++i) {
                const BT& t = _memberof(pattern, i);
                trienode* child = current->children[t];
                if (nullptr == child) {
                    child = new trienode;
                    current->children[t] = child;
                }
                current = child;
            }

            size_t index = _patterns.size();
            current->output.push_back(index);
            _patterns.insert({index, std::move(p)});
        }
        return *this;
    }
    /**
     * @brief   build the Aho-Corasick finite state machine
     */
    void build_state_machine() {
        std::queue<trienode*> q;

        // set failure links
        for (auto& pair : _root->children) {
            pair.second->fail = _root;
            q.push(pair.second);
        }

        // Breadth-first traversal
        while (false == q.empty()) {
            trienode* current = q.front();
            q.pop();

            for (auto& pair : current->children) {
                const BT& key = pair.first;
                trienode* child = pair.second;

                q.push(child);

                trienode* failNode = current->fail;
                while (failNode && !failNode->children[key]) {
                    failNode = failNode->fail;
                }

                child->fail = failNode ? failNode->children[key] : _root;

                // Merge output lists
                child->output.insert(child->output.end(), child->fail->output.begin(), child->fail->output.end());
            }
        }
    }

    /**
     * @brief   search for patterns
     * @return  std::multimap<unsigned, size_t> as is multimap<pattern_id, position>
     */
    std::multimap<unsigned, size_t> search(const std::vector<T>& source) { return search(&source[0], source.size()); }
    std::multimap<unsigned, size_t> search(const T* source, size_t size) {
        std::multimap<unsigned, size_t> result;
        if (source) {
            trienode* current = _root;
            for (size_t i = 0; i < size; ++i) {
                const BT& t = _memberof(source, i);
                while (current && (nullptr == current->children[t])) {
                    current = current->fail;
                }
                if (current) {
                    current = current->children[t];
                    for (auto v : current->output) {
                        // v is index of pattern
                        // i is end position of pattern
                        // (i - sizepat + 1) is beginning position of pattern
                        size_t sizepat = _patterns[v].size();
                        size_t pos = i - sizepat + 1;
                        result.insert({v, pos});
                        // debug
                        // printf("pattern:%i at [%zi] pattern [%.*s] \n",  v, pos, (unsigned)sizepat, &(_patterns[v])[0]);
                    }
                } else {
                    current = _root;
                }
            }
        }
        return result;
    }
    const std::vector<T>& get_patterns(size_t index) { return _patterns[index]; }

    void reset() {
        delete _root;
        _root = new trienode;
        _patterns.clear();
    }

   private:
    trienode* _root;
    std::map<size_t, std::vector<T>> _patterns;
    memberof_t _memberof;
};

}  // namespace hotplace

#endif
