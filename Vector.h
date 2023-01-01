#pragma once


template <typename T>
class Vector
{
private:

	T* m_Data; // String* m_Data[100]
	int m_index; // hint - monitor the index 
	int m_size;
	int m_count; 


	T* resizeby2()
	{
		T* temp = new T[m_size * 2];
		for (int i = 0; i < m_size; i++) 
		{
			temp[i] = m_Data[i];
		}
		delete[] m_Data;
		m_Data = 0; 
		m_size = m_size * 2; 
		return temp; 
	}


public:
	
	static constexpr int DEFAULT_SIZE = 10;


	Vector() : m_Data{ 0 }, m_index{ 0 }, m_size{ Vector::DEFAULT_SIZE }, m_count{ 0 }
	{
		m_Data = new T[Vector::DEFAULT_SIZE];
	}

	~Vector() 
	{
		if (m_Data) 
		{
			delete[] m_Data;
			m_Data = 0;
		}
	} 


	void Insert(const T& d, int idx)
	{
		m_Data[idx] = d;
		m_count++;
	}


	void PushBack(const T& item)
	{
		if (m_index < m_size) {
			m_Data[m_index++] = item;
		}
		else {
			m_Data = resizeby2();
			m_Data[m_index++] = item;
		}
		m_count++; 
	}

	//function to find an element in the vector
	// find first element that match
	// todo: viki - designa  fucntion to find an element

	int Size() const { return m_size;  }

	int Count() const { return m_count;  }

	T& operator[](int index)
	{
		if (index >= 0 && index < m_size)
			return m_Data[index];
	}

	T* data() { return m_Data;  }

};