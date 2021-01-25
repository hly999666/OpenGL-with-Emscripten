  #pragma once
 #include <iostream>
#include <memory>
   //Returns r-value
 #ifndef Optr_ol_H
#define Optr_ol_H

 void Optr_ol_ok();
class Integer_opol;
class good_friend;
void  plus_by_friend_fun(Integer_opol& i,int a);

class Integer_opol {
	int *m_pInt;
public:
	//Default constructor
	Integer_opol();
	//Parameterized constructor
	//using explicit to avoid implicit cast
	explicit Integer_opol(int value);
	//Copy constructor
	Integer_opol(const Integer_opol &obj);
	//Move constructor
	Integer_opol(Integer_opol &&obj);
	int GetValue()const;
	void SetValue(int value);
	~Integer_opol();
	// this is ++a,return lvalue
	Integer_opol & operator ++();
	// this is  a++,return rvalue
	Integer_opol operator ++(int);
	bool operator ==(const Integer_opol &a)const;

	//Copy assignment
	Integer_opol & operator =(const Integer_opol &a);
	//Move assignment
	Integer_opol & operator =(Integer_opol &&a);
	Integer_opol operator +(const Integer_opol & a)const;
	//	friend mean can access private element in class
    friend  void  plus_by_friend_fun(Integer_opol& i,int a);
	//friend class as the same
	friend class good_friend;
	void operator ()();
	//cast to primary operator ,not very useful
	operator int();
};
Integer_opol operator +(int x, const Integer_opol &y);
std::ostream & operator <<(std::ostream & out, const Integer_opol &a);
std::istream & operator >> (std::istream &input, Integer_opol &a);



//example of a smart pointer
class IntPtr {
	Integer_opol *m_p;
public:
	IntPtr(Integer_opol *p) :m_p(p) {}
	~IntPtr() {
		delete m_p;
	}
	Integer_opol *operator ->() {
		return m_p;
	}
	Integer_opol & operator *() {
		return *m_p;
	}
};
void CreateInteger(); 
 #endif
 