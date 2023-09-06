#include "SkipList.hpp"

int main() {

    SkipList<int, std::string> skipList(6);
	skipList.insertElement(1, "我"); 
	skipList.insertElement(3, "爱学"); 
	skipList.insertElement(7, "C++"); 
	skipList.insertElement(8, "没有人比我更加爱学C++"); 
	skipList.insertElement(9, "学习"); 
	skipList.insertElement(19, "是提升生产力的方式"); 
	skipList.insertElement(21, "轰动，干巴得！"); 
    skipList.showSkipList();

    skipList.searchElement(9);
    skipList.searchElement(18);

    skipList.removeElement(3);
    skipList.removeElement(7);

    skipList.showSkipList();
    
    skipList.dumpFile();
    skipList.loadFile();

    skipList.showSkipList();
}
