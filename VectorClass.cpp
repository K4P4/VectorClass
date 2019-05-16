#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <iterator>
#include <algorithm>

using std::string;
using std::cerr;
using std::endl;
using std::cout;
using std::cin;
using namespace std::chrono;

const int defaultSize = 2;

//naudojama kaip susinaikinantis pointeris
template <typename T>
class auto_array{
    private :
        T *ptr_;
    public :
        auto_array(T *t) : ptr_( t )
        { }

        ~auto_array()
        { delete[] ptr_; }

        T *operator->()
        { return ptr_; }

        T *release()
        {
            T *tmp( ptr_ );
            ptr_ = 0;
            return tmp;
        }

        T &operator[](int i)
        { return ptr_[i]; }
};

template <typename T>
class Vector
{
    private :
        T *array_;
        int size_;
        int reserved_size_;
    public:
        Vector() //Default konstruktorius kai nezinomas pradinis elementu kiekis;
            : array_( new T[defaultSize]),
            reserved_size_(defaultSize),
            size_(0) { }

        Vector(int n) //Konstruktorius kai zinomas pradinis elementu kiekis;
            : array_( new T[n] ),
            reserved_size_( n ),
            size_( 0 ) { }

        ~Vector() //Destruktorius
        { delete[] array_; }

		typedef T* iterator;

        //metodai
        void assign(int n, T value);
        void push_back(const T &t);
        void pop_back();
        iterator insert(iterator it, T value);
        void insert(iterator it, int n, T value);
        iterator erase(iterator it);
        iterator erase(iterator it1, iterator it2);
        void clear();

        T* data(){return array_;}

        void resize(int n, T val = 0);
        int size(){return size_;}
        int capacity(){return reserved_size_;}
        bool empty(){if(size_ > 0) return false; else return true;};
        void shrink_to_fit();
        void reserve(int n);

        //nuorodos
        T& at(int it) {
		if (it < 0 || size_ < it) throw std::out_of_range {"Vector::reference_at"};
		return array_[it];
        }
        T& back(){return array_[size_-1];}
        T& front(){return array_[0];}

		T* begin(){ return array_;};
		T* end(){return array_+size_;};


        //operatoriai
        T& operator[](int i) {
        if(i < size_ && i >= 0){ return array_[i]; }
        else throw std::out_of_range {"Vector::operator[]"};
        }
	};

	template <typename T>
    void Vector<T>::reserve(int n)
    {
    if(n > reserved_size_) {
        auto_array<T> new_array( new T[n] ); //sukuria nauja reikiamo dydzio masyva
        for(int i=0; i<size_; i++){ //perkopijuoja sena masyva i naujaji
            new_array[i] = array_[i];
        }

        delete[] array_; //istrina sena masyva
        array_ = new_array.release();
        reserved_size_ = n;
    }
}

	template <typename T>
    void Vector<T>::shrink_to_fit()
    {
    if(size_ < reserved_size_){
        for(int i = size_; i < reserved_size_; i++){
            delete &array_[i];
        }
        reserved_size_ = size_;
    }
}

    template <typename T>
    void Vector<T>::push_back(const T &t)
    {
        if(size_ == reserved_size_)
            reserve(reserved_size_*2 + defaultSize);

        array_[size_] = t;

        size_++;
    }

    template <typename T>
    void Vector<T>::resize(int n, T val)
    {
        if(n > 0){
        auto_array<T> new_array( new T[n] ); //sukuria nauja reikiamo dydzio masyva
        int k;
        if(size_ > n) k = n; else k = size_;
        for(int i=0; i<k; i++){ //perkopijuoja sena masyva i naujaji
            new_array[i] = array_[i];
        }
        for(int i=k; i<n; i++){
            new_array[i] = val;
        }

        delete[] array_; //istrina sena masyva
        array_ = new_array.release();
        if(n > reserved_size_) reserved_size_ = n;
        size_ = n;
        }
}

    template <typename T>
        void Vector<T>::pop_back()
        {
        if(size_ > 0){
            delete &array_[size_];
            size_--;
        }
    }

    template <typename T>
    void Vector<T>::assign(int n, T value){
            if (n < 0) throw std::out_of_range {"Vector::assign"};
            delete[] array_;
            size_ = n;
            reserved_size_ = size_;
            array_ = new T[size_];
            std::fill_n(array_, size_, value);
    }

    template <typename T>
    T* Vector<T>::insert(iterator it, T value){
		int i = 0;
		if (size_ < reserved_size_) {
			for(iterator itr = array_ + size_; itr != it; itr--, i++)
				array_[size_ - i] = array_[size_ - i - 1];
			*it = value;
			size_++;
		}
		else {
			auto_array<T> new_array(new T[size_ + 1]); //sukuria nauja reikiamo dydzio masyva

			for (iterator itr = array_; itr != it; itr++, i++)
				new_array[i] = array_[i];
			new_array[i] = value;
			i++;
			for (iterator itr = array_ + i + 1; itr != array_ + size_ + 2; itr++, i++)
				new_array[i] = array_[i - 1];

			delete[] array_; //istrina sena masyva
			array_ = new_array.release();
			size_++;
			reserved_size_ = size_;
		}
    }

    template <typename T>
    void Vector<T>::insert(iterator it, int n, T value){
        int i = 0;
		if (size_ + n < reserved_size_) {
			for (iterator itr = array_ + size_ + n-1; itr != it+n-1; itr--, i++)
				array_[size_ - i + n - 1] = array_[size_ - i - 1];
			for (int j = 0; j < n; j++, i++) {
				array_[size_ - i + n - 1] = value;
			}
			size_++;
		}
		else {
			auto_array<T> new_array(new T[size_ + n]); //sukuria nauja reikiamo dydzio masyva
			int i = 0, j = 0;

			for (iterator itr = array_; itr != it; itr++, i++)
				new_array[i] = array_[i];

			for (j; j < n; j++)
				new_array[i + j] = value;

			for (iterator itr = array_ + i; itr != array_ + size_; itr++, i++)
				new_array[i + j] = array_[i];

			delete[] array_; //istrina sena masyva
			array_ = new_array.release();
			size_ = size_ + n;
			if (reserved_size_ < size_) reserved_size_ = size_;
		}
    }

    template <typename T>
    void Vector<T>::clear(){
		if(size_ > 0) {
			size_ = 0;
		}
	}

	template <typename T>
    T* Vector<T>::erase(iterator it){
		int i = 0;

		for (auto itr = (*this).begin(); itr != it; itr++, i++);
		for (auto itr = it + 1; itr != (*this).end(); itr++, i++)
			array_[i] = array_[i + 1];

		size_--;
		return it;
    }

    template <typename T>
    T* Vector<T>::erase(iterator it1, iterator it2) {
		int i = 0;
		int temp = 0;

		for (auto itr = (*this).begin(); itr != it1; itr++, i++);
		for (auto itr = it1; itr != it2; itr++, temp++, i++);

		for (auto itr = it2; itr != (*this).end(); itr++, i++)
			array_[i - temp] = array_[i];

		size_ -= temp;
		return it2;
	}

class zmogus{
protected:
    string vardas;
    string pavarde;
public:
    zmogus(string defv = "", string defp = ""): vardas{defv}, pavarde{defp}{}
    virtual void setEgzaminas() = 0;
    void setVardas(string vard) { vardas = vard;}
	void setPavarde(string pav) { pavarde = pav;}
    inline string getVardas() const { return vardas;}
    inline string getPavarde() const { return pavarde;}
};

class stud: public zmogus{
protected:
    int egz;
    Vector<int> nd;
    double vidurkis;
public:
    stud() : vidurkis(0){}
    stud(std::istream& is);
    double getVidurkis() const { return vidurkis;}
	void pushNd(int pazymys) { nd.push_back(pazymys);}
	void setEgzaminas();
	void apskVidurki();
	void addVidurkis(int pazymys);
    friend bool operator == (const stud &st1, const stud& st2) { return st1.vidurkis == st2.vidurkis;}
    friend bool operator != (const stud &st1, const stud& st2) { return st1.vidurkis != st2.vidurkis;}
    friend std::istream & operator >> (std::istream & in, stud & st);
    friend std::ostream & operator << (std::ostream & out, const stud &st);
    stud (const stud& a);
    stud& operator=(const stud& st);
    stud(stud&& st) : zmogus{st.vardas,st.pavarde}, egz{st.egz}, vidurkis{st.vidurkis},nd{std::move(st.nd)} {};
    friend bool operator < (const stud &st1, const stud &st2) { return st1.vidurkis < st2.vidurkis;}
    friend bool operator > (const stud &st1, const stud &st2) { return st1.vidurkis > st2.vidurkis;}
};

int ivedimas();


std::istream& operator >> (std::istream & in, stud & st){
        cout << "Pasibaigus namu darbu eilutei iveskite neigiama skaiciu (pvz. -1)" << endl;
        cout << "Iveskite studento varda: ";
        cin >> st.vardas;
        cout << "Pavarde: ";
        cin >> st.pavarde;
        cout << "Namu darbu rezultatus:" << endl;
        st.vidurkis = 0;
        int k=1;
            while(k >= 0){
                k = ivedimas();
                if(k > 0){
                    st.nd.push_back(k);
                    st.vidurkis += k;
                }
            }
        cout << "Egzamino ivertinima: ";
        st.egz = 0;
        while(st.egz <= 0) st.egz = ivedimas();
        return in;
}

std::ostream & operator << (std::ostream & out, const stud &st){
        out << "Vardas: " << st.vardas << endl;
        out << "Pavarde: " << st.pavarde << endl;
        out << "Vidurkis: " << st.vidurkis << endl;
        return out;
}

stud& stud::operator=(const stud& st){
	if (&st == this) return *this;
	vardas = st.vardas;
	pavarde = st.pavarde;
	egz = st.egz;
    nd = st.nd;
	vidurkis = st.vidurkis;
	return *this;
}

stud::stud(const stud& st){
	vardas = st.vardas;
	pavarde = st.pavarde;
	egz = st.egz;
    nd = st.nd;
	vidurkis = st.vidurkis;
}

void stud::setEgzaminas(){
    egz = nd[nd.size()-1];
    nd.pop_back();
}

void stud::apskVidurki(){
    vidurkis = vidurkis/nd.size()*0.4 + egz*0.6;
}

void stud::addVidurkis(int pazymys){
    vidurkis += pazymys;
}

int ivedimas(){
    int k = 0;
    cin >> k;
        if(cin.fail()){
            cin.clear();
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            cerr << "Klaida. Ivestas simbolis nera skaitmuo" << endl;
            return 0;
        }
    else return k;
}

void nuskaitymasFaile(int& sk, Vector<stud>& studentai){
    std::ifstream in("sugeneruota100.txt");
    string eil;
    if(!in.good()){
        cerr << "Klaida. Toks failas neegzistuoja" << endl;
        exit(1);
    }
    int temp;
    sk = 0;
        while(in >> eil){
            stud studentas;
            studentas.setVardas(eil);
            in >> eil;
            studentas.setPavarde(eil);
            while(in.peek()!='\n' && in >> temp){
                if(in.fail() || temp <= 0) {
                    cerr << "Klaida. Patikrinkite faila" << endl;
                    exit(1);
                }
                studentas.pushNd(temp);
                studentas.addVidurkis(temp);
            }
            studentas.setEgzaminas();
            studentas.apskVidurki();
            studentai.push_back(studentas);
            sk++;
        }
        in.close();
}

bool maziau(const stud& esm) {
	return (esm.getVidurkis() < 5.0);
}



int main(){
    Vector<stud> studentai;
    int sk = -1;
    auto start = high_resolution_clock::now();

    nuskaitymasFaile(sk, studentai);

    Vector<stud> vargs;
    Vector<stud> kieti;

    for(int i = 0; i < studentai.size(); i++){
        if(studentai[i].getVidurkis() >= 5.0) kieti.push_back(studentai[i]);
        else vargs.push_back(studentai[i]);
    }
    auto it1 = studentai.begin();
    auto it2 = studentai.end();

    std::partition(it1, it2, maziau);

    for(int i = 0; i < studentai.size(); i++){
        std::cout << i << " "<<studentai[i].getVidurkis() << endl;
    }
    std::cout << studentai.size() << endl;
    studentai.erase(studentai.begin()+10, studentai.end());
    studentai.shrink_to_fit();
    for(int i = 0; i < studentai.size(); i++){
        std::cout << i << " "<<studentai[i].getVidurkis() << endl;
    }
    std::cout << studentai.size() << endl;


    auto ending = high_resolution_clock::now();

    duration<double> diff = ending-start;
    std::cout << sk << " studentu isskirstymas uztruko: " << diff.count() << "s" << endl;
}
