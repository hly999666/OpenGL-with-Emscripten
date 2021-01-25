#include "Optr_ol.hpp"
 
#include <iostream>

 
 void Optr_ol_ok(){
	 std::cout<<"Optr_ol_ok"<<std::endl;
 }


Integer_opol::Integer_opol() {
	std::cout << "Integer_opol()" << std::endl;
	m_pInt = new int(0);
}

 Integer_opol::Integer_opol(int value) {
	std::cout << "Integer_opol(int)" << std::endl;
	m_pInt = new int(value);
}

Integer_opol::Integer_opol(const Integer_opol & obj) {
	std::cout << "Integer_opol(const Integer_opol&)" << std::endl;
	m_pInt = new int(*obj.m_pInt);
}

Integer_opol::Integer_opol(Integer_opol && obj) {
	std::cout << "Integer_opol(int&&)" << std::endl;
	m_pInt = obj.m_pInt;
	obj.m_pInt = nullptr;
}

int Integer_opol::GetValue() const {
	return *m_pInt;
}

void Integer_opol::SetValue(int value) {
	*m_pInt = value;
}

Integer_opol::~Integer_opol() {
	std::cout << "~Integer_opol() " <<*m_pInt<< std::endl;
	delete m_pInt;
}

Integer_opol & Integer_opol::operator++() {
	++(*m_pInt);
	return *this;
	// TODO: insert return statement here
}

Integer_opol  Integer_opol::operator++(int) {
	Integer_opol temp(*this);
	++(*m_pInt);
	return temp;
}

bool Integer_opol::operator==(const Integer_opol & a) const {
	return *m_pInt == *a.m_pInt;
}

Integer_opol & Integer_opol::operator=(const Integer_opol & a) {
	  //note test assign is the same to avoid self-destorying when a=a
	if (this != &a) {
		delete m_pInt;
		m_pInt = new int(*a.m_pInt);
	}
	return *this;
}

Integer_opol & Integer_opol::operator=(Integer_opol && a) {
  //note test assign is the same to avoid self-destorying when a=a
	if (this != &a) {
		delete m_pInt;
		m_pInt = a.m_pInt;
		a.m_pInt = nullptr;
	}
	return *this;
}



Integer_opol Integer_opol::operator+(const Integer_opol & a) const {
	Integer_opol temp;
	*temp.m_pInt = *m_pInt + *a.m_pInt;
	return temp;
}

void Integer_opol::operator()() {
	std::cout << *m_pInt << std::endl; 
}

Integer_opol operator +(int x, const Integer_opol &y) {
	Integer_opol temp;
	temp.SetValue(x + y.GetValue());
	return temp;
}

Integer_opol::operator int(){
   return *m_pInt;

}

std::ostream & operator <<(std::ostream & out, const Integer_opol &a) {
	out << a.GetValue();
	return out;
}

std::istream & operator >> (std::istream &input, Integer_opol &a) {
	int x;
	input >> x;
	a.SetValue(x);
	return input;
}
void  plus_by_friend_fun(Integer_opol& i,int a){
  *(i.m_pInt)+=a;
}

void CreateInteger() {
	std::unique_ptr<Integer_opol> p(new Integer_opol(3334));
	//below can't pass ompile unique_ptr can't copy
   // auto p2(p);
   // below is OK
   auto p2=std::move(p);

   std::shared_ptr<Integer_opol> sp(new Integer_opol(555));
   auto sp2=sp;
   //Runtime BUG!!!,Because address in p is nullptr set by std::move above
	//(*p).SetValue(3);
	std::cout << sp->GetValue() << std::endl; 
}

