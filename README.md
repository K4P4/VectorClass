# 1 Užduotis

## reserve
```
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
```
## shrink_to_fit
```
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
```

## resize
```
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
```

## insert
```
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
```

## erase
```
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
  ```
  
# 2 Užduotis

| Elementų kiekis | std::vector | Vector class |
| --------------  | ----------- | ------------ |
| 100000          | 0.002s      | 0.001s       |
| 1000000         | 0.016s      | 0.001s       |
| 10000000        | 0.169s      | 0.127s       |
| 100000000       | 1.605s      | 1.038s       |

# 3 Užduotis

Atminties perskirstymai:

| Elementų kiekis | std::vector | Vector class |
| --------------  | ----------- | ------------ |
| 100000000       | 28 kartai   | 25 kartai    |

# 4 Užduotis

Trukmės su partition:

| Elementų kiekis | std::vector | Vector class |
| --------------  | ----------- | ------------ |
| 100000          | 0.831s      | 0.904s       |
