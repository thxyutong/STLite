#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"
#include <iostream>
#include <cstddef>

namespace sjtu { 

template<typename T>
class list {
	
	struct node {
		node *prev, *next;
		T *data;
		node(node *_prev = NULL, node *_next = NULL, T *_data = NULL) :
			prev(_prev), next(_next), data(_data) {
		}
		~node() {
			if(data != NULL) delete data;
		}
	};
	
	size_t current_size;
	node *head, *tail;
	
public:
	class const_iterator;
	class iterator {
		friend class list<T>;
	private:
		size_t index;
		node *current_node;
		list *belong;
	public:
		iterator(size_t _index = 0, node *_current_node = NULL, const list *_belong = NULL) :
			index(_index), current_node(_current_node), belong((list*)_belong) {}
		iterator operator+(int n) const {
			iterator rtn = *this;
			rtn.index += n;
			while(n > 0) rtn.current_node = rtn.current_node->next, --n;
			while(n < 0) rtn.current_node = rtn.current_node->prev, ++n;
			return rtn;
		}
		iterator operator-(int n) const {return operator+(-n);}
		int operator-(const iterator &rhs) const {
			if(belong != rhs.belong) throw invalid_iterator();
			return index - rhs.index;
		}
		iterator operator+=(const int &n) {return *this = operator+(n);}
		iterator operator-=(const int &n) {return *this = operator-(n);}
		iterator operator++(int) {
			iterator rtn = *this;
			operator+=(1);
			return rtn;
		}
		iterator& operator++() {return *this = operator+(1);}
		iterator operator--(int) {
			iterator rtn = *this;
			operator-=(1);
			return rtn;
		}
		iterator& operator--() {return *this = operator-(1);}
		T& operator*() const{
			if(current_node == belong->tail) throw invalid_iterator();
			return *current_node->data;
		}
		T* operator->() const noexcept {return current_node->data;}
		bool operator==(const iterator &rhs) const {
			if(belong != rhs.belong) return false;
			if(index != rhs.index) return false;
			if(current_node != rhs.current_node) return false;
			return true;
		}
		bool operator==(const const_iterator &rhs) const {
			if(belong != rhs.belong) return false;
			if(index != rhs.index) return false;
			if(current_node != rhs.current_node) return false;
			return true;
		}
		bool operator!=(const iterator &rhs) const {return !operator==(rhs);}
		bool operator!=(const const_iterator &rhs) const {return !operator==(rhs);}
	};
	class const_iterator {
		friend class list<T>;
	private:
		size_t index;
		node *current_node;
		list *belong;
	public:
		const_iterator(size_t _index = 0, node *_current_node = NULL, const list *_belong = NULL) :
			index(_index), current_node(_current_node), belong((list*)_belong) {}
		const_iterator operator+(int n) const {
			const_iterator rtn = *this;
			rtn.index += n;
			while(n > 0) rtn.current_node = rtn.current_node->next, --n;
			while(n < 0) rtn.current_node = rtn.current_node->prev, ++n;
			return rtn;
		}
		const_iterator operator-(int n) const {return operator+(-n);}
		int operator-(const const_iterator &rhs) const {
			if(belong != rhs.belong) throw invalid_iterator();
			return index - rhs.index;
		}
		const_iterator operator+=(const int &n) {return *this = operator+(n);}
		const_iterator operator-=(const int &n) {return *this = operator-(n);}
		const_iterator operator++(int) {
			iterator rtn = *this;
			operator+=(1);
			return rtn;
		}
		const_iterator& operator++() {return *this = operator+(1);}
		const_iterator operator--(int) {
			iterator rtn = *this;
			operator-=(1);
			return rtn;
		}
		const_iterator& operator--() {return *this = operator-(1);}
		const T& operator*() const{
			if(current_node == belong->tail) throw invalid_iterator();
			return *current_node->data;
		}
		const T* operator->() const noexcept {return &(*current_node->data);}
		bool operator==(const iterator &rhs) const {
			if(belong != rhs.belong) return false;
			if(index != rhs.index) return false;
			if(current_node != rhs.current_node) return false;
			return true;
		}
		bool operator==(const const_iterator &rhs) const {
			if(belong != rhs.belong) return false;
			if(index != rhs.index) return false;
			if(current_node != rhs.current_node) return false;
			return true;
		}
		bool operator!=(const iterator &rhs) const {return !operator==(rhs);}
		bool operator!=(const const_iterator &rhs) const {return !operator==(rhs);}
	};
	void copy(const list &other) {
		clear();
		for(const_iterator it = other.cbegin(); it != other.cend(); ++it) push_back(*it);
	}
	list() {
		current_size = 0;
		node *tmp = new node();
		head = new node(NULL, tmp);
		tail = new node(tmp, NULL);
		tmp->prev = head;
		tmp->next = tail;
	}
	list(const list &other) : list() {copy(other);}
	~list() {
		if(head == NULL) return;
		while(head != tail) {
			node *tmp = head;
			head = head->next;
			delete tmp;
		}
		delete tail;
	}
	list &operator=(const list &other) {
		copy(other);
		return *this;
	}
	T & at(const size_t &pos) {
		if(pos >= current_size) throw index_out_of_bound();
		return *(begin() + pos);
	}
	const T & at(const size_t &pos) const {
		if(pos >= current_size) throw index_out_of_bound();
		return *(cbegin() + pos);
	}
	T & operator[](const size_t &pos) {return at(pos);}
	const T & operator[](const size_t &pos) const {return at(pos);}
	const T & front() const {
		if(empty()) throw container_is_empty();
		return *cbegin();
	}
	const T & back() const {
		if(empty()) throw container_is_empty();
		return *(cend() - 1);
	}
	iterator begin() {return iterator(0, head->next, this);}
	const_iterator cbegin() const {return const_iterator(0, head->next, this);}
	iterator end() {return iterator(current_size, tail->prev, this);}
	const_iterator cend() const {return const_iterator(current_size, tail->prev, this);}
	bool empty() const {return current_size == 0;}
	size_t size() const {return current_size;}
	void clear() {while(!empty()) pop_back();}
	void push_back(const T &value) {
		++current_size;
		node *new_node = new node(tail->prev->prev, tail->prev, new T(value));
		tail->prev->prev->next = new_node;
		tail->prev->prev = new_node;
	}
	void push_back(T &&value) {
		++current_size;
		node *new_node = new node(tail->prev->prev, tail->prev, new T(std::move(value)));
		tail->prev->prev->next = new_node;
		tail->prev->prev = new_node;
	}
	void pop_back() {
		if(empty()) throw container_is_empty();
		--current_size;
		node *tmp = tail->prev->prev;
		tmp->next->prev = tmp->prev;
		tmp->prev->next = tmp->next;
		delete tmp;
	}
	void push_front(const T &value) {
		++current_size;
		node *new_node = new node(head, head->next, new T(value));
		head->next->prev = new_node;
		head->next = new_node;
	}
	void pop_front() {
		if(empty()) throw container_is_empty();
		--current_size;
		node *tmp = head->next;
		tmp->next->prev = tmp->prev;
		tmp->prev->next = tmp->next;
		delete tmp;
	}
	iterator insert(iterator pos, const T &value) {
		++current_size;
		node *tmp = pos.current_node;
		node *new_node = new node(tmp->prev, tmp, new T(value));
		tmp->prev->next = new_node;
		tmp->prev = new_node;
		return iterator(pos.index, new_node, this);
	}
	iterator insert(iterator pos, T &&value) {
		++current_size;
		node *tmp = pos.current_node;
		node *new_node = new node(tmp->prev, tmp, new T(std::move(value)));
		tmp->prev->next = new_node;
		tmp->prev = new_node;
		return iterator(pos.index, new_node, this);
	}
	iterator erase(iterator pos) {
		if(pos == end()) return pos;
		--current_size;
		node *tmp = pos.current_node;
		iterator rtn = iterator(pos.index, tmp->next, this);
		tmp->next->prev = tmp->prev;
		tmp->prev->next = tmp->next;
		delete tmp;
		return rtn;
	}
	
	list split(iterator it) {
		size_t cnt = 0;
		for(iterator itt = begin(); itt != it; ++itt, ++cnt);
		list rtn;
		node *tmp_head = it.current_node;
		node *tmp_tail = tail->prev->prev;
		rtn.current_size = current_size - cnt;
		current_size = cnt;
		tmp_head->prev->next = tmp_tail->next;
		tmp_tail->next->prev = tmp_head->prev;
		rtn.head->next = tmp_head;
		rtn.tail->prev->prev = tmp_tail;
		tmp_head->prev = rtn.head;
		tmp_tail->next = rtn.tail->prev;
		return rtn;
	}
	
	void merge(list &other) {
		current_size += other.current_size;
		node *tmp_head = other.head->next;
		node *tmp_tail = other.tail->prev->prev;
		other.head->next = other.tail->prev;
		other.tail->prev->prev = other.head;
		tmp_head->prev = tail->prev->prev;
		tmp_tail->next = tail->prev;
		tail->prev->prev->next = tmp_head;
		tail->prev->prev = tmp_tail;
	}
};

template<class T>
class deque {
	
	typedef list<T*> inner_list_type;
	typedef list<inner_list_type> outer_list_type;
	
	friend class iterator;
	friend class const_iterator;
	size_t current_size;
	outer_list_type outer_list;
	const static size_t SIZE = 2048;
	const static size_t BUFF_SIZE_HGH = SIZE;
	const static size_t BUFF_SIZE_LOW = BUFF_SIZE_HGH / 2;
	
	void fix(typename outer_list_type::iterator it) {
		typename outer_list_type::iterator itt = it + 1;
		if(it->size() > BUFF_SIZE_HGH) outer_list.insert(it + 1, (*it).split((*it).begin() + BUFF_SIZE_LOW));
		if(it == outer_list.begin() || itt == outer_list.end()) {
			if(!it->empty()) return;
			outer_list.erase(it);
		} else if(it->size() < BUFF_SIZE_LOW) {
			if(itt->size() <= BUFF_SIZE_LOW) it->merge(*itt), outer_list.erase(itt);
			else it->push_back(itt->front()), itt->pop_front();
		}
	}
	
public:
	class const_iterator;
	class iterator {
		friend class deque;
	private:
		size_t index;
		typename outer_list_type::iterator it_on_outer_list;
		typename inner_list_type::iterator it_on_inner_list;
		deque *belong;
	public:
		iterator() {}
		iterator(size_t _index, typename outer_list_type::iterator _it_on_outer_list,
			typename inner_list_type::iterator _it_on_inner_list, const deque *_belong) :
			index(_index), it_on_outer_list(_it_on_outer_list), it_on_inner_list(_it_on_inner_list),
			belong((deque*)_belong) {}
		void step_over() {
			size_t cnt = 0;
			bool from_begin = index <= belong->current_size / 2;
			size_t limit = from_begin ? index : belong->current_size - index;
			if(from_begin) for(it_on_outer_list = belong->outer_list.begin();
				it_on_outer_list != belong->outer_list.end(); ++it_on_outer_list) {
				if(cnt + it_on_outer_list->size() > limit) break;
				else cnt += it_on_outer_list->size();
			}
			else for(it_on_outer_list = belong->outer_list.end() - 1; 
				it_on_outer_list != belong->outer_list.begin(); --it_on_outer_list) {
				if(cnt + it_on_outer_list->size() > limit) break;
				else cnt += it_on_outer_list->size();
			}
			if(!from_begin) cnt = belong->current_size + 1 - cnt - it_on_outer_list->size();
			it_on_inner_list = it_on_outer_list->begin();
			while(cnt < index && it_on_inner_list != it_on_outer_list->end()) ++it_on_inner_list, ++cnt;
		}
		iterator operator+(const int &n) const {
			iterator rtn = *this;
			rtn.index += n;
			size_t high = it_on_outer_list->end() - it_on_inner_list - 1 + index;
			size_t low = it_on_outer_list->begin() - it_on_inner_list + index;
			if(low <= rtn.index && rtn.index <= high) rtn.it_on_inner_list += n;
			else rtn.step_over();
			return rtn;
		}
		iterator operator-(const int &n) const {return operator+(-n);}
		int operator-(const iterator &rhs) const {
			if(belong != rhs.belong) throw invalid_iterator();
			return index - rhs.index;
		}
		iterator operator+=(const int &n) {return *this = operator+(n);}
		iterator operator-=(const int &n) {return operator+=(-n);}
		iterator operator++(int) {
			iterator tmp = *this;
			operator+=(1);
			return tmp;
		}
		iterator& operator++() {return *this = operator+(1);}
		iterator operator--(int) {
			iterator tmp = *this;
			operator-=(1);
			return tmp;
		}
		iterator& operator--() {return *this = operator-(1);}
		T& operator*() const {
			if(index < 0 || index >= belong->current_size) throw invalid_iterator();
			if(*this == belong->end()) throw invalid_iterator();
			return **it_on_inner_list;
		}
		T* operator->() const noexcept {return &(**it_on_inner_list);}
		bool operator==(const iterator &rhs) const {
			if(index != rhs.index) return false;
			if(it_on_inner_list != rhs.it_on_inner_list) return false;
			if(it_on_outer_list != rhs.it_on_outer_list) return false;
			if(belong != rhs.belong) return false;
			return true;
		}
		bool operator==(const const_iterator &rhs) const {
			if(index != rhs.index) return false;
			if(it_on_inner_list != rhs.it_on_inner_list) return false;
			if(it_on_outer_list != rhs.it_on_outer_list) return false;
			if(belong != rhs.belong) return false;
			return true;
		}
		bool operator!=(const iterator &rhs) const {return !operator==(rhs);}
		bool operator!=(const const_iterator &rhs) const {return !operator==(rhs);}
	};
	class const_iterator {
		friend class deque;
	private:
		size_t index;
		typename outer_list_type::const_iterator it_on_outer_list;
		typename inner_list_type::const_iterator it_on_inner_list;
		deque *belong;
	public:
		const_iterator() {}
		const_iterator(size_t _index, typename outer_list_type::const_iterator _it_on_outer_list,
			typename inner_list_type::const_iterator _it_on_inner_list, const deque *_belong) :
			index(_index), it_on_outer_list(_it_on_outer_list), it_on_inner_list(_it_on_inner_list),
			belong((deque*)_belong) {}
		void step_over() {
			size_t cnt = 0;
			bool from_begin = index <= belong->current_size / 2;
			size_t limit = from_begin ? index : belong->current_size - index;
			if(from_begin) for(it_on_outer_list = belong->outer_list.cbegin();
				it_on_outer_list != belong->outer_list.cend(); ++it_on_outer_list) {
				if(cnt + it_on_outer_list->size() > limit) break;
				else cnt += it_on_outer_list->size();
			}
			else for(it_on_outer_list = belong->outer_list.cend() - 1; 
				it_on_outer_list != belong->outer_list.cbegin(); --it_on_outer_list) {
				if(cnt + it_on_outer_list->size() > limit) break;
				else cnt += it_on_outer_list->size();
			}
			if(!from_begin) cnt = belong->current_size + 1 - cnt - it_on_outer_list->size();
			it_on_inner_list = it_on_outer_list->cbegin();
			while(cnt < index && it_on_inner_list != it_on_outer_list->cend()) ++it_on_inner_list, ++cnt;
		}
		const_iterator operator+(const int &n) const {
			const_iterator rtn = *this;
			rtn.index += n;
			size_t high = it_on_outer_list->cend() - it_on_inner_list - 1 + index;
			size_t low = it_on_outer_list->cbegin() - it_on_inner_list + index;
			if(low <= rtn.index && rtn.index <= high) rtn.it_on_inner_list += n;
			else rtn.step_over();
			return rtn;
		}
		const_iterator operator-(const int &n) const {return operator+(-n);
		}
		int operator-(const const_iterator &rhs) const {
			if(belong != rhs.belong) throw invalid_iterator();
			return index - rhs.index;
		}
		const_iterator operator+=(const int &n) {return *this = operator+(n);}
		const_iterator operator-=(const int &n) {return operator+=(-n);}
		const_iterator operator++(int) {
			const_iterator tmp = *this;
			operator+=(1);
			return tmp;
		}
		const_iterator& operator++() {return *this = operator+(1);}
		const_iterator operator--(int) {
			const_iterator tmp = *this;
			operator-=(1);
			return tmp;
		}
		const_iterator& operator--() {return *this = operator-(1);}
		const T& operator*() const {
			if(index < 0 || index >= belong->current_size) throw invalid_iterator();
			if(*this == belong->cend()) throw invalid_iterator();
			return **it_on_inner_list;
		}
		const T* operator->() const noexcept {return &(**it_on_inner_list);}
		bool operator==(const iterator &rhs) const {
			if(index != rhs.index) return false;
			if(it_on_inner_list != rhs.it_on_inner_list) return false;
			if(it_on_outer_list != rhs.it_on_outer_list) return false;
			if(belong != rhs.belong) return false;
			return true;
		}
		bool operator==(const const_iterator &rhs) const {
			if(index != rhs.index) return false;
			if(it_on_inner_list != rhs.it_on_inner_list) return false;
			if(it_on_outer_list != rhs.it_on_outer_list) return false;
			if(belong != rhs.belong) return false;
			return true;
		}
		bool operator!=(const iterator &rhs) const {return !operator==(rhs);}
		bool operator!=(const const_iterator &rhs) const {return !operator==(rhs);}
	};
	deque() {
		current_size = 0;
		inner_list_type inner_list;
		inner_list.push_back(NULL);
		outer_list.push_back(inner_list);
	}
	void copy(const deque &other) {
		if(this == &other) return;
		clear();
		for(const_iterator it = other.cbegin(); it != other.cend(); ++it)
			push_back(*it);
	}
	deque(const deque &other) : deque() {copy(other);}
	~deque() {
		for(iterator it = begin(); it != end(); ++it)
			delete *it.it_on_inner_list;
	}
	deque &operator=(const deque &other) {
		copy(other);
		return *this;
	}
	T & at(const size_t &pos) {
		if(pos >= current_size) throw index_out_of_bound();
		return *(begin() + pos);
	}
	const T & at(const size_t &pos) const {
		if(pos >= current_size) throw index_out_of_bound();
		return *(cbegin() + pos);
	}
	T & operator[](const size_t &pos) {return at(pos);}
	const T & operator[](const size_t &pos) const {return at(pos);}
	const T & front() const {
		if(empty()) throw container_is_empty();
		return *cbegin();
	}
	const T & back() const {
		if(empty()) throw container_is_empty();
		return *(cend() - 1);
	}
	iterator begin() {return iterator(size_t(0), outer_list.begin(), outer_list.begin()->begin(), this);}
	const_iterator cbegin() const {return const_iterator(size_t(0), outer_list.cbegin(), outer_list.cbegin()->cbegin(), this);}
	iterator end() {
		auto it = outer_list.end() - 1;
		return iterator(current_size, it, it->end() - 1, this);
	}
	const_iterator cend() const {
		auto it = outer_list.cend() - 1;
		return const_iterator(current_size, it, it->cend() - 1, this);
	}
	bool empty() const {return current_size == 0;}
	size_t size() const {return current_size;}
	void clear() {
		for(iterator it = begin(); it != end(); ++it) {
			if(*(it.it_on_inner_list) == NULL) continue;
			delete *(it.it_on_inner_list);
			*(it.it_on_inner_list) = NULL;
		}
		current_size = 0;
		outer_list.clear();
		inner_list_type inner_list;
		inner_list.push_back(NULL);
		outer_list.push_back(inner_list);
	}
	void push_back(const T &value) {
		auto it_on_outer_list = end().it_on_outer_list;
		auto it_on_inner_list = end().it_on_inner_list;
		it_on_outer_list->insert(it_on_inner_list, new T(value));
		++current_size;
		fix(it_on_outer_list);
	}
	void push_back(T &&value) {
		auto it_on_outer_list = end().it_on_outer_list;
		auto it_on_inner_list = end().it_on_inner_list;
		it_on_outer_list->insert(it_on_inner_list, new T(std::move(value)));
		++current_size;
		fix(it_on_outer_list);
	}
	void pop_back() {
		if(empty()) throw container_is_empty();
		auto it = end() - 1;
		auto it_on_outer_list = it.it_on_outer_list;
		auto it_on_inner_list = it.it_on_inner_list;
		delete *it_on_inner_list;
		it_on_outer_list->erase(it_on_inner_list);
		--current_size;
		fix(it_on_outer_list);
	}
	void push_front(const T &value) {
		auto it_on_outer_list = begin().it_on_outer_list;
		it_on_outer_list->push_front(new T(value));
		++current_size;
		fix(it_on_outer_list);
	}
	void pop_front() {
		if(empty()) throw container_is_empty();
		delete *begin().it_on_inner_list;
		auto it_on_outer_list = begin().it_on_outer_list;
		it_on_outer_list->pop_front();
		--current_size;
		fix(it_on_outer_list);
	}
	iterator insert(iterator pos, const T &value) {
		if(pos.belong != this) throw invalid_iterator();
		if(pos.index > current_size) throw invalid_iterator();
		pos = begin() + pos.index;
		if(pos == begin()) {push_front(value);return begin();}
		if(pos == end()) {push_back(value); return end() - 1;}
		pos.it_on_outer_list->insert(pos.it_on_inner_list, new T(value));
		++current_size;
		fix(pos.it_on_outer_list);
		return begin() + pos.index;
	}
	iterator insert(iterator pos, T &&value) {
		if(pos.belong != this) throw invalid_iterator();
		if(pos.index > current_size) throw invalid_iterator();
		pos = begin() + pos.index;
		if(pos == begin()) {push_front(value); return begin();}
		if(pos == end()) {push_back(value); return end() - 1;}
		pos.it_on_outer_list->insert(pos.it_on_inner_list, new T(std::move(value)));
		++current_size;
		fix(pos.it_on_outer_list);
		return begin() + pos.index;
	}
	iterator erase(iterator pos) {
		if(pos.belong != this) throw invalid_iterator();
		if(pos.index >= current_size) throw invalid_iterator();
		pos = begin() + pos.index;
		if(pos == end()) return pos;
		if(pos == begin()) {pop_front(); return begin();}
		if(pos == end() - 1) {pop_back(); return end();}
		--current_size;
		delete *pos.it_on_inner_list;
		pos.it_on_outer_list->erase(pos.it_on_inner_list);
		fix(pos.it_on_outer_list);
		return begin() + pos.index;
	}
};

}

#endif
