/*
 * @Author: XiaChuerwu 1206575349@qq.com
 * @Date: 2023-09-06 21:34:29
 * @LastEditors: XiaChuerwu 1206575349@qq.com
 * @LastEditTime: 2023-09-07 10:17:50
 * @FilePath: \SkipList\main.cpp
 */
#ifndef __SKIPLIST_H__
#define __SKIPLIST_H__

#include <iostream> 
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <mutex>
#include <sstream> 
#include <fstream>

// File Save Path 
const char* STORE_FILE = "./file/dumpFile.txt";
const char* delimiter = "->";
// 使用锁来对跳表临界资源的操作进行保护
std::mutex mtx;     

/* 元素节点的类模板 */
template <class K, class V>
class Node {

public:
    /*默认构造 构造 与 析构函数*/
    Node() {}
    Node(K key, V value, int level);
    ~Node();

    /*获取键值对 K V */
    K getKey() const;
    V getValue() const;

    /*设置值 V */
    void setValue(const V &value);

public:
/*                                  eg.1    forward数组保存下一层的头节点指针

        Level 2:  4 ----------------------> 8 ----------------------> 12 ----------------------> nullptr
        Level 1:  4 ----------> 6 ----------> 8 ----------> 9 ----------> 10 ----------> 12 ----------> 14 ----------> nullptr
        Level 0:  4 --> 5 --> 6 --> 7 --> 8 --> 9 --> 10 --> 11 --> 12 --> 13 --> 14 --> 15 --> nullptr

        也就是说在 Level 1中 节点8 的 forward[0]保存的是 Level 0 中的 节点9，也就是它下一层的头节点指针
*/
    Node<K,V> **forward;

    int node_level;


private:
    K key;
    V value;
};

/* Node节点的构造函数 */
template <typename K, typename V>
Node<K,V>::Node(K key, V value, int level) {
    this->key = key;;
    this->value = value;
    this->node_level = level;

    //  例如，如果节点的层级数为2，那么level的值为2，但是数组的索引范围是0到2，共3个元素。所以需要将数组的大小设置为level+1，即3。
    //  在 eg.1 中 level 2 的 forword[2] 等于 level 0 的下一层
    this->forward = new Node<K,V>*[this->node_level + 1];

    //  初始化数组
    memset(this->forward, 0, sizeof(Node<K,V>*) * this->node_level + 1);
}

/* Node节点的析构函数 */
template <typename K, typename V>
Node<K,V>::~Node() {
    delete [] this->forward;
}

/*获取键值对 K V */
template <typename K, typename V>
K Node<K,V>::getKey() const {
    return key;
}

template <typename K, typename V>
V Node<K,V>::getValue() const {
    return value;
}

template <typename K, typename V>
void Node<K,V>::setValue(const V &value) {
    this->value = value;
}

/* 跳表类模板 */
template <typename K,typename V>
class SkipList {

public:
    /* 构造 与 析构函数 */
    SkipList(int max_level);
    ~SkipList();

    /* 增删改查 */
    Node<K,V> *creatNode(K key, V value, int level);
    bool insertElement(K key, V value);
    bool removeElement(K key);
    bool searchElement(K key) const;

    /* 生成随机层数 */
    int getRandomLevel() const;

    /* 落盘与加载 */
    void dumpFile();
    void loadFile();

    /* 获取当前跳表大小 */
    int getSize() const;

    /* 展示跳表 */
    void showSkipList() const;

private:
    void get_KV_from_string(std::string &line, std::string *key, std::string *value);

    bool isExist(Node<K,V> *node) const;
private:
    // 该跳表的最大层级
    int maxLevel;

    // 跳表当前层级
    int currentLevel;

    // 跳表的头节点 
    Node<K,V> *head;

    // 跳表当前的元素个数
    int elementCount;

    // 文件读写流
    std::ofstream fileWriter;
    std::ifstream fileReader;
};

/* 构造 与 析构函数 */
template <typename K, typename V>
SkipList<K,V>::SkipList(int max_level) {
    this->maxLevel = max_level;
    this->elementCount = 0;;
    this->currentLevel = 0;

    // 创建头节点，键值都为空
    K key{};
    V value{};
    this->head = new Node<K,V>(key, value, max_level);
}

template <typename K, typename V>
SkipList<K,V>::~SkipList() {
    // 首先关闭读写流
    if (this->fileReader.is_open()) {
        fileReader.close();
    }
    if (this->fileWriter.is_open()) {
        fileWriter.close();
    }

    delete this->head;
}

/* 增删改查 */
template <typename K, typename V>
Node<K, V> *SkipList<K, V>::creatNode(K key, V value, int level) {
    Node<K,V> *node = new Node<K,V>{key,value,level};
    return node;
}

template <typename K, typename V>
bool SkipList<K, V>::insertElement(K key, V value) {
    // 上锁
    mtx.lock();

    // 
    Node<K,V> *current = this->head;

    // 定义一个 update 数组用来存放 current->forward[i]操作之后的元素
    Node<K, V> *update[this->maxLevel + 1];
    memset(update, 0, sizeof(Node<K,V> *) * this->maxLevel + 1);

    // 跳表由高到低进行遍历找到需要插入的位置
    for (int i = this->currentLevel; i >= 0; i--) {
        // 当当前层级的下一个节点不为空且该key小于要插入的key就往右遍历
        while (current->forward[i] != nullptr && current->forward[i]->getKey() < key) {
            current = current->forward[i];
        }
        // 否则就将该节点放入需要进行更新的数组 update 中
        update[i] = current;
    }

    // 当到达 level 0 层级时，将指针指向右节点，也就是希望插入键的节点
    current = current->forward[0];

    // 如果当前节点的键等于搜索到的键，那么证明已经存在该节点了
    if (current != nullptr && current->getKey() == key) {
        current->setValue(value);
        mtx.unlock();
        return true;
    }

    // 如果 current 为 NULL，则表示我们已到最底层
    // 如果 current 的键不等于 key，这意味着我们必须在 update[0] 和当前节点之间插入节点
    if (current == nullptr || current->getKey() != key) {
        // 先为新节点分配随机的 level/层数
        int randomLevel = getRandomLevel();

        // 如果随机层数大于跳表当前最大的层数，那么就用头节点来初始化update数组
        /*  
            假设当前跳表的最高层级 currentLevel 为 2，而生成的随机层级 randomLevel 为 4。
            在这种情况下，需要扩展 update 数组和更新 currentLevel。
                进入 for 循环，i 的初始值为 currentLevel+1，即 3。
                在第一次迭代中，i 的值为 3，执行 update[3] = heade，即将 update[3] 赋值为头节点。
                在第二次迭代中，i 的值为 4，执行 update[4] = heade，即将 update[4] 赋值为头节点。
            for 循环结束后，update 数组扩展为 [空, 空, 空, 头节点, 头节点]。
            然后，将 currentLevel 更新为 randomLevel，即将 currentLevel 更新为 4。
        */
        if (randomLevel > this->currentLevel) {
            for (int i = this->currentLevel + 1; i <= randomLevel + 1; i++) {
                update[i] = this->head;
            }
            this->currentLevel = randomLevel;
        }

        // 为新节点分配空间
        Node<K,V>* newNode = creatNode(key,value,randomLevel);

        // 插入该节点        
        for (int i = 0; i <= randomLevel; i++) {
            newNode->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = newNode;
        }
        std::cout << "Successfully inserted key:" << key << ", value:" << value << std::endl;
        this->elementCount++;
    }
    mtx.unlock();
    return true;
}

template <typename K, typename V>
bool SkipList<K, V>::removeElement(K key) {
    mtx.lock();

    Node<K,V> *current = this->head;
    Node<K,V> *update[this->currentLevel + 1];
    memset(update, 0, sizeof(Node<K,V> *) * (this->currentLevel + 1));

    // 从高到低进行查找
    for (int i = this->currentLevel; i >= 0; i--) {
        while (current->forward[i] != nullptr && current->forward[i]->getKey() < key) {
            current = current->forward[i];
        }
        update[i] = current;
    }

    current = current->forward[0];
    if (current != nullptr && current->getKey() == key) {
        // 从 level 0 开始，在每一层删除当前的节点
        for (int i = 0; i < this->currentLevel; i++) {
            // 如果下一个节点不是目标节点，则中断循环。
            if (update[i]->forward[i] != current) {
                break;
            }
            // 否则就让需要删除的节点的前一个指向后一个
            update[i]->forward[i] = current->forward[i];
        }

        // 删除没有元素的层级
        while (this->currentLevel > 0 && current->forward[this->currentLevel] == 0) {
            this->currentLevel--;
        }

        std::cout << "Successfully deleted key "<< key << std::endl;
        delete current;
        this->elementCount--;
    }
    mtx.unlock();
    return true;
}

template <typename K, typename V>
bool SkipList<K, V>::searchElement(K key) const {
    std::cout << "----------------- search element -----------------" << std::endl;

    Node<K, V> *current = this->head;
    // 从高到低进行查找
    for (int i = this->currentLevel; i >= 0; i--) {
        while (current->forward[i] != nullptr && current->forward[i]->getKey() < key) {
            current = current->forward[i];
        }
    }

    // 达到 level 0 层时，我们将指针指向右节点
    current = current->forward[0];

    if (current != nullptr && current->getKey() == key) {
        std::cout << "Found key: " << key << ", value: " << current->getValue() << std::endl;
        return true; 
    }

    std::cout << "Not Found Key:" << key << std::endl;
    return false;
}

/* 生成随机层数 */
template <typename K, typename V>
int SkipList<K, V>::getRandomLevel() const {
    int k = 1;
    while (rand() % 2) {
        k++;
    }
    return k < this->maxLevel ? k : this->maxLevel;
}

/* 将内存中的数据转存到文件中 */
template <typename K, typename V>
void SkipList<K, V>::dumpFile() {
    std::cout << "----------------- dump file -----------------" << std::endl;
    fileWriter.open(STORE_FILE,std::ios::in | std::ios::out | std::ios::app | std::ios::binary);
    Node<K, V> *node = this->head->forward[0]; 
    
    // 先定义一个临时的跳表保存落盘前文件所有的键值对
    SkipList<K, V> tempList(this->maxLevel);
    tempList.loadFile();

    while (node != nullptr) {
        fileWriter << node->getKey() << "->" << node->getValue() << std::endl;
        std::cout << node->getKey() << "->" << node->getValue() << std::endl;  
        node = node->forward[0];
    }

    fileWriter.flush();
    fileWriter.close();
    return ;
    
}

/* 将文件中的数据加载到跳表中 */
template <typename K, typename V>
void SkipList<K, V>::loadFile() {
    std::cout << "-----------------load file-----------------" << std::endl;
    fileReader.open(STORE_FILE);
    std::string line;
    std::string* key = new std::string();
    std::string* value = new std::string();
    while (getline(fileReader, line)) {
        get_KV_from_string(line, key, value);
        if (key->empty() || value->empty()) {
            continue;
        }
        insertElement(*key, *value);
        std::cout << "key:" << *key << " value:" << *value << std::endl;
    }
    delete key;
    delete value;
    fileReader.close();
}

/* 获取当前跳表大小 */
template <typename K, typename V>
int SkipList<K, V>::getSize() const {
    return this->elementCount();
}

/* 展示跳表 */
template <typename K, typename V>
void SkipList<K, V>::showSkipList() const {
    std::cout << "\n*****Skip List*****"<< std::endl; 
    for (int i = 0; i <= this->currentLevel; i++) {
        Node<K, V> *node = this->head->forward[i]; 
        std::cout << "Level " << i << ": ";
        while (node != NULL) {
            std::cout << node->getKey() << ":" << node->getValue() << ";";
            node = node->forward[i];
        }
        std::cout << std::endl;
    }
}

template <typename K, typename V>
void SkipList<K, V>::get_KV_from_string(std::string &line, std::string *key, std::string *value) {
    // 如果这行字符串为空返回
    if (line.empty()) {
        return;
    }
    // 如果分隔符找不到返回
    if (line.find(delimiter) == std::string::npos) {
        return;
    }
    *key = line.substr(0, line.find(delimiter));
    *value = line.substr(line.find(delimiter)+2, line.length());
}

#endif

