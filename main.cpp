/*
 * @Author: XiaChuerwu 1206575349@qq.com
 * @Date: 2023-09-06 21:34:29
 * @LastEditors: XiaChuerwu 1206575349@qq.com
 * @LastEditTime: 2023-09-07 10:17:24
 * @FilePath: \SkipList\main.cpp
 */


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
