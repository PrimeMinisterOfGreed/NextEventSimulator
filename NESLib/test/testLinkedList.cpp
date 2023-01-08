#include <gtest/gtest.h>
#include <functional>
#include "LinkedList.hpp"



TEST(test_linked_list, test_enqueue)
{
	DoubleLinkedList<int> linkedList{};
	linkedList.Enqueue(new int(1));
	linkedList.Enqueue(new int(2));
	linkedList.Enqueue(new int(3));

	auto itr = linkedList.begin();
	itr++;
	assert(*itr == 2);
	assert(*linkedList.begin() == 1);
	assert(*linkedList.end() == 3);

	for (int i = 1; i < 4; i++)
		assert(linkedList.Dequeue() == i);
	
}

TEST(test_linked_list, test_push)
{
	DoubleLinkedList<int> linkedList{};
	linkedList.Push(new int(1));
	linkedList.Push(new int(2));
	linkedList.Push(new int(3));

	auto itr = linkedList.begin();
	itr++;
	assert(*itr == 2);
	assert(*linkedList.begin() == 3);
	assert(*linkedList.end() == 1);

	for (int i = 0; i < 3; i++)
		assert(linkedList.Pull() == 3 - i);

}


TEST(test_linked_list, test_insertion)
{
	DoubleLinkedList<int> linkedList{};
	std::function<bool(int, int)> comparator = [](int a, int b) { return a > b; };
	linkedList.Insert(new int(2), comparator);
	linkedList.Insert(new int(3), comparator);
	linkedList.Insert(new int(1), comparator);
	linkedList.Insert(new int(5), comparator);
	linkedList.Insert(new int(4), comparator);

	auto itr = linkedList.begin();
	itr++;
	assert(*itr == 2);
	itr++;
	assert(*itr == 3);
	assert(*linkedList.begin() == 1);
	assert(*linkedList.end() == 5);
}

