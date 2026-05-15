#include <algorithm>
#include <iostream>

#include "lru.cpp"

int lev(std::string s1, std::string s2)
{
    if (s1.size() == 0) return s2.size();
    if (s2.size() == 0) return s1.size();
    if (s1[0] == s2[0])
        return lev(s1.substr(1), s2.substr(1));
    return 1 + std::min({
                lev(s1.substr(1), s2),
                lev(s1, s2.substr(1)),
                lev(s1.substr(1), s2.substr(1))
            });
}

int lev_cached(
    const std::string& s1,
    const std::string& s2,
    int i,
    int j,
    LRU<int, int>& lru)
{
    int key = i * s2.size() + j;

    if (auto cached = lru.lookup(key))
        return *cached;

    int result;

    if (i == s1.size())
    {
        result = s2.size() - j;
    }
    else if (j == s2.size())
    {
        result = s1.size() - i;
    }
    else if (s1[i] == s2[j])
    {
        result = lev_cached(s1, s2, i + 1, j + 1, lru);
    }
    else
    {
        result = 1 + std::min({
            lev_cached(s1, s2, i + 1, j, lru),
            lev_cached(s1, s2, i, j + 1, lru),
            lev_cached(s1, s2, i + 1, j + 1, lru)
        });
    }

    lru.insert(key, result);

    return result;
}

int main(void)
{
    LRU<int, int> lru;
    clock_t start = clock();
    float duration = float(clock() - start) / CLOCKS_PER_SEC;
    // int d = lev("thiswillbelonger", "longerthiswillbe");
    int d = lev_cached("thiswillbelonger", "longerthiswillbe", 0, 0, lru);
    std::cout << "answer: " << d << " time: " << duration << "\n";
    return 0;
}
