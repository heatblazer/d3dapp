#pragma once
#include <iostream>

template <typename T>
class List
{
	struct Node
	{
		T value; 

		Node* link; // [o]-> [0] -> [0] -> NULL
		Node(const T& data) : value{ data }, link{ nullptr } { }
	};

	// Begin (NODE) ........... END (NODE)

	Node* m_head; // begin 
	Node* m_tail; // end -> NULL

	int m_size; 

	void Cleanup()
	{
		while (m_head)
		{
			Pop();
		}
	}

public:

	List() : m_head{ nullptr }, m_tail{ nullptr }, m_size{ 0 }
	{

	}

	~List() 
	{
		Cleanup();
	}

	//VIKI : implement the logic for size 
	int Size()
	{
		return m_size; 
	}


	//task 2: Think how to reverse the stack - 9,8,7,6,5,4...1 to be 1.., 4,5,6,7,8,9

	T Peek()
	{
		return m_head->value; 
	}


	T Pop()
	{
		Node* it = m_head;
		T value = it->value;
		m_head = m_head->link; // what happens here?
		delete it;
		return value;
	}


	void Push(const T& item)
	{
		Node* n = new Node{ item };		
		n->link = m_head; // no elements found - we set the new node to be head and tail 
		m_head =  n;		
	}

	void Insert(const T& item)
	{
		Node* n = new Node{ item };
		if (!m_head) {
			n->link = m_head; // no elements found - we set the new node to be head and tail 
			m_head = m_tail = n;
		}
		else {
			m_tail->link = n; //tail -> new element 
			m_tail = n;      // new element is always last or tail
		}
	}

	void Print()
	{
		Node* it = m_head; // copy to the beginning 
		while (it != nullptr)
		{
			std::cout << it->value << "\r\n";
			it = it->link;
		}

	}

	void Reverse()
	{
		Node* it = m_head;
		Node* next = nullptr, * prev = nullptr;

		while (it)
		{
			next = it->link;  // 1 -> 2   ... 2 = 1 ->2
			it->link = prev; 
			prev = it;  // --- >
			it = next;  // <--- 
		}
		m_head = prev; 
	}


	void Reverse2()
	{
		List<T> n;
		Node* it = m_head;
		while (it)
		{
			n.Push(it->value);
			it = it->link;
		}
		Cleanup();  //ok

		Node* it2 = n.m_head;

		while (it2)
		{
//viki - think what to do here
			it2 = it2->link; 
		}


	}


};